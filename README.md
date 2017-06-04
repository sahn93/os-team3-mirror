# Team 3 - Project 4

In this project, we developed a new kernel-level mechanism for embedding location information into ext2 file system metadata and used if for access for control. 

This project consists of 5 parts:
tracking device location, 
adding GPS-related operations to inode,
updating location information for files,
user-space testing for location information, 
and file accessing based on location.

## Policies

### 1. Policy that determines when to call set_gps_location

### 2. Policy about distance calculation

Due to the fact that kernel does not support any floating point operations, we had to make several assumptions.

-	We used equirectangular approximation.
-	We assumed the radius of earth is `6378000` meters.
-	We supposed that `pi=3.14`.
-	We made an array `cos` of 10 elements which stores cos(0), cos(10), ..., cos(90) respectively. We calculated average latitude of two points and used `cos` array with round-off.
- We checked if two points has exactly same coordinates and returns `0` in that case.
- We also considered the case when both points has +-90 degree as their latitude. We did not check longitude in this case and returns `0`.
- Yet, we considered about the range of `long long`, sign and endian matters as well. 

## High-level design

### 1. Tracking device location

We defined `struct gps_location` on `include/linux/gps.h` and implemented `set_gps_location` system call on `kernel/gps.c`. Also, we made `test/gpsupdate.c` file which updates GPS location with `set_gps_location` system call.

### 2. Adding GPS-related operations to inode

We added `set_gps_location` and `get_gps_location` interface in `include/linux/fs.h`

### 3. Updating location information for files


### 4. User-space testing for location information




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


