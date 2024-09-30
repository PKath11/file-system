#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include "wfs.h"

int main(int argc, char *argv[]) {
    char *disk_img = NULL;
    size_t num_inodes = 0;
    size_t num_blocks = 0;

    // parse command line args
    int op;
    while ((op = getopt(argc, argv, "d:i:b:")) != -1) {
        switch (op) {
            case 'd':
                disk_img = optarg;
                break;
            case 'i':
                num_inodes = atoi(optarg);
                break;
            case 'b':
                num_blocks = atoi(optarg);
                break;
            default:
                printf("Usage: mkfs -d disk_img -i num_inodes -b num_blocks\n");
                return 1;
        }
    }

    // ensure args are valid
    if (!disk_img || num_inodes <= 0 || num_blocks <= 0) {
        printf("Usage: mkfs -d disk_img -i num_inodes -b num_blocks\n");
        return 1;
    }

    // open the disk image file
    int fd = open(disk_img, O_RDWR);
    if (fd == -1) {
        printf("failed to open disk image file");
        return 1;
    }
    
    // round up the number of blocks to the nearest multiple of 32
    num_blocks = num_blocks % 32 != 0 ? ((num_blocks / 32) + 1) * 32 : num_blocks;
    num_inodes = num_inodes % 32 != 0 ? ((num_inodes / 32) + 1) * 32 : num_inodes;

    // init the superblock
    struct wfs_sb sb;
    sb.num_inodes = num_inodes;
    sb.num_data_blocks = num_blocks;
    sb.i_bitmap_ptr = sizeof(struct wfs_sb);
    sb.d_bitmap_ptr = sb.i_bitmap_ptr + (num_inodes / 8);
    sb.i_blocks_ptr = sb.d_bitmap_ptr + (num_blocks / 8);
    sb.d_blocks_ptr = sb.i_blocks_ptr + num_inodes * BLOCK_SIZE;

    struct stat img_info;
    if (fstat(fd, &img_info) == -1) {
        close(fd);
        return 1;
    }
    // check if disk image file is too small to accomodate the number of blocks
    if (sb.d_blocks_ptr + (num_blocks * BLOCK_SIZE) >= img_info.st_size) {
        printf("disk image file is too small to accomodate the number of blocks\n");
	close(fd);
        return 1;
    }

    char *disk = mmap(NULL, img_info.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    memcpy((struct wfs_sb *) disk, &sb, sizeof(struct wfs_sb));
    *(disk + sb.i_bitmap_ptr) = 1;
    struct wfs_inode *root_inode = (struct wfs_inode *) (disk + sb.i_blocks_ptr);
    root_inode->num = 0;
    root_inode->mode = S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH;
    root_inode->uid = getuid();
    root_inode->gid = getgid();
    root_inode->size = 0;
    root_inode->nlinks = 0;
    root_inode->atim = time(NULL);
    root_inode->mtim = time(NULL);
    root_inode->ctim = time(NULL);
    
    close(fd);
    return 0;
}
