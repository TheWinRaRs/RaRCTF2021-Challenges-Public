from collections import namedtuple
import random
from secret import flag
from Crypto.Util.number import inverse
def moddiv(x,y,p):
    return (x * inverse(y,p)) %p
Point = namedtuple("Point","x y")
class EllipticCurve:
    INF = Point(0,0)
    def __init__(self, a, b, p):
        self.a = a
        self.b = b
        self.p = p
    def add(self,P,Q):
        if P == self.INF:
            return Q
        elif Q == self.INF:
            return P

        if P.x == Q.x and P.y == (-Q.y % self.p):
            return self.INF
        if P != Q:
            Lambda = moddiv(Q.y - P.y, Q.x - P.x, self.p)
        else:
            Lambda = moddiv(3 * P.x**2 + self.a,2 * P.y , self.p)
        Rx = (Lambda**2 - P.x - Q.x) % self.p
        Ry = (Lambda * (P.x - Rx) - P.y) % self.p
        return Point(Rx,Ry)
    def multiply(self,P,n):
        n %= self.p
        if n != abs(n):
            ans = self.multiply(P,abs(n))
            return Point(ans.x, -ans.y % p)
        R = self.INF
        while n > 0:
            if n % 2 == 1:
                R = self.add(R,P)
            P = self.add(P,P)
            n = n // 2
        return R
# P256 parameters, secure.
p = 115792089210356248762697446949407573530086143415290314195533631308867097853951
order = 115792089210356248762697446949407573529996955224135760342422259061068512044369
a = -3
b = 41058363725152142129326129780047268409114441015993725554835256314039467401291
E = EllipticCurve(a,b,p)
print("Welcome to my prediction centre!")
print("We're always looking out for psychics!")
print("We're gonna choose a random number. You get to choose a point. We'll multiply that point by our random number.")
print("Since this curve is of perfect and prime order, it'll be impossible to break this test.")
print("Only a psychic could know!")
print("Be psychic, get the flag.")
x = int(input("Enter point x: "))
y = int(input("Enter point y: "))
P = Point(x,y)
n = random.randint(1,order)
Q = E.multiply(P,n)
print("Ok, where do you think the point will go?")
px = int(input("Enter point x: "))
py = int(input("Enter point y: "))
prediction = Point(px,py)
if prediction == E.INF or prediction == P:
    print("Psychics don't use dirty tricks.")
    quit()
if prediction == Q:
    print("Wow! You're truly psychic!")
    print(flag)
    quit()
print("Better luck next time.")
print(f"Point was {Q}")