import ra_prng
from PIL import Image
import numpy as np
from datetime import datetime, timezone
seed = int(datetime.now().timestamp())
print(seed)
rng = ra_prng.RA_PRNG(seed = 1764732726)


img = Image.open("test.png").convert("RGB")

arr = np.array(img)

# XOR image

arr_enc = arr
for y in arr_enc:
    for x in y:
        r = rng.next()
        x[0] ^= ((r >> 8) ^ r) & 0xFF
        x[1] ^= ((r >> 16) ^ r) & 0xFF
        x[2] ^= ((r >> 24) ^ r)& 0xFF

img_enc = Image.fromarray(arr_enc.astype("uint8"))
img_enc.save("encrypted.png")
#img_enc.show()