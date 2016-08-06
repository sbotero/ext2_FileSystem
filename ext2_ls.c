#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "ext2.h"

int flag_a;

void print_entries(struct ext2_dir_entry_2 *dir_entry){
	//read entries of directory, from http://cs.smith.edu/~nhowe/262/oldlabs/ext2.html
 
	// if directory entry doesn't point to a directory, simply print the file/link name if it exists
	if ((dir_entry->file_type == EXT2_FT_REG_FILE) || (dir_entry->file_type == EXT2_FT_SYMLINK)) {
		char buf[dir_entry->name_len + 1];
		memcpy(buf, dir_entry->name, dir_entry->name_len);
		buf[dir_entry->name_len] = '\0';
		printf("%s\n", buf);
	}
	// if the directroy entry points to a directory
	else {
		// inode for the directory
		struct ext2_inode *in = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * gd->bg_inode_table + sb->s_inode_size * (dir_entry->inode - 1));
		struct ext2_dir_entry_2 *curr_dir;
		int i;
		for (i = 0; i < BLOCK_N; i++) {
			if (i < 12 && *(in->i_block + i)) {
				int remainding_blk_size = EXT2_BLOCK_SIZE;
				int off_set = 0;
				while (remainding_blk_size > 0) {
 					curr_dir = (struct ext2_dir_entry_2 *) (disk + *(in->i_block) * EXT2_BLOCK_SIZE + off_set);
					char buf[curr_dir->name_len + 1];
					memcpy(buf, curr_dir->name, curr_dir->name_len);
					buf[curr_dir->name_len] = '\0';
					if (flag_a) {
						printf("%s\n", buf);
					}
					else {
						if (strcmp(".", buf) && strcmp("..", buf)) {
							printf("%s\n", buf);
						}
					}         
					off_set = off_set + curr_dir->rec_len;
					remainding_blk_size = remainding_blk_size - curr_dir->rec_len;
 				}
 			}
			// Handle single indirection
			else {
				int j;
				for (j = 0; j < EXT2_BLOCK_SIZE - 4; j = j + 4) {
					if (*(disk + EXT2_BLOCK_SIZE * *(in->i_block + i) + j)) {
						int block_num = *(disk + EXT2_BLOCK_SIZE * *(in->i_block + i) + j);
						int remainding_blk_size = EXT2_BLOCK_SIZE;
						int off_set = 0;
						while (remainding_blk_size > 0) {
							curr_dir = (struct ext2_dir_entry_2 *) (disk + block_num * EXT2_BLOCK_SIZE + off_set);
							char buf[curr_dir->name_len + 1];
							memcpy(buf, curr_dir->name, curr_dir->name_len);
							buf[curr_dir->name_len] = '\0'; 
							if (flag_a) {
								printf("%s\n", buf);
							} 
							else {
								if (strcmp(".", buf) && strcmp("..", buf)) {
									printf("%s\n", buf);
								}
							}      
							off_set = off_set + curr_dir->rec_len;
							remainding_blk_size = remainding_blk_size - curr_dir->rec_len;
						}          
					}
				}           
			}
		}
	}
}

int main(int argc, char **argv){

	int opt;
	char *disk_name;
	char *path_name;
	flag_a = 0;

	if (argc > 4) {
		fprintf(stderr, "ext2_ls disk_name path_name [-a]\n" );
		exit(1);
	}

	while ((opt = getopt(argc, argv, "a")) != -1) {
		switch (opt) {
		case 'a':
			flag_a = 1;
			break;
		default:
			fprintf(stderr, "ext2_ls disk_name path_name [-a]\n" );
			exit(1);
		}
	}
    
	if (flag_a) {
		disk_name = argv[2];
		path_name = argv[3];
	} else {
		disk_name = argv[1];
		path_name = argv[2];
	}

    
	int fd = open(disk_name, O_RDWR);
	disk = mmap(NULL, 128 * EXT2_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(disk == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	close(fd);
	sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
	gd = (struct ext2_group_desc *)(disk + 2 * EXT2_BLOCK_SIZE);
	
	struct ext2_dir_entry_2 *dir = path_traversal(path_name);
	if (!dir){
		fprintf(stderr, "No such file or directory\n");
		return ENOENT;
	}

	print_entries(dir);

	return 0;
}
