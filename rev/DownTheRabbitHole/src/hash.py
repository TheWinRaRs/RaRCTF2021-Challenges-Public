#!/usr/bin/python3

from hashlib import md5
s1 = b"bnnyr4bbits"
s2 = b"https://i.imgur.com/gub160B.png"

hash1 = md5(s1).digest()
hash2 = md5(s2).digest()

hash3 = md5(hash1 + hash2)
print(hash3.hexdigest())

"""
RESULT - 8a6c1210701e07f386069aecf11ddefe

KEY  - 8a6c1210701e07f386069aecf11ddefe
IV   - 098f6bcd4621d373cade4e832627b4f6
MODE - CBC
CT   - 01d157154bee90432d3c4de1fce463294d48e45e6c13ca8d889070dee03030ee8b13d550470ea38d4b96ee4fa0de63a2 # noqa: E501
FLAG - rarctf{W0w_y0u-t00k_th3_r3d_p177-539c8a083e}
"""
