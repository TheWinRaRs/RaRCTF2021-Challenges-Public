out = "281 547 54 380 392 98 158 440 724 218 406 672 193 457 694 208 455 745 196 450 724".split(" ")
out = [int(x) for x in out]

exec('def f(x):'+'yield((x:=-~x)*x+-~-x)%727;'*100)

for i in range(727):
  g=f(i)
  a = (list([*map(lambda c:c^next(g),out)]))
  if all([x < 128 for x in a]):
    print(i)
    print("".join([chr(x) for x in a]))
