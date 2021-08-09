'''
Section [1]
'''
F.<a>   = GF(3^3, modulus=x^3 + x^2 + 2)
SBOX    = (6, 25, 17, 11, 0, 19, 22, 14, 3, 4, 23, 12, 15, 7, 26, 20, 9, 1, 2, 18, 10, 13, 5, 21, 24, 16, 8)
        
def sbrr(sbox):
    for i in range(27):
        for j in range(27):
            ii, jj = F.fetch_int(i), F.fetch_int(j)
            test = [(F.fetch_int(x)*ii + jj).integer_representation() for x in range(27)]
            if tuple(test) == tuple(sbox):
                return ii, jj


grad, offset = sbrr(SBOX) # (2*a^2 + 1, 2*a)
print(grad, offset)

'''
Section [2]
'''
def mald(tup, a, b):
    out = 0
    for i, j in enumerate(tup):
        out2 = 0
        for k, l in enumerate(j):
            out2 += l*a^k
        out += out2*b^i
    return out

def unmald(pol):
    return tuple(tuple(i.polynomial().list()) for i in pol.list())

G.<b> = PolynomialRing(F)
mm = ((2, 0, 1), (1, 2, 0), (0, 2, 1), (2, 0, 1))
MM = mald(mm, a, b)
H.<c> = G.quotient(MM) # Setting up for mix columns

cc = ((1, 2, 0), (2, 0, 1), (1, 1, 1))
CC = mald(cc, a, c)
DD = CC.inverse_mod(MM)
assert CC*DD == 1

R = PolynomialRing(H, 'v', 29*9) # Tryte values for the inputs of the equation
VV = R.gens()

            
def add(a, b):
    return [i + j for i, j in zip(a, b)]
    
def sub(a, x, y):
    return [x*i + y for i in a]
    
def mix(a):
    b = [0 for _ in range(9)]
    for i in range(3):
        we = a[i::3]
        we = we[0] + we[1]*c + we[2]*c^2
        we *= CC
        tt = [0, 0, 0]
        for j, k in zip(we.coefficients(), we.monomials()):
            tt = add(tt, [l*k for l in j.list()])
        b[i::3] = tt
    return b
    
def row(a):
    return [a[i] for i in (0, 1, 2, 4, 5, 3, 8, 6, 7)]
    
def gen_eq():
    kek = VV[28*9:29*9]
    kek = add(kek, VV[:9])
    
    for i in range(9, 27*9, 9):
        kek = sub(kek, grad, offset)
        kek = row(kek)
        kek = mix(kek)
        kek = add(kek, VV[i:i+9])
    
    kek = sub(kek, grad, offset)
    kek = row(kek)
    kek = add(kek, VV[27*9:28*9])
    
    return kek

eq = gen_eq()
print(eq)

'''
Section [3]
'''

int_to_byt = lambda x: int(x).to_bytes((int(x).bit_length() + 7) // 8, "big")
byt_to_int = lambda x: int.from_bytes(x, byteorder="big")

def itt(a):
    b = Integer(a).str(3)[::-1]
    c = [int(x) for x in b]
    d = c + [0] * (27-len(c))
    e = [F(d[i:i+3]) for i in range(0, len(d), 3)]
    return e

def tti(a):
    b = 0
    for i in a[::-1]:
        b *= 27
        b += i.integer_representation()
    return b

cc = str(eq) # need to reformat later to remove the mix columns layer

G = PolynomialRing(F, "v", 29*9)
G.inject_variables()
VV = G.gens()

inp = b"sus."
out = b'\x06\x0f"\x02\x8e\xd1'

inp1 = itt(byt_to_int(inp))
out1 = itt(byt_to_int(out))

out22 = itt(byt_to_int(b'\x01\x00\xc9\xe9m=\r\x07x\x04\xab\xd3]\xd3\xcd\x1a\x8e\xaa\x87;<\xf1[\xb8\xe0%\xec\xdb*D\xeb\x10\t\xa0\xb9.\x1az\xf0%\xdc\x16z\x12$0\x17\x8d1'))
out2 = [out22[i:i+9] for i in range(0, len(out22), 9)] # split into chunks

def sub(inp1=None, out1=None):
    work = cc
    if inp1:
        for i in range(28*9, 29*9):
            j = i % 28
            work = work.replace(f"v{i}", f"({inp1[j]})")
    
    work = work.replace("^", "**")

    if out1:
        return [j - out1[i] for i, j in enumerate(eval(work))]
    return eval(work)

def pseudo_enc(pair, inp2):
    inp1, out1 = pair
    dd = sub(inp1, out1)
    ee = sub(inp2)
    ff = [j - i for i, j in zip(dd, ee)]
    print(ff)
    ff = [F(i) for i in ff]
    print(int_to_byt(tti(ff)))

def psuedo_dec(pair, out2):
    solver = PolynomialRing(F, [f"u{i}" for i in range(28*9, 29*9)])
    inp1, out1 = pair
    dd = sub(inp1, out1)
    zz = sub(out1=out2)
    ff = [j - i for i, j in zip(dd, zz)]
    print(ff)
    gg = Ideal(*[solver(str(i).replace("v", "u")) for i in ff])

    print(gg.dimension()) # check for solutions
    out = []
    try: # incase too many solutions
        for i in gg.variety():
            for j in solver.gens():
                out.append(i[j])
    except Exception as e:
        print(e)
    
    return int_to_byt(tti(out))
    
yum = []
for out in out2:
    stu = psuedo_dec((inp1, out1), out)
    yum += itt(byt_to_int(stu))
print(int_to_byt(tti(yum)))
