# Emoji DB
## Pwn

# Solution
The program provides a standard 'heap note' menu.
```
Emoji DB v 2.1
1) Add new Emoji
2) Read Emoji
3) Delete Emoji
4) Collect Garbage
```

An `Emoji` struct is laid out in the format
```c
/* 0000 */ char data[4];
/* 0004 */ char* name;
/* 000c */
```
When we call 'Add Emoji', `find_free_slot` traverses the `entries` list of pointers, searching for a NULL pointer that can be filled in. `malloc` is then called, allocating space for the `Emoji`, which `malloc(0x80)` reserves space for the name of the emoji.

This input is then null-terminated, and the emoji is read in. UTF-8 uses the number of leading bits to encode the number of bytes in the character:
```
0XXXXXXX - 1 byte
110XXXXX - 2 bytes
1110XXXX - 3 bytes
11110XXX - 4 bytes
```
The program accounts for this by reading the *first* character, counting the number of leading bits, then reading the rest of the data accordingly.

```c
    printf("Enter title: ");
    read(0, new_entry->title, 0x80 - 1);
    new_entry->title[0x80-1] = '\0';
    printf("Enter emoji: ");
    read(0, new_entry->data, 1);
    read(0, new_entry->data+1, count_leading_ones(new_entry->data[0]) - 1);
[ ... ]
int count_leading_ones(unsigned char i) {
    int count = 0;
    while ((i & 0b10000000) > 0) {
        count += 1;
        i = i << 1;
    }
    return count;
}
```

## Bug
4 bytes of space have been reserved for a UTF-8 character. However, if (for instance) the byte 0xff was sent, the program would then read another 7 characters - 4 of which would be out of bounds. We are overflowing into the `name` pointer of the entry. This allows us to overwrite the least significant bytes of the address.

## Deletion
Another important note is how the program handles deleting an entry. A 'garbage' list is maintained, with enough room for 50 pointers. When we delete an entry, the address of the `Emoji` and then its `name` field are added to this list, and their pointers are nulled in the original list.

When the 'Collect garbage' option is chosen, the `garbage` list is iterated over. Each non-null pointer is freed and then nulled out, allowing for more control over when pointers are freed.

