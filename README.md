# Team 3 - Project 3

In this project, we will build our own CPU scheduler in the Tizen Linux kernel. Then, we test the scheduler with the ARTIK device.


## Policies


## High-level design



## Implementation

### 1. Data Structure


### 2. Functions



## How to build kernel & Test
### Build & flash kernel
1. Type `build` on the root directory to build kernel.
2. Type `flash` to upload kernel to the ARTIK.

### Compile & push test code
1. Type `arm-linux-gnueabi-gcc <source file> -o <output name>` to compile test code.
2. Connect SDB by typing `direct_set_debug.sh --sdb-set` on Artik.
3. Type `push <source> <destination>` to send a file to Artik.

### Run the test


## What we have learned

