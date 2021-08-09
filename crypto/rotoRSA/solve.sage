# thanks will
N = ?
e = 111

def gcd(a,b):
    # custom GCD implementation because Sage's one apparently doesn't work here
    while b:
        a, b = b, a % b
    return a.monic()

P.<x> = PolynomialRing(Zmod(N))
p1 = (poly1) ^ e - ct0
p2 = (poly2) ^ e - ct0
result = -gcd(p1, p2).coefficients()[0]
print(long_to_bytes(result))
