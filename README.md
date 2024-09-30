# Custom Filesystem Project using FUSE

## Introduction

In this project, you'll find the implementation of a custom filesystem using FUSE (Filesystem in Userspace). FUSE allows users to create their own filesystems without requiring special permissions, offering flexibility in designing and using filesystems. This project handles basic tasks like reading, writing, making directories, deleting files, and more.

## Objectives

- To understand how filesystem operations are implemented.
- To implement a traditional block-based filesystem.
- To learn to build a user-level filesystem using FUSE.

## Background

### FUSE

FUSE (Filesystem in Userspace) is a framework that enables the creation of custom filesystems in user space. This simplifies filesystem development and allows using standard programming languages like C, C++, Python, and others.

## Mounting
The mountpoint is a directory where the FUSE-based filesystem will be attached or "mounted." Once mounted, this directory serves as the entry point to access and interact with the FUSE filesystem.

## Filesystem Details
This filesystem is similar to traditional filesystems like FFS or ext2. It has a superblock, inode and data block bitmaps, and inodes and data blocks. The filesystem supports two file types: directories and regular files.

### Project Details

## mkfs.c
This program initializes a file to an empty filesystem. It receives three arguments: the disk image file, the number of inodes, and the number of data blocks. The number of blocks is rounded up to the nearest multiple of 32.
Example: ```./mkfs -d disk_img -i 32 -b 200```
This initializes disk_img to an empty filesystem with 32 inodes and 224 data blocks.

## wfs.c
This file contains the implementation for the FUSE filesystem. Running this program mounts the filesystem to a specified mount point.
Usage: ```./wfs disk_path [FUSE options] mount_point```

## Features
The filesystem implements the following features:
 + Create empty files and directories
 + Read and write to files up to the maximum size supported by the indirect data block
 + Read a directory (e.g., ls should work)
 + Remove an existing file or directory (assume directories are empty)
 + Get attributes of an existing file/directory

## Utilities:
 - create_disk.sh: Creates a file named disk with size 1MB whose content is zeroed.
 - umount.sh: Unmounts a mount point specified in the first argument.
 - Makefile: Template makefile used to compile the code.

## Compilation and Execution
Typical commands to compile and launch the filesystem:
```
$ make
$ ./create_disk.sh                 # creates file `disk.img`
$ ./mkfs -d disk.img -i 32 -b 200  # initialize `disk.img`
$ mkdir mnt
$ ./wfs disk.img -f -s mnt         # mount. -f runs FUSE in foreground
```
You can interact with the filesystem once it is mounted:
```
$ stat mnt
$ mkdir mnt/a
$ stat mnt/a
$ mkdir mnt/a/b
$ ls mnt
$ echo asdf > mnt/x
$ cat mnt/x
```
