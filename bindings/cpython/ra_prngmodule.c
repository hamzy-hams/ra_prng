// ra_prngmodule.c
// Python C extension wrapper for RA_PRNG struct
// Copyright (c) 2025 Hamas A. Rahman
// Licensed under CC BY-NC-SA 4.0
// Build with: python setup.py build_ext --inplace

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdint.h>
#include <string.h>

#define STATE_SIZE 256

// Rotate a 32-bit value n by r bits
static inline uint32_t rot32(uint32_t n, uint32_t r) {
    r &= 31u;
    return ((n << r) | (n >> (32 - r))) & 0xFFFFFFFFu;
}

// Hash M[256] -> out8[8]: nonlinear XOR reduction
static void ra_hash(const uint32_t *N, uint32_t *out8) {
    uint32_t idx;
    for (uint8_t i = 0; i < 8; ++i) out8[i] = 0u;
    for (uint8_t i = 0; i < 8; ++i) {
        idx = N[i] & 0xFF;
        out8[i] ^= N[idx];
        for (uint8_t j = 0; j < 32; ++j) {
            out8[i] ^= N[j * 8 + i];
        }
    }
}

// Struct that contains entire PRNG internal state
typedef struct {
    uint32_t L[STATE_SIZE] __attribute__((aligned(64)));
    uint32_t M[STATE_SIZE] __attribute__((aligned(64)));
    uint32_t tmp8[8];
    uint32_t cons;              // current scalar entropy (stable seed-like value)
    uint64_t outputs_generated; // counts outer iterations completed (used as 'it')
} RA_PRNG;

// Initialize struct state (mirrors start of ra_core)
static void ra_prng_init(RA_PRNG *r, uint32_t seed) {
    r->cons = seed;
    r->outputs_generated = 0;

    for (int i = 0; i < STATE_SIZE; ++i) {
        r->M[i] = (uint32_t)(i * 0x06a0dd9bUL + 0x06a0dd9bUL);
        r->L[i] = (uint32_t)(i * 0x9e3779b7UL + 0x9e3779b7UL);
    }
}

// Perform one full outer iteration; returns the new cons
static uint32_t ra_prng_next(RA_PRNG *r) {
    uint32_t a = r->cons;
    uint32_t b = (uint32_t)r->outputs_generated; // volatile entropy (it)
    uint32_t c = 0u;
    uint32_t d = 0u;

    // Permutation step: inner loop i from 255 down to 1
    for (uint32_t i = 255u; i > 0u; --i) {
        // Aggregate mixer o from M
        uint32_t o = 0u;
        for (uint8_t e = 0; e < 8; ++e) {
            uint8_t idx = (uint8_t)(i + e); // wraps mod 256
            o ^= (r->M[idx] << e);
        }

        // Core transforms
        a = (rot32(b ^ o, d) ^ (r->cons + a));
        b = (rot32(r->cons + a, i) ^ (o + d));
        o = (rot32(a ^ o, i) << 9) ^ (b >> 18);
        c = rot32((o + (c << 14)) ^ (b >> 13) ^ a, b);

        // Lemire's fast reduction: random index d in [0, i]
        d = (uint32_t)(((uint64_t)c * (uint64_t)(i + 1u)) >> 32);

        // Swap L[i] <-> L[d]
        uint32_t tmp = r->L[i];
        r->L[i] = r->L[d];
        r->L[d] = tmp;
    }

    // Mix M ^= L (entropy diffusion)
    for (uint32_t i = 0u; i < STATE_SIZE; ++i) {
        r->M[i] ^= r->L[i];
    }

    // Hash M to produce next cons
    ra_hash(r->M, r->tmp8);

    uint32_t new_cons = 0u;
    for (uint8_t e = 0; e < 8; ++e) {
        new_cons ^= (r->tmp8[e] << e);
    }

    r->cons = new_cons;
    r->outputs_generated++;

    return r->cons;
}

// Advance PRNG by 'iterations' outer steps; returns last cons
static uint32_t ra_prng_advance(RA_PRNG *r, size_t iterations) {
    uint32_t last = r->cons;
    for (size_t k = 0; k < iterations; ++k) {
        last = ra_prng_next(r);
    }
    return last;
}

/* ------------------------------------------------------------------ */
/*  Python binding                                                      */
/* ------------------------------------------------------------------ */

typedef struct {
    PyObject_HEAD
    RA_PRNG prng;
} PyRAObject;

static void PyRA_dealloc(PyRAObject *self) {
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *PyRA_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyRAObject *self = (PyRAObject *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

static int PyRA_init(PyRAObject *self, PyObject *args, PyObject *kwds) {
    unsigned long seed = 0;
    static char *kwlist[] = {"seed", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "k", kwlist, &seed)) {
        return -1;
    }
    ra_prng_init(&self->prng, (uint32_t)seed);
    return 0;
}

static PyObject *PyRA_next(PyRAObject *self, PyObject *Py_UNUSED(ignored)) {
    uint32_t v = ra_prng_next(&self->prng);
    return PyLong_FromUnsignedLong((unsigned long)v);
}

static PyObject *PyRA_advance(PyRAObject *self, PyObject *args) {
    unsigned long iterations;
    if (!PyArg_ParseTuple(args, "k", &iterations)) {
        return NULL;
    }
    uint32_t v = ra_prng_advance(&self->prng, (size_t)iterations);
    return PyLong_FromUnsignedLong((unsigned long)v);
}

static PyObject *PyRA_get_cons(PyRAObject *self, void *closure) {
    return PyLong_FromUnsignedLong((unsigned long)self->prng.cons);
}

static PyObject *PyRA_get_outputs_generated(PyRAObject *self, void *closure) {
    return PyLong_FromUnsignedLongLong((unsigned long long)self->prng.outputs_generated);
}

static PyMethodDef PyRA_methods[] = {
    {"next",    (PyCFunction)PyRA_next,    METH_NOARGS,  "Produce next cons (one outer iteration)"},
    {"advance", (PyCFunction)PyRA_advance, METH_VARARGS, "Advance by N outer iterations, return last cons"},
    {NULL}
};

static PyGetSetDef PyRA_getset[] = {
    {"cons",              (getter)PyRA_get_cons,              NULL, "current cons value",         NULL},
    {"outputs_generated", (getter)PyRA_get_outputs_generated, NULL, "number of outputs produced", NULL},
    {NULL}
};

static PyTypeObject PyRAType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name      = "ra_prng.RA_PRNG",
    .tp_doc       = "RA_PRNG pseudorandom number generator",
    .tp_basicsize = sizeof(PyRAObject),
    .tp_itemsize  = 0,
    .tp_flags     = Py_TPFLAGS_DEFAULT,
    .tp_new       = PyRA_new,
    .tp_init      = (initproc)PyRA_init,
    .tp_dealloc   = (destructor)PyRA_dealloc,
    .tp_methods   = PyRA_methods,
    .tp_getset    = PyRA_getset,
};

static PyMethodDef module_methods[] = {
    {NULL}
};

static struct PyModuleDef ra_prngmodule = {
    PyModuleDef_HEAD_INIT,
    "ra_prng",
    "RA PRNG module",
    -1,
    module_methods
};

PyMODINIT_FUNC PyInit_ra_prng(void) {
    PyObject *m;
    if (PyType_Ready(&PyRAType) < 0)
        return NULL;

    m = PyModule_Create(&ra_prngmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyRAType);
    if (PyModule_AddObject(m, "RA_PRNG", (PyObject *)&PyRAType) < 0) {
        Py_DECREF(&PyRAType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}