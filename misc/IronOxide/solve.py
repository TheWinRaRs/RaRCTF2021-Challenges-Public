from pwn import *
while True:
    p = process("./IronOxide") if not args.REMOTE else remote(args.HOST, int(args.PORT))
    import itertools
    from functools import reduce
    data = []
    sign = lambda x: 0 if x == 0 else x // abs(x)
    from operator import add
    from functools import reduce
    for i in range(25):
        idx = 0
        data.append({})
        for _ in range(24):
            p.recvuntil(f": ")
            if idx == i:
                idx += 1
            output = p.recvline()[:-1].decode().split(", ")
            output[1] = int(output[1])
            output[2] = float(output[2] if output[2] != 'NaN' else '0')
            data[i][idx] = output
            idx += 1
    elemprops = []
    with open("elemprops.csv") as csvfile:
        csvfile.readline()
        for i,line in enumerate(csvfile.readlines()[:62]):
            line = line[:-1].split(',')
            elemprops.append({"number": int(line[0]), "symbol": line[1], "valence": int(line[2]),
            "electroneg": float(line[3])})


    numbercombos = {}
    for combo in itertools.product(range(1,63),repeat=2):
        diff = abs(combo[0] - combo[1])
        if diff in numbercombos.keys():
            numbercombos[diff].append(combo)
        else:
            numbercombos[diff] = [combo]
    bondcombos = {}
    for combo in itertools.product(range(1,63),repeat=2):
        num1 = elemprops[combo[0] - 1]["valence"]
        num2 = elemprops[combo[1] - 1]["valence"]
        if sign(num1) == 1 and sign(num2) == 1:
            result = "Metallic"
        elif sign(num1) == -1 and sign(num2) == -1:
            result = "Covalent"
        elif sign(num1) == 0 or sign(num2) == 0:
            result = "No Reaction"
        else:
            result = "Ionic"
        if result in bondcombos.keys():
            bondcombos[result].append(combo)
        else:
            bondcombos[result] = [combo]
    electrocombos = {}
    for combo in itertools.product(range(1,63),repeat=2):
        num1 = elemprops[combo[0] - 1]["electroneg"]
        num2 = elemprops[combo[1] - 1]["electroneg"]
        if num1 == 0 or num2 == 0:
            diff = 0
        else:
            diff = round(abs(num1 - num2),2)
        if diff in electrocombos.keys():
            electrocombos[diff].append(combo)
        else:
            electrocombos[diff] = [combo]

    results = []
    for char in data:
        possibles = []
        for reaction in char:
            bond, numdiff, electrodiff = char[reaction]
            sets = set(reduce(add, numbercombos[numdiff])), set(reduce(add, bondcombos[bond])), set(reduce(add, electrocombos[electrodiff]))
            possibles.extend(sets)
        result = reduce(set.intersection, possibles)
        print(result)
        results.append(result)
    p.recv()
    combos = list(itertools.product(*results))
    print(len(combos))
    if len(combos) >= 30:
        continue
    for combo in itertools.product(*results):
        combo = [x + 64 for x in combo]
        print(combo)
        key = bytes(combo)
        p.sendline(key)
        data = p.recv()
        if b'rarctf' in data:
            print(data)
            quit()
