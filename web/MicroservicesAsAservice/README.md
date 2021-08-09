# Microservices As A Service (Part 1: Calculator)
## Web

The 'add' function concatenates strings instead of adding them as numbers.

The returned string will be `eval`'ed to check if it is an integer. We can see if an arbitrary Python expression evaluates to an integer.

`1 if open('/flag.txt', 'r').read()[0] == 'r' else None`

Using this we can bruteforce each byte of the flag to discover its contents.
`rarctf{0v3rk1ll_4s_4_s3rv1c3_3fca0faa}`
## Part 2: Notes

UNINTENDED METHOD: When adding a bio, the filter is never actually applied, so SSTI can be easily performed
`{{request.application.__globals__.__builtins__.open('/flag.txt').read()}}`

Intended method:
There are two Redis servers - `redis_users` and `redis_userdata`. `redis_users` simply maps usernames to ports. `redis_userdata` creates a new Redis instance for every user who signs up on a unique port. When a user adds a 'key', their port is looked up, then the key/value are stored on their Redis instance. When they edit their bio, it is saved to `/tmp/<port>.txt` on `redis_userdata`.

Here is my method for solving.

First, two users are created, with names 'user1' and 'user2'. 'user1' creates a key with the name 'user2' and the value '../bio/<port1>' - where `port1` is the port of user1's Redis instance.
Then, user1 performs a key transfer - with the target being `redis_users:6379`. This changes user2's Redis port.

User2 then adds a key, with the name 'bio' and the value being an SSTI payload that reads the flag:
`{{get_flashed_messages.__globals__.__builtins__.open("/flag.txt").read()}}`. The URL that is requsted is `http://redis_userdata:5000/putuser/../bio/<port1>`. This endpoint is 'behind' the SSTI filter, meaning that the 'bio' key is read from their request and written to the users bio file. Finally, user1 can now read their own profile, and the unfiltered SSTI payload will be read and executed, giving the flag.
`rarctf{wh4t_w4s_1_th1nk1ng..._60a4ee96}`

## Part 3: User Manager

UNINTENDED METHOD: There were two instances of improper network isolation, meaning that code execution on part 1 or 2 allowed users to bypass the filter. I released a patch for one of these, but unfortunately missed the fact that I had accidentally placed the 'updater' container on the 'public' network, meaning it could still be reached.

The intended method abused the fact that different JSON parsers can parse the same payload differently. Specifically, most parsers will, upon encountering duplicate keys, only retain the last one. Some parsers however, such as the `buger/jsonparser` Golang module take the *first* key.

When updating a user's password, the app runs it through the Python `jsonschema` module. The password is validated to be of a minimum length, and that the 'target' user has a higher ID than the user doing the updating - the intent of this is to act as a kind of highly simplistic linear privilege model - each user can only manage users created after them.
To solve, one could simply send a JSON payload of the form `{"id": 0, "id": 1000000, "password": "ABCDEFGHIJKLM"}` - the Python parser would take the final value, see it as valid, and pass it to the Golang parser, which would see the *first* ID, and update the admin's password. Users could then log in as the admin and retrieve the flag.
