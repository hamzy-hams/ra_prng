	.file	"diag_init_candidates.c"
	.text
	.p2align 4
	.type	ra_reseed, @function
ra_reseed:
.LFB25:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	subq	$64, %rsp
	movq	%fs:40, %rax
	movq	%rax, 56(%rsp)
	xorl	%eax, %eax
	.p2align 6
	.p2align 4,,10
	.p2align 3
.L2:
	vmovdqa	(%rdi,%rax), %ymm0
	vpxord	(%rsi,%rax), %ymm0, %ymm0
	vmovdqa	%ymm0, (%rdi,%rax)
	addq	$32, %rax
	cmpq	$1024, %rax
	jne	.L2
	vmovdqa	(%rdi), %ymm0
	vmovdqa	32(%rdi), %ymm1
	vpxord	96(%rdi), %ymm0, %ymm0
	vpternlogd	$150, 64(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovdqa	192(%rdi), %ymm1
	vmovd	%xmm0, (%rsp)
	vmovdqa	128(%rdi), %ymm0
	vpxord	224(%rdi), %ymm0, %ymm0
	vpternlogd	$150, 160(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovdqa	320(%rdi), %ymm1
	vmovd	%xmm0, 4(%rsp)
	vmovdqa	256(%rdi), %ymm0
	vpxord	352(%rdi), %ymm0, %ymm0
	vpternlogd	$150, 288(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovdqa	448(%rdi), %ymm1
	vmovd	%xmm0, 8(%rsp)
	vmovdqa	384(%rdi), %ymm0
	vpxord	480(%rdi), %ymm0, %ymm0
	vpternlogd	$150, 416(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovdqa	576(%rdi), %ymm1
	vmovd	%xmm0, 12(%rsp)
	vmovdqa	512(%rdi), %ymm0
	vpxord	608(%rdi), %ymm0, %ymm0
	vpternlogd	$150, 544(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovdqa	704(%rdi), %ymm1
	vmovd	%xmm0, 16(%rsp)
	vmovdqa	640(%rdi), %ymm0
	vpxord	736(%rdi), %ymm0, %ymm0
	vpternlogd	$150, 672(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, 20(%rsp)
	vmovdqa	768(%rdi), %ymm0
	vpxord	864(%rdi), %ymm0, %ymm0
	vmovdqa	832(%rdi), %ymm1
	vpternlogd	$150, 800(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovdqa	928(%rdi), %ymm1
	vmovd	%xmm0, 24(%rsp)
	vmovdqa	896(%rdi), %ymm0
	vpxord	992(%rdi), %ymm0, %ymm0
	vpternlogd	$150, 960(%rdi), %ymm0, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, 28(%rsp)
	vmovdqa	(%rsp), %ymm1
	vpsllvd	.LC0(%rip), %ymm1, %ymm1
	vextracti32x4	$0x1, %ymm1, %xmm0
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$8, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vpsrldq	$4, %xmm0, %xmm1
	vpxord	%xmm1, %xmm0, %xmm0
	vmovd	%xmm0, %eax
	movq	56(%rsp), %rdx
	subq	%fs:40, %rdx
	jne	.L7
	vzeroupper
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L7:
	.cfi_restore_state
	vzeroupper
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE25:
	.size	ra_reseed, .-ra_reseed
	.p2align 4
	.type	ra_init_state_orbit_baseline, @function
ra_init_state_orbit_baseline:
.LFB26:
	.cfi_startproc
	movl	%edx, %ecx
	imull	$-1640531529, %edx, %edx
	vpcmpeqd	%ymm6, %ymm6, %ymm6
	vmovdqa	.LC0(%rip), %ymm1
	vpbroadcastd	%ecx, %ymm9
	vpsrld	$27, %ymm6, %ymm6
	vpxor	%xmm11, %xmm11, %xmm11
	xorl	$949054964, %edx
	movl	%edx, %eax
	shrl	$16, %eax
	xorl	%edx, %eax
	imull	$-2048144789, %eax, %eax
	movl	%eax, %edx
	shrl	$13, %edx
	xorl	%edx, %eax
	imull	$-1028477387, %eax, %eax
	movl	%eax, %edx
	shrl	$16, %edx
	xorl	%edx, %eax
	imull	$111205787, %ecx, %edx
	vpbroadcastd	%eax, %ymm7
	xorl	%eax, %eax
	xorl	$1814494354, %edx
	movl	%edx, %r8d
	shrl	$16, %r8d
	xorl	%edx, %r8d
	imull	$-2048144789, %r8d, %r8d
	movl	%r8d, %edx
	shrl	$13, %edx
	xorl	%r8d, %edx
	imull	$-1028477387, %edx, %edx
	movl	%edx, %r8d
	shrl	$16, %r8d
	xorl	%r8d, %edx
	vpbroadcastd	%edx, %ymm8
	movl	$111205787, %edx
	vpbroadcastd	%edx, %ymm0
	movl	$-1640531529, %edx
	vpmulld	%ymm0, %ymm8, %ymm8
	vpbroadcastd	%edx, %ymm0
	movl	$8, %edx
	vpmulld	%ymm0, %ymm7, %ymm7
	vpbroadcastd	%edx, %ymm10
	.p2align 4,,10
	.p2align 3
.L9:
	vpmulld	%ymm1, %ymm8, %ymm2
	vpmulld	%ymm1, %ymm7, %ymm4
	vmovdqa	%ymm1, %ymm0
	vpternlogd	$40, %ymm6, %ymm9, %ymm0
	vpaddd	%ymm10, %ymm1, %ymm1
	vpsubd	%ymm0, %ymm11, %ymm3
	vpandd	%ymm6, %ymm3, %ymm3
	vpsllvd	%ymm0, %ymm4, %ymm5
	vpsllvd	%ymm0, %ymm2, %ymm0
	vpsrlvd	%ymm3, %ymm4, %ymm4
	vpsrlvd	%ymm3, %ymm2, %ymm2
	vpord	%ymm4, %ymm5, %ymm4
	vpord	%ymm2, %ymm0, %ymm0
	vmovdqa	%ymm4, (%rdi,%rax)
	vmovdqa	%ymm0, (%rsi,%rax)
	addq	$32, %rax
	cmpq	$1024, %rax
	jne	.L9
	vzeroupper
	ret
	.cfi_endproc
.LFE26:
	.size	ra_init_state_orbit_baseline, .-ra_init_state_orbit_baseline
	.p2align 4
	.type	ra_init_state_singleblock_baseline, @function
ra_init_state_singleblock_baseline:
.LFB27:
	.cfi_startproc
	imull	$111205787, %esi, %edx
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vmovdqa	.LC0(%rip), %ymm2
	vpbroadcastd	%esi, %ymm6
	vpsrld	$27, %ymm4, %ymm4
	vpxor	%xmm8, %xmm8, %xmm8
	xorl	$1814494354, %edx
	movl	%edx, %eax
	shrl	$16, %eax
	xorl	%edx, %eax
	imull	$-2048144789, %eax, %eax
	movl	%eax, %edx
	shrl	$13, %edx
	xorl	%edx, %eax
	imull	$-1028477387, %eax, %eax
	movl	%eax, %edx
	shrl	$16, %edx
	xorl	%edx, %eax
	leaq	1024(%rdi), %rdx
	vpbroadcastd	%eax, %ymm5
	movl	$111205787, %eax
	vpbroadcastd	%eax, %ymm0
	movl	$8, %eax
	vpmulld	%ymm0, %ymm5, %ymm5
	vpbroadcastd	%eax, %ymm7
	.p2align 4,,10
	.p2align 3
.L12:
	vpmulld	%ymm2, %ymm5, %ymm1
	vmovdqa	%ymm2, %ymm3
	vpaddd	%ymm7, %ymm2, %ymm2
	addq	$32, %rdi
	vpternlogd	$40, %ymm4, %ymm6, %ymm3
	vpsubd	%ymm3, %ymm8, %ymm0
	vpandd	%ymm4, %ymm0, %ymm0
	vpsrlvd	%ymm0, %ymm1, %ymm0
	vpsllvd	%ymm3, %ymm1, %ymm1
	vpord	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdi)
	cmpq	%rdi, %rdx
	jne	.L12
	vzeroupper
	ret
	.cfi_endproc
.LFE27:
	.size	ra_init_state_singleblock_baseline, .-ra_init_state_singleblock_baseline
	.p2align 4
	.type	ra_init_state_orbit_accum, @function
ra_init_state_orbit_accum:
.LFB28:
	.cfi_startproc
	imull	$-1640531529, %edx, %eax
	movq	%rsi, %r8
	movl	%edx, %r9d
	vmovdqa	.LC0(%rip), %ymm5
	imull	$111205787, %edx, %edx
	vpcmpeqd	%ymm6, %ymm6, %ymm6
	vpbroadcastd	%r9d, %ymm10
	vpsrld	$27, %ymm6, %ymm6
	vpxor	%xmm12, %xmm12, %xmm12
	xorl	$949054964, %eax
	movl	%eax, %esi
	xorl	$1814494354, %edx
	shrl	$16, %esi
	xorl	%eax, %esi
	imull	$-2048144789, %esi, %esi
	movl	%esi, %eax
	shrl	$13, %eax
	xorl	%eax, %esi
	imull	$-1028477387, %esi, %esi
	movl	%esi, %eax
	shrl	$16, %eax
	xorl	%eax, %esi
	movl	%edx, %eax
	shrl	$16, %eax
	xorl	%edx, %eax
	imull	$-1640531529, %esi, %edx
	imull	$-2048144789, %eax, %eax
	imull	$-239350344, %esi, %esi
	vpbroadcastd	%edx, %ymm3
	movl	$8, %edx
	movl	%eax, %ecx
	vpmulld	%ymm5, %ymm3, %ymm3
	vpbroadcastd	%edx, %ymm11
	shrl	$13, %ecx
	vpbroadcastd	%esi, %ymm9
	xorl	%eax, %ecx
	imull	$-1028477387, %ecx, %ecx
	movl	%ecx, %eax
	shrl	$16, %eax
	xorl	%eax, %ecx
	imull	$111205787, %ecx, %eax
	imull	$889646296, %ecx, %ecx
	vpbroadcastd	%eax, %ymm2
	xorl	%eax, %eax
	vpmulld	%ymm5, %ymm2, %ymm2
	vpbroadcastd	%ecx, %ymm8
	.p2align 4,,10
	.p2align 3
.L15:
	vmovdqa	%ymm5, %ymm0
	vpaddd	%ymm11, %ymm5, %ymm5
	vpternlogd	$40, %ymm6, %ymm10, %ymm0
	vpsubd	%ymm0, %ymm12, %ymm1
	vpsllvd	%ymm0, %ymm3, %ymm4
	vpandd	%ymm6, %ymm1, %ymm1
	vpsllvd	%ymm0, %ymm2, %ymm0
	vpsrlvd	%ymm1, %ymm3, %ymm7
	vpsrlvd	%ymm1, %ymm2, %ymm1
	vpaddd	%ymm9, %ymm3, %ymm3
	vpaddd	%ymm8, %ymm2, %ymm2
	vpord	%ymm7, %ymm4, %ymm4
	vpord	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm4, (%rdi,%rax)
	vmovdqa	%ymm0, (%r8,%rax)
	addq	$32, %rax
	cmpq	$1024, %rax
	jne	.L15
	vzeroupper
	ret
	.cfi_endproc
.LFE28:
	.size	ra_init_state_orbit_accum, .-ra_init_state_orbit_accum
	.p2align 4
	.type	ra_init_state_singleblock_accum, @function
ra_init_state_singleblock_accum:
.LFB29:
	.cfi_startproc
	imull	$111205787, %esi, %eax
	vmovdqa	.LC0(%rip), %ymm3
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vpbroadcastd	%esi, %ymm6
	leaq	1024(%rdi), %rcx
	vpsrld	$27, %ymm4, %ymm4
	vpxor	%xmm8, %xmm8, %xmm8
	xorl	$1814494354, %eax
	movl	%eax, %edx
	shrl	$16, %edx
	xorl	%eax, %edx
	imull	$-2048144789, %edx, %edx
	movl	%edx, %eax
	shrl	$13, %eax
	xorl	%eax, %edx
	imull	$-1028477387, %edx, %edx
	movl	%edx, %eax
	shrl	$16, %eax
	xorl	%eax, %edx
	imull	$111205787, %edx, %eax
	imull	$889646296, %edx, %edx
	vpbroadcastd	%eax, %ymm2
	movq	%rdi, %rax
	vpmulld	%ymm3, %ymm2, %ymm2
	vpbroadcastd	%edx, %ymm5
	movl	$8, %edx
	vpbroadcastd	%edx, %ymm7
	.p2align 4,,10
	.p2align 3
.L18:
	vmovdqa	%ymm3, %ymm1
	addq	$32, %rax
	vpaddd	%ymm7, %ymm3, %ymm3
	vpternlogd	$40, %ymm4, %ymm6, %ymm1
	vpsubd	%ymm1, %ymm8, %ymm0
	vpsllvd	%ymm1, %ymm2, %ymm1
	vpandd	%ymm4, %ymm0, %ymm0
	vpsrlvd	%ymm0, %ymm2, %ymm0
	vpaddd	%ymm5, %ymm2, %ymm2
	vpord	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rax, %rcx
	jne	.L18
	vzeroupper
	ret
	.cfi_endproc
.LFE29:
	.size	ra_init_state_singleblock_accum, .-ra_init_state_singleblock_accum
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC5:
	.string	"orbit_baseline"
.LC6:
	.string	"orbit_accum"
.LC7:
	.string	"singleblock_baseline"
.LC8:
	.string	"singleblock_accum"
.LC9:
	.string	"Unknown core '%s'. Known: "
.LC10:
	.string	"%s "
	.text
	.p2align 4
	.type	find_core, @function
find_core:
.LFB36:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movq	%rdi, %rsi
	movq	%rbx, 8(%rsp)
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	leaq	.LC5(%rip), %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L24
	movq	%rbx, %rsi
	leaq	.LC6(%rip), %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L25
	movq	%rbx, %rsi
	leaq	.LC7(%rip), %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L26
	movq	%rbx, %rsi
	leaq	.LC8(%rip), %rdi
	call	strcmp@PLT
	testl	%eax, %eax
	je	.L30
	movq	stderr(%rip), %rdi
	movq	%rbx, %rdx
	leaq	.LC9(%rip), %rsi
	xorl	%eax, %eax
	leaq	CORES(%rip), %rbx
	movq	%rbp, 16(%rsp)
	.cfi_offset 6, -16
	call	fprintf@PLT
	leaq	96(%rbx), %rbp
.L23:
	movq	(%rbx), %rdx
	movq	stderr(%rip), %rdi
	leaq	.LC10(%rip), %rsi
	xorl	%eax, %eax
	addq	$24, %rbx
	call	fprintf@PLT
	cmpq	%rbp, %rbx
	jne	.L23
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc@PLT
	movl	$1, %edi
	call	exit@PLT
.L24:
	.cfi_restore 6
	xorl	%eax, %eax
.L21:
	imulq	$24, %rax, %rax
	leaq	CORES(%rip), %rdx
	movq	8(%rsp), %rbx
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	addq	%rdx, %rax
	ret
.L25:
	.cfi_restore_state
	movl	$1, %eax
	jmp	.L21
.L26:
	movl	$2, %eax
	jmp	.L21
.L30:
	movl	$3, %eax
	jmp	.L21
	.cfi_endproc
.LFE36:
	.size	find_core, .-find_core
	.section	.rodata.str1.1
.LC11:
	.string	"rng>255 out of scope\n"
	.text
	.p2align 4
	.type	ra_core_singleblock_accum.part.0, @function
ra_core_singleblock_accum.part.0:
.LFB40:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$21, %edx
	leaq	.LC11(%rip), %rdi
	movl	$1, %esi
	movq	stderr(%rip), %rcx
	call	fwrite@PLT
	call	abort@PLT
	.cfi_endproc
.LFE40:
	.size	ra_core_singleblock_accum.part.0, .-ra_core_singleblock_accum.part.0
	.set	ra_core_singleblock_baseline.part.0,ra_core_singleblock_accum.part.0
	.p2align 4
	.type	ra_permutation_cycle_singleblock.constprop.0, @function
ra_permutation_cycle_singleblock.constprop.0:
.LFB44:
	.cfi_startproc
	subq	$72, %rsp
	.cfi_def_cfa_offset 80
	movq	%r14, 56(%rsp)
	.cfi_offset 14, -24
	movq	%fs:40, %r14
	movq	%r14, 8(%rsp)
	movq	%rdx, %r14
	testq	%rcx, %rcx
	je	.L44
	movq	%rbx, 24(%rsp)
	.cfi_offset 3, -56
	movq	%rsi, %rbx
	movq	%rbp, 32(%rsp)
	.cfi_offset 6, -48
	movq	%rcx, %rbp
	xorl	%ecx, %ecx
	movq	%r12, 40(%rsp)
	.cfi_offset 12, -40
	movl	%edi, %r12d
	movq	%r13, 48(%rsp)
	.cfi_offset 13, -32
	movl	$5, %r13d
	movq	%r15, 64(%rsp)
	.cfi_offset 15, -16
	leal	(%rdi,%rdi), %r15d
	jmp	.L34
	.p2align 4,,10
	.p2align 3
.L45:
	decq	%rax
	decl	%r13d
	movzbl	4(%rsp), %ecx
	movq	%rax, (%r14)
	cmpb	$6, %r13b
	je	.L43
.L34:
	leal	1(%r13), %eax
	movzbl	%r13b, %edx
	leaq	4(%rsp), %rdi
	movl	$4, %esi
	movzbl	%al, %eax
	movl	(%rbx,%rdx,4), %edx
	movl	(%rbx,%rax,4), %eax
	sall	$6, %edx
	sall	$7, %eax
	xorl	%edx, %eax
	xorl	%ecx, %eax
	xorl	%r15d, %eax
	movl	%eax, %edx
	leal	(%r12,%rax), %r15d
	shrl	$13, %edx
	xorl	%r15d, %ecx
	xorl	%eax, %edx
	roll	%cl, %edx
	movq	%rbp, %rcx
	movl	%edx, 4(%rsp)
	movl	$1, %edx
	call	fwrite@PLT
	movq	(%r14), %rax
	cmpq	$1, %rax
	ja	.L45
.L43:
	movq	24(%rsp), %rbx
	.cfi_restore 3
	movq	32(%rsp), %rbp
	.cfi_restore 6
	movq	40(%rsp), %r12
	.cfi_restore 12
	movq	48(%rsp), %r13
	.cfi_restore 13
	movq	64(%rsp), %r15
	.cfi_restore 15
.L33:
	movq	8(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L46
	movq	56(%rsp), %r14
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L44:
	.cfi_restore_state
	movl	$5, %edx
	jmp	.L36
	.p2align 5
	.p2align 4,,10
	.p2align 3
.L47:
	decq	%rax
	decl	%edx
	movq	%rax, (%r14)
	cmpb	$6, %dl
	je	.L33
.L36:
	movq	(%r14), %rax
	cmpq	$1, %rax
	ja	.L47
	jmp	.L33
.L46:
	movq	%rbx, 24(%rsp)
	movq	%rbp, 32(%rsp)
	movq	%r12, 40(%rsp)
	movq	%r13, 48(%rsp)
	movq	%r15, 64(%rsp)
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 15, -16
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE44:
	.size	ra_permutation_cycle_singleblock.constprop.0, .-ra_permutation_cycle_singleblock.constprop.0
	.p2align 4
	.type	ra_core_singleblock_baseline, @function
ra_core_singleblock_baseline:
.LFB34:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-64, %rsp
	subq	$1152, %rsp
	.cfi_offset 3, -24
	movq	%fs:40, %rbx
	movq	%rbx, 1144(%rsp)
	movl	%edi, %ebx
	testq	%rsi, %rsi
	je	.L49
	cmpq	$255, %rsi
	ja	.L56
	movq	%rsi, 56(%rsp)
	leaq	64(%rsp), %rdi
	movl	%ebx, %esi
	movq	%rdx, %rcx
	call	ra_init_state_singleblock_baseline
	leaq	56(%rsp), %rdx
	leaq	64(%rsp), %rsi
	movl	%ebx, %edi
	call	ra_permutation_cycle_singleblock.constprop.0
.L49:
	movq	1144(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L57
	movl	%ebx, %eax
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L57:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
.L56:
	call	ra_core_singleblock_baseline.part.0
	.cfi_endproc
.LFE34:
	.size	ra_core_singleblock_baseline, .-ra_core_singleblock_baseline
	.p2align 4
	.type	ra_core_singleblock_accum, @function
ra_core_singleblock_accum:
.LFB35:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-64, %rsp
	subq	$1152, %rsp
	.cfi_offset 3, -24
	movq	%fs:40, %rbx
	movq	%rbx, 1144(%rsp)
	movl	%edi, %ebx
	testq	%rsi, %rsi
	je	.L59
	cmpq	$255, %rsi
	ja	.L66
	movq	%rdx, %r8
	leaq	64(%rsp), %rdi
	movq	%rsi, 56(%rsp)
	movl	%ebx, %esi
	call	ra_init_state_singleblock_accum
	movq	%rdi, %rsi
	leaq	56(%rsp), %rdx
	movq	%r8, %rcx
	movl	%ebx, %edi
	call	ra_permutation_cycle_singleblock.constprop.0
.L59:
	movq	1144(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L67
	movl	%ebx, %eax
	movq	-8(%rbp), %rbx
	leave
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L67:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
.L66:
	call	ra_core_singleblock_accum.part.0
	.cfi_endproc
.LFE35:
	.size	ra_core_singleblock_accum, .-ra_core_singleblock_accum
	.p2align 4
	.type	ra_permutation_cycle_orbit.isra.0, @function
ra_permutation_cycle_orbit.isra.0:
.LFB45:
	.cfi_startproc
	subq	$88, %rsp
	.cfi_def_cfa_offset 96
	movq	%r13, 64(%rsp)
	.cfi_offset 13, -32
	movq	%rsi, %r13
	movq	%r14, 72(%rsp)
	.cfi_offset 14, -24
	movq	%rcx, %r14
	movq	%r15, 80(%rsp)
	.cfi_offset 15, -16
	movl	%edi, %r15d
	movq	%rbx, 40(%rsp)
	movq	%rdx, (%rsp)
	movq	%r8, 8(%rsp)
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	testq	%r8, %r8
	.cfi_offset 3, -56
	je	.L79
	movq	%rbp, 48(%rsp)
	movl	$5, %ebx
	.cfi_offset 6, -48
	leaq	1020(%rdx), %rbp
	xorl	%ecx, %ecx
	movq	%r12, 56(%rsp)
	.cfi_offset 12, -40
	leal	(%rdi,%rdi), %r12d
	jmp	.L72
	.p2align 4,,10
	.p2align 3
.L80:
	movzbl	20(%rsp), %ecx
	movq	(%rsp), %rdi
	decq	%rax
	decl	%ebx
	movq	%rax, (%r14)
	movl	0(%rbp), %edx
	subq	$4, %rbp
	movl	%ecx, %eax
	movl	(%rdi,%rax,4), %esi
	movl	%esi, 4(%rbp)
	movl	%edx, (%rdi,%rax,4)
	cmpb	$6, %bl
	je	.L78
.L72:
	leal	1(%rbx), %eax
	movzbl	%bl, %edx
	leaq	20(%rsp), %rdi
	movl	$4, %esi
	movzbl	%al, %eax
	movl	0(%r13,%rdx,4), %edx
	movl	0(%r13,%rax,4), %eax
	sall	$6, %edx
	sall	$7, %eax
	xorl	%edx, %eax
	xorl	%ecx, %eax
	xorl	%r12d, %eax
	movl	%eax, %edx
	leal	(%r15,%rax), %r12d
	shrl	$13, %edx
	xorl	%r12d, %ecx
	xorl	%eax, %edx
	roll	%cl, %edx
	movq	8(%rsp), %rcx
	movl	%edx, 20(%rsp)
	movl	$1, %edx
	call	fwrite@PLT
	movq	(%r14), %rax
	cmpq	$1, %rax
	ja	.L80
.L78:
	movq	48(%rsp), %rbp
	.cfi_restore 6
	movq	56(%rsp), %r12
	.cfi_restore 12
.L68:
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L81
	movq	40(%rsp), %rbx
	movq	64(%rsp), %r13
	movq	72(%rsp), %r14
	movq	80(%rsp), %r15
	addq	$88, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L79:
	.cfi_restore_state
	leaq	1020(%rdx), %r8
	xorl	%r9d, %r9d
	movl	%edi, %edx
	movl	$5, %edi
	jmp	.L71
	.p2align 4,,10
	.p2align 3
.L82:
	leal	1(%rdi), %eax
	movzbl	%dil, %ecx
	addl	%r15d, %edx
	movq	(%rsp), %rbx
	movzbl	%al, %eax
	movl	0(%r13,%rcx,4), %ecx
	decq	%rsi
	decl	%edi
	movl	0(%r13,%rax,4), %eax
	movq	%rsi, (%r14)
	subq	$4, %r8
	sall	$6, %ecx
	sall	$7, %eax
	xorl	%ecx, %eax
	xorl	%r9d, %eax
	xorl	%eax, %edx
	movl	%edx, %eax
	leal	(%r15,%rdx), %ecx
	shrl	$13, %eax
	xorl	%r9d, %ecx
	xorl	%edx, %eax
	roll	%cl, %eax
	movl	4(%r8), %ecx
	movzbl	%al, %r9d
	movzbl	%al, %eax
	movl	(%rbx,%rax,4), %esi
	movl	%esi, 4(%r8)
	movl	%ecx, (%rbx,%rax,4)
	cmpb	$6, %dil
	je	.L68
.L71:
	movq	(%r14), %rsi
	cmpq	$1, %rsi
	ja	.L82
	jmp	.L68
.L81:
	movq	%rbp, 48(%rsp)
	movq	%r12, 56(%rsp)
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE45:
	.size	ra_permutation_cycle_orbit.isra.0, .-ra_permutation_cycle_orbit.isra.0
	.p2align 4
	.type	ra_core_orbit_baseline.part.0, @function
ra_core_orbit_baseline.part.0:
.LFB43:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movabsq	$-9187201950435737471, %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 14, -24
	.cfi_offset 13, -32
	.cfi_offset 12, -40
	xorl	%r12d, %r12d
	pushq	%rbx
	.cfi_offset 3, -48
	movl	%edi, %ebx
	andq	$-64, %rsp
	subq	$2176, %rsp
	movq	%fs:40, %r14
	movq	%r14, 2168(%rsp)
	movq	%rdx, %r14
	mulq	%rsi
	movq	%rsi, 56(%rsp)
	leaq	1088(%rsp), %rsi
	shrq	$7, %rdx
	movq	%rdx, %r13
	movl	%edi, %edx
	leaq	64(%rsp), %rdi
	call	ra_init_state_orbit_baseline
	jmp	.L85
	.p2align 4,,10
	.p2align 3
.L87:
	incq	%r12
.L85:
	movq	%r14, %r8
	leaq	56(%rsp), %rcx
	leaq	64(%rsp), %rdx
	movl	%ebx, %edi
	leaq	1088(%rsp), %rsi
	call	ra_permutation_cycle_orbit.isra.0
	cmpq	$1, 56(%rsp)
	jbe	.L84
	leaq	64(%rsp), %rsi
	leaq	1088(%rsp), %rdi
	call	ra_reseed
	movl	%eax, %ebx
	cmpq	%r12, %r13
	jne	.L87
.L84:
	movq	2168(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L89
	leaq	-32(%rbp), %rsp
	movl	%ebx, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L89:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE43:
	.size	ra_core_orbit_baseline.part.0, .-ra_core_orbit_baseline.part.0
	.p2align 4
	.type	ra_core_orbit_baseline, @function
ra_core_orbit_baseline:
.LFB32:
	.cfi_startproc
	testq	%rsi, %rsi
	jne	.L92
	movl	%edi, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L92:
	jmp	ra_core_orbit_baseline.part.0
	.cfi_endproc
.LFE32:
	.size	ra_core_orbit_baseline, .-ra_core_orbit_baseline
	.p2align 4
	.type	ra_core_orbit_accum.part.0, @function
ra_core_orbit_accum.part.0:
.LFB42:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movabsq	$-9187201950435737471, %rax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 14, -24
	.cfi_offset 13, -32
	.cfi_offset 12, -40
	xorl	%r12d, %r12d
	pushq	%rbx
	.cfi_offset 3, -48
	movl	%edi, %ebx
	andq	$-64, %rsp
	subq	$2176, %rsp
	movq	%fs:40, %r14
	movq	%r14, 2168(%rsp)
	movq	%rdx, %r14
	mulq	%rsi
	movq	%rsi, 56(%rsp)
	leaq	1088(%rsp), %rsi
	shrq	$7, %rdx
	movq	%rdx, %r13
	movl	%edi, %edx
	leaq	64(%rsp), %rdi
	call	ra_init_state_orbit_accum
	jmp	.L95
	.p2align 4,,10
	.p2align 3
.L97:
	incq	%r12
.L95:
	movq	%r14, %r8
	leaq	56(%rsp), %rcx
	leaq	64(%rsp), %rdx
	movl	%ebx, %edi
	leaq	1088(%rsp), %rsi
	call	ra_permutation_cycle_orbit.isra.0
	cmpq	$1, 56(%rsp)
	jbe	.L94
	leaq	64(%rsp), %rsi
	leaq	1088(%rsp), %rdi
	call	ra_reseed
	movl	%eax, %ebx
	cmpq	%r12, %r13
	jne	.L97
.L94:
	movq	2168(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L99
	leaq	-32(%rbp), %rsp
	movl	%ebx, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L99:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE42:
	.size	ra_core_orbit_accum.part.0, .-ra_core_orbit_accum.part.0
	.p2align 4
	.type	ra_core_orbit_accum, @function
ra_core_orbit_accum:
.LFB33:
	.cfi_startproc
	testq	%rsi, %rsi
	jne	.L102
	movl	%edi, %eax
	ret
	.p2align 4,,10
	.p2align 3
.L102:
	jmp	ra_core_orbit_accum.part.0
	.cfi_endproc
.LFE33:
	.size	ra_core_orbit_accum, .-ra_core_orbit_accum
	.section	.rodata.str1.1
.LC13:
	.string	"--stream"
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC14:
	.string	"--stream: n=%zu exceeds core '%s' max_rng=%ld.\n"
	.align 8
.LC16:
	.string	"Streamed %zu pseudorandom outputs in %.3f seconds\n"
	.section	.rodata.str1.1
.LC17:
	.string	"Last cons from RNGing: %u\n"
.LC18:
	.string	"--bench"
.LC19:
	.string	"K must be > 0\n"
	.section	.rodata.str1.8
	.align 8
.LC20:
	.string	"--bench: K=%lld exceeds core '%s' max_rng=%ld.\n"
	.section	.rodata.str1.1
.LC22:
	.string	"sink=%u\n"
	.section	.rodata.str1.8
	.align 8
.LC23:
	.string	"%s K %lld cycles %lld ns_per_word %.4f\n"
	.align 8
.LC24:
	.string	"usage:\n  %s --stream <core> <key> <n>\n  %s --bench <core> <cycles_target_words> <K>\ncores: orbit_baseline orbit_accum singleblock_baseline singleblock_accum\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB38:
	.cfi_startproc
	subq	$136, %rsp
	.cfi_def_cfa_offset 144
	movq	%rbx, 88(%rsp)
	movq	%rbp, 96(%rsp)
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	movq	%fs:40, %rbp
	movq	%rbp, 72(%rsp)
	movq	%rsi, %rbp
	cmpl	$4, %edi
	jle	.L104
	movq	%r12, 104(%rsp)
	.cfi_offset 12, -40
	movq	8(%rbp), %r12
	leaq	.LC13(%rip), %rsi
	movq	%r12, %rdi
	call	strcmp@PLT
	movl	%eax, %ebx
	testl	%eax, %eax
	je	.L130
	leaq	.LC18(%rip), %rsi
	movq	%r12, %rdi
	call	strcmp@PLT
	movl	%eax, %ebx
	testl	%eax, %eax
	je	.L131
	movq	104(%rsp), %r12
	.cfi_restore 12
.L104:
	movq	0(%rbp), %rdx
	movq	stderr(%rip), %rdi
	leaq	.LC24(%rip), %rsi
	xorl	%eax, %eax
	movq	%rdx, %rcx
	call	fprintf@PLT
.L110:
	movl	$1, %ebx
.L103:
	movq	72(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L132
	movl	%ebx, %eax
	movq	96(%rsp), %rbp
	movq	88(%rsp), %rbx
	addq	$136, %rsp
	.cfi_def_cfa_offset 8
	ret
.L130:
	.cfi_def_cfa_offset 144
	.cfi_offset 12, -40
	movq	16(%rbp), %rdi
	movq	%r13, 112(%rsp)
	.cfi_offset 13, -32
	call	find_core
	movq	24(%rbp), %rdi
	xorl	%edx, %edx
	xorl	%esi, %esi
	movq	%rax, %r12
	call	strtoul@PLT
	movq	32(%rbp), %rdi
	xorl	%edx, %edx
	xorl	%esi, %esi
	movq	%rax, %r13
	call	strtoull@PLT
	movq	16(%r12), %r8
	movq	%rax, %rbp
	cmpq	%rax, %r8
	jge	.L106
	testq	%r8, %r8
	jns	.L133
.L106:
	leaq	32(%rsp), %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	stdout(%rip), %rdx
	movl	%r13d, %edi
	movq	%rbp, %rsi
	call	*8(%r12)
	leaq	48(%rsp), %rsi
	movl	$1, %edi
	movl	%eax, %r12d
	call	clock_gettime@PLT
	movq	56(%rsp), %rax
	vxorpd	%xmm5, %xmm5, %xmm5
	movq	stderr(%rip), %rdi
	movq	%rbp, %rdx
	leaq	.LC16(%rip), %rsi
	subq	40(%rsp), %rax
	vcvtsi2sdq	%rax, %xmm5, %xmm0
	movq	48(%rsp), %rax
	vdivsd	.LC15(%rip), %xmm0, %xmm0
	subq	32(%rsp), %rax
	vcvtsi2sdq	%rax, %xmm5, %xmm1
	vaddsd	%xmm1, %xmm0, %xmm0
	movl	$1, %eax
	call	fprintf@PLT
	movq	stderr(%rip), %rdi
	movl	%r12d, %edx
	leaq	.LC17(%rip), %rsi
	xorl	%eax, %eax
	call	fprintf@PLT
	movq	104(%rsp), %r12
	.cfi_restore 12
	movq	112(%rsp), %r13
	.cfi_restore 13
	jmp	.L103
.L131:
	.cfi_offset 12, -40
	movq	16(%rbp), %rdi
	movq	%r13, 112(%rsp)
	.cfi_offset 13, -32
	call	find_core
	movq	24(%rbp), %rdi
	movl	$10, %edx
	xorl	%esi, %esi
	movq	%rax, %r12
	call	strtoll@PLT
	movq	32(%rbp), %rdi
	movl	$10, %edx
	xorl	%esi, %esi
	movq	%rax, %r13
	call	strtoll@PLT
	movq	%rax, %rbp
	testq	%rax, %rax
	jle	.L134
	movq	16(%r12), %r8
	cmpq	%rax, %r8
	jge	.L111
	testq	%r8, %r8
	jns	.L135
.L111:
	movq	%r13, %rax
	vmovsd	.LC12(%rip), %xmm6
	movq	%r14, 120(%rsp)
	cqto
	movq	%r15, 128(%rsp)
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	movl	$3, %r15d
	idivq	%rbp
	vmovsd	%xmm6, 8(%rsp)
	movl	$200000, %edx
	cmpq	%rdx, %rax
	cmovle	%rax, %rdx
	movl	$10, %eax
	cmpq	%rax, %rdx
	cmovge	%rdx, %rax
	xorl	%edx, %edx
	movl	%edx, 32(%rsp)
	imull	$-1640531535, %eax, %r13d
	movq	%rax, 24(%rsp)
	incl	%r13d
.L116:
	leaq	48(%rsp), %rsi
	movl	$1, %edi
	movl	$1, %r14d
	call	clock_gettime@PLT
	vxorpd	%xmm3, %xmm3, %xmm3
	vcvtsi2sdq	56(%rsp), %xmm3, %xmm0
	vcvtsi2sdq	48(%rsp), %xmm3, %xmm1
	vdivsd	.LC15(%rip), %xmm0, %xmm0
	vaddsd	%xmm1, %xmm0, %xmm5
	vmovsd	%xmm5, 16(%rsp)
	.p2align 4,,10
	.p2align 3
.L112:
	movl	%r14d, %edi
	xorl	%edx, %edx
	movq	%rbp, %rsi
	subl	$1640531535, %r14d
	call	*8(%r12)
	xorl	%eax, 32(%rsp)
	cmpl	%r14d, %r13d
	jne	.L112
	leaq	48(%rsp), %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	vmovsd	8(%rsp), %xmm2
	vxorpd	%xmm4, %xmm4, %xmm4
	vxorpd	%xmm7, %xmm7, %xmm7
	vcvtsi2sdq	56(%rsp), %xmm4, %xmm0
	vcvtsi2sdq	48(%rsp), %xmm4, %xmm1
	vdivsd	.LC15(%rip), %xmm0, %xmm0
	vaddsd	%xmm1, %xmm0, %xmm0
	vcomisd	%xmm2, %xmm7
	vsubsd	16(%rsp), %xmm0, %xmm0
	ja	.L118
	vminsd	%xmm2, %xmm0, %xmm1
	vmovsd	%xmm1, 8(%rsp)
.L113:
	movl	%r15d, %eax
	decl	%eax
	movl	%eax, %r15d
	jne	.L116
	movl	32(%rsp), %edx
	movq	stderr(%rip), %rdi
	leaq	.LC22(%rip), %rsi
	xorl	%eax, %eax
	call	fprintf@PLT
	vmovsd	.LC15(%rip), %xmm7
	movq	24(%rsp), %rcx
	movq	%rbp, %rdx
	movq	(%r12), %rsi
	leaq	.LC23(%rip), %rdi
	vmulsd	8(%rsp), %xmm7, %xmm0
	movq	%rcx, %rax
	vxorpd	%xmm7, %xmm7, %xmm7
	imulq	%rbp, %rax
	vcvtsi2sdq	%rax, %xmm7, %xmm1
	movl	$1, %eax
	vdivsd	%xmm1, %xmm0, %xmm0
	call	printf@PLT
	movq	104(%rsp), %r12
	.cfi_remember_state
	.cfi_restore 12
	movq	112(%rsp), %r13
	.cfi_restore 13
	movq	120(%rsp), %r14
	.cfi_restore 14
	movq	128(%rsp), %r15
	.cfi_restore 15
	jmp	.L103
.L118:
	.cfi_restore_state
	vmovsd	%xmm0, 8(%rsp)
	jmp	.L113
.L133:
	.cfi_restore 14
	.cfi_restore 15
	movq	(%r12), %rcx
	movq	stderr(%rip), %rdi
	movq	%rax, %rdx
	leaq	.LC14(%rip), %rsi
	xorl	%eax, %eax
	movl	$1, %ebx
	call	fprintf@PLT
	movq	104(%rsp), %r12
	.cfi_remember_state
	.cfi_restore 12
	movq	112(%rsp), %r13
	.cfi_restore 13
	jmp	.L103
.L135:
	.cfi_restore_state
	movq	(%r12), %rcx
	movq	stderr(%rip), %rdi
	movq	%rax, %rdx
	leaq	.LC20(%rip), %rsi
	xorl	%eax, %eax
	call	fprintf@PLT
	movq	104(%rsp), %r12
	.cfi_remember_state
	.cfi_restore 12
	movq	112(%rsp), %r13
	.cfi_restore 13
	jmp	.L110
.L134:
	.cfi_restore_state
	movq	stderr(%rip), %rcx
	movl	$14, %edx
	movl	$1, %esi
	leaq	.LC19(%rip), %rdi
	call	fwrite@PLT
	movq	104(%rsp), %r12
	.cfi_restore 12
	movq	112(%rsp), %r13
	.cfi_restore 13
	jmp	.L110
.L132:
	movq	%r12, 104(%rsp)
	movq	%r13, 112(%rsp)
	movq	%r14, 120(%rsp)
	movq	%r15, 128(%rsp)
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE38:
	.size	main, .-main
	.section	.data.rel.ro.local,"aw"
	.align 32
	.type	CORES, @object
	.size	CORES, 96
CORES:
	.quad	.LC5
	.quad	ra_core_orbit_baseline
	.quad	-1
	.quad	.LC6
	.quad	ra_core_orbit_accum
	.quad	-1
	.quad	.LC7
	.quad	ra_core_singleblock_baseline
	.quad	255
	.quad	.LC8
	.quad	ra_core_singleblock_accum
	.quad	255
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.long	0
	.long	1
	.long	2
	.long	3
	.long	4
	.long	5
	.long	6
	.long	7
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC12:
	.long	0
	.long	-1074790400
	.align 8
.LC15:
	.long	0
	.long	1104006501
	.ident	"GCC: (GNU) 16.1.1 20260430"
	.section	.note.GNU-stack,"",@progbits
