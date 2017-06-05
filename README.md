# Team 3 - Project 4

In this project, we developed a new kernel-level mechanism for embedding location information into ext2 file system metadata and use it for access for control. 

This project consists of 5 parts:
tracking device location, 
adding GPS-related operations to inode,
updating location information for files,
user-space testing for location information, 
and file accessing based on location.

## Policies

### 1. Policy about distance calculation

Due to the fact that kernel does not support any floating point operations, we had to make several assumptions.

-	We used equirectangular approximation.
-	We assumed the radius of earth is `6378000` meters.
-	We supposed that `pi=3.14`.
-	We made an array `cos` of 10 elements which stores cos(0), cos(10), ..., cos(90) respectively. We calculated average latitude of two points and used `cos` array with round-off.
- We checked if two points has exactly same coordinates and returns `0` in that case.
- We also considered the case when both points has +-90 degree as their latitude. We did not check longitude in this case and returns `0`.
- Yet, we considered about the range of `long long`, sign and endian matters as well. 

### 2. Error check in get_gps_location

- We assumed the maximum path length as `200` and returned `-1` if the path length is longer than `200`.
- We returned `EINVAL`, `EACCES`, `ENODEV` properly.

### 3. Initial GPS value
- We statically initialized every GPS-related variables to 0.

## High-level design

### 1. Tracking device location

We defined `struct gps_location` on `include/linux/gps.h` and implemented `set_gps_location` system call on `kernel/gps.c`. Also, we made `test/gpsupdate.c` file which updates GPS location with `set_gps_location` system call.

### 2. Adding GPS-related operations to inode

We added `set_gps_location` and `get_gps_location` interface in `include/linux/fs.h`

### 3. Updating location information for files

We made the file system to update regular files' location information when they are created or modified. We explained detail implementation below in Implementation.

### 4. User-space testing for location information

We modified `e2fsprogs/lib/ext2fs/ext2_fs.h` to support location information. 

### 5. Location-based file access

We compared the distance between the current device and the file location with `accuracy`. We calculated the distance as we mentioned in Policies.

## Implementation

### 1. Determine when to call set_gps_location

- First, we searched where the `i_mtime` or the `i_ctime` of inode change in `/fs` and `/fs/ext2`. Then we called `inode->i_op->set_gps_location(inode)` after every change of `i_mtime` and `i_ctime`. 

### 2. Modify ext2 inode structure

- We added below items in `struct ext2_inode` and `struct ext2_inode_large` in `e2fsprogs/lib/ext2fs/ext2_fs.h`
```c
	__le32	i_lat_integer;
	__le32	i_lat_fractional;
	__le32	i_lng_integer;
	__le32	i_lng_fractional;
	__le32	i_accuracy;
```
- We also added each gps information in big endian notation in `struct ext2_inode_info`
- We built `proj4.fs` file and pushed it into ARTIK device.

## How to build kernel & Test

### Build & flash kernel
1. Type `build` on the root directory to build kernel.
2. Type `flash` to upload kernel to the ARTIK.

### Compile & push test code
1. Type `make` to compile two test programs (`file_loc`, `gpsupdate`).
2. If you want to compile individual program, just append its name after `make` (e.g. `make file_loc`, `make gpsupdate`).
3. Connect SDB by typing `direct_set_debug.sh --sdb-set` on Artik.
4. Type `push <source> <destination>` to send a file to Artik.

### Run the test


## What we have learned

We learned how to modify the file system, how to use ext2 file system. Also, we experienced delay when we change ext2 inode structure. We learned how to use submodule in github while we use e2fsprogs. In addition, we experienced different aspect of our kernel when we execute `file_loc` at `sdb_shell` and shell connected by serial port. We learned kernel does not support any floating point operations and we found out other approach: using `do_div()`. 
