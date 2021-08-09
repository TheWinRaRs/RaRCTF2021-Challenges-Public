p = 115792089210356248762697446949407573530086143415290314195533631308867097853951
order = 115792089210356248762697446949407573529996955224135760342422259061068512044369
a = -3
b = 41058363725152142129326129780047268409114441015993725554835256314039467401291
E = EllipticCurve(GF(p),[a,3]) # our evil, invalid curve
# Order is 115792089210356248762697446949407573529995394580452997270780266901612618829008
# One of the factors is 3
P = E.gens()[0]
print(P)
_p_=P*ZZ(E.order()/3) # This makes the order of _p_ be 3, so we have an incredibly high chance of success.
print(_p_.order())
print(f"Choice point: {_p_}")
print(f"Predict point: {_p_*2}")