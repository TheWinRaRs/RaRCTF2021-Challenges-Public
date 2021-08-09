import base64
MORSE_CODE_DICT = { 'A':'.-', 'B':'-...',
                    'C':'-.-.', 'D':'-..', 'E':'.',
                    'F':'..-.', 'G':'--.', 'H':'....',
                    'I':'..', 'J':'.---', 'K':'-.-',
                    'L':'.-..', 'M':'--', 'N':'-.',
                    'O':'---', 'P':'.--.', 'Q':'--.-',
                    'R':'.-.', 'S':'...', 'T':'-',
                    'U':'..-', 'V':'...-', 'W':'.--',
                    'X':'-..-', 'Y':'-.--', 'Z':'--..',
                    '1':'.----', '2':'..---', '3':'...--',
                    '4':'....-', '5':'.....', '6':'-....',
                    '7':'--...', '8':'---..', '9':'----.',
                    '0':'-----', ', ':'--..--', '.':'.-.-.-',
                    '?':'..--..', '/':'-..-.', '-':'-....-',
                    '(':'-.--.', ')':'-.--.-', ' ': '/'}

# Ensure flag is padded so that '=' are not in the result
flag = "rarctf{d1d_y0u_p33k_0r_5py????_fa4ac605}"
check = f"THE FLAG IS {base64.b32encode(flag.encode()).decode()} BASE32"

template = """using System;

namespace Dotty {
    class Check {
        public static string check = "%s";
    }
}
""" % '|'.join(MORSE_CODE_DICT[c] for c in check)

with open('Check.cs', 'w') as f:
    f.write(template)

print(check)
