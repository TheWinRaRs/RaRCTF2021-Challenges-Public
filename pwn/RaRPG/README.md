# ConsoleGame
### Build Requirements
`lib/` - should contain `enet.lib` and `pdcurses.lib`.

`include/` - should contain the pdcurses headers, and the enet headers directory
`protobuf` should be installed at `C:/vcpkg/vcpkg/packages/protobuf_x86-windows`

### Runtime Requirements
This requires the enet and pdcurses runtime libraries in order to run (can be distributed with the binary?).

## Solution
The intended method is to use a program like GameConqueror to scan memory for the player's current coordinates. Once the user's X coordinate is found, it can be incremented in order to move them behind the wall of 'W's and touch the hidden door, which will take them to the 'dev room' containing the flag.

