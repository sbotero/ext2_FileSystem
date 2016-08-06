#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "ext2.h"

unsigned char *disk;
int flag;
struct ext2_super_block *sb;
struct ext2_group_desc *gd;

void copy(struct ext2_dir_entry_2 *target_dir, int fd, char *file_name) {
	// Check if copy operation can be done
	
	// Check the file size
	int off_set = lseek(fd, 0, SEEK_END);
	if (off_set == -1) {
	    return;
	}
	int file_size = lseek(fd, 0, SEEK_CUR);
	if (file_size == -1) {
	    return;
	}
	// Reposition file offset
	int reposition = lseek(fd, 0, SEEK_SET);
	if (reposition == -1) {
	    return;
	}
	
	// Find out how many blocks are data are required to store the file data
	int num_of_blocks = file_size / EXT2_BLOCK_SIZE;
	if ((num_of_blocks % EXT2_BLOCK_SIZE) > 0) {
	    num_of_blocks++;
	}
	
	// Need another extra block for single indirection
	int single_indirection_capacity = EXT2_BLOCK_SIZE / 4;
	if (num_of_blocks > 12) {
	    int single_indirection_blocks = (num_of_blocks - 12) / single_indirection_capacity;
	    if ((num_of_blocks - 12) % single_indirection_capacity) {
	        single_indirection_blocks++;
	    }
	    num_of_blocks += single_indirection_blocks;
	}
	
	// Find an inode for the file to be copied
	int i,j,k;
	int inode_bitmap_block_num = gd->bg_inode_bitmap;
	int num_of_bytes_inode = (sb->s_inodes_count) / 8;
	int free_inode_num = 0;
	int current_inode_num = 0;
	
	for (i = 0; i < num_of_bytes_inode; i++) {
		k = *(disk + EXT2_BLOCK_SIZE * inode_bitmap_block_num + i);
		for (j = 0; j < 8; j++) {
			current_inode_num++;
			if ((free_inode_num == 0) && k % 2 == 0) {
				free_inode_num = current_inode_num;
				// Set bit in bitmap
				*(disk + EXT2_BLOCK_SIZE * inode_bitmap_block_num + i) += pow(2, j);
			}
			k = k/2;
		}
	}
	
	if (free_inode_num == 0) {
		return;
	}
	
	// Set inode bit in inode bitmap
	
	// Instantiate the inode
	struct ext2_inode *new_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (free_inode_num - 1)));
	
	new_inode->i_mode = EXT2_FT_REG_FILE;
	new_inode->i_size = file_size;
	new_inode->i_dtime = 0;
	new_inode->i_links_count = 1;
	new_inode->i_blocks = num_of_blocks * 2;
	
	// Find empty blocks to store the file data
	int block_bitmap_block_num = gd->bg_block_bitmap;
	int num_of_bytes_block = (sb->s_blocks_count) / 8;
	int current_block_num = 0;
	int blocks[num_of_blocks];
	int block_index = 0;
	
	for (i = 0; i < num_of_bytes_block; i++) {
		k = *(disk + EXT2_BLOCK_SIZE * block_bitmap_block_num + i);
		for (j = 0; j < 8; j++) {
			current_block_num++;
			if (k % 2 == 0 && block_index < num_of_blocks) {
				blocks[block_index] = current_block_num;
				block_index++;
				// Set bit in bitmap
				*(disk + EXT2_BLOCK_SIZE * block_bitmap_block_num + i) += pow(2, j);
			}
			k = k/2;
		}
	}
	
	// If there's not enough free blocks
	if (block_index < num_of_blocks) {
		return;
	}
	
	// Set i_blocks for inode
	for (i = 0; i < BLOCK_N; i++) {
		if (i < num_of_blocks) {
			int single_indirection_block;
			int single_indirection_count = 0;
			if (i < 12) {
				memcpy(&(new_inode->i_block[i]), &blocks[i], sizeof(int));
			}
			else {
				if ((i - 11) % (single_indirection_capacity + 1) == 1) {
					memcpy(&(new_inode->i_block[i]), &blocks[i], sizeof(int));
					single_indirection_block = blocks[i];
					single_indirection_count++;
				}
				// FIX LATER!!! INCORRECT
				else {
					int block_address_offset = (((i - 11 - 1 * single_indirection_count) % (single_indirection_capacity + 1)) - 1) * 4;
					*(disk + EXT2_BLOCK_SIZE * single_indirection_block + block_address_offset) = blocks[i];
				}
			}      
		}
		else {
			new_inode->i_block[i] = 0;
		}
	}
	
	// Find an address (location) to place the new directory entry
	struct ext2_inode *parent_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (target_dir->inode - 1)));
	
	int size_of_new_dir = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char) * strlen(file_name);
	
	// Make new directory entry size 4B aligned
	int remainder = size_of_new_dir % 4;
	int lack_in_bytes = 4 - remainder;
	if (lack_in_bytes < 4) {
		size_of_new_dir += lack_in_bytes;
	}
	
	struct ext2_dir_entry_2 *new_dir;
	
	int found_space = 0;
	
	for (i = 0; i < BLOCK_N; i++) {
		if (found_space) {
			break;
		}
		if (*(parent_inode->i_block + i)) {
			if (i < 12) {
				int offset = 0;
				while (offset != EXT2_BLOCK_SIZE) {
					struct ext2_dir_entry_2 *curr_dir = (struct ext2_dir_entry_2 *) (disk + *(parent_inode->i_block + i) * EXT2_BLOCK_SIZE + offset);
					// Check if curr_dir is the last directory entry in the block
					if ((offset + curr_dir->rec_len) == EXT2_BLOCK_SIZE) {
						// Check if the directory entry has enough empty space
						int curr_dir_required_size = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char) * strlen(curr_dir->name);
							remainder = curr_dir_required_size % 4;
							lack_in_bytes = 4 - remainder;
							if (lack_in_bytes < 4) {
								curr_dir_required_size += lack_in_bytes;
							}
							int space_available = curr_dir->rec_len - curr_dir_required_size;
		                    
							// If there is enough space for the new directory entry
							if (space_available >= size_of_new_dir) {
								found_space = 1;
								curr_dir->rec_len = curr_dir_required_size;
								new_dir = (struct ext2_dir_entry_2 *) (disk + *(parent_inode->i_block + i) * EXT2_BLOCK_SIZE + offset + curr_dir->rec_len);
								new_dir->inode = free_inode_num;
								new_dir->rec_len = size_of_new_dir;
								new_dir->name_len = strlen(file_name);
								new_dir->file_type = EXT2_FT_REG_FILE;
								memcpy(new_dir->name, file_name, strlen(file_name));
								break;
							}
						}
						offset += curr_dir->rec_len;
					}
				}
				// Handle single indirection
				else {
					for (j = 0; j < EXT2_BLOCK_SIZE; j = j + 4) {
						if (*(disk + EXT2_BLOCK_SIZE * *(parent_inode->i_block + i) + j)) {
							int curr_block_num = *(disk + EXT2_BLOCK_SIZE * *(parent_inode->i_block + i) + j);
							int offset = 0;
							while (offset != EXT2_BLOCK_SIZE) {
								struct ext2_dir_entry_2 *curr_dir = (struct ext2_dir_entry_2 *) (disk + curr_block_num * EXT2_BLOCK_SIZE + offset);
								// Check if curr_dir is the last directory entry in the block
								if ((offset + curr_dir->rec_len) == EXT2_BLOCK_SIZE) {
									// Check if the directory entry has enough empty space
									int curr_dir_required_size = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char) * strlen(curr_dir->name);
									remainder = curr_dir_required_size % 4;
									lack_in_bytes = 4 - remainder;
									if (lack_in_bytes < 4) {
										curr_dir_required_size += lack_in_bytes;
									}
									int space_available = curr_dir->rec_len - curr_dir_required_size;
		                            
									// If there is enough space for the new directory entry
									if (space_available >= size_of_new_dir) {
										found_space = 1;
										curr_dir->rec_len = curr_dir_required_size;
										new_dir = (struct ext2_dir_entry_2 *) (disk + *(parent_inode->i_block + i) * EXT2_BLOCK_SIZE + offset + curr_dir->rec_len);
										new_dir->inode = free_inode_num;
										new_dir->rec_len = size_of_new_dir;
										new_dir->name_len = strlen(file_name);
										new_dir->file_type = EXT2_FT_REG_FILE;
										memcpy(new_dir->name, file_name, strlen(file_name));
										break;
									}
								}
								offset += curr_dir->rec_len;
							}
						}
					}
				}
			}
		}
	
	if (found_space == 0) {
		return;
	}

	i = 0;
	void *buf = NULL;
	
	while (read(fd, buf, EXT2_BLOCK_SIZE) != -1) {
		memcpy((void *)(disk + EXT2_BLOCK_SIZE * blocks[i]), buf, EXT2_BLOCK_SIZE);
		i++;
	}
}

int main(int argc, char **argv){
	char *disk_name = argv[1];
	char *source_name = argv[2];
	char *dest_name = argv[3];
    
	int fd = open(disk_name, O_RDWR);
	disk = mmap(NULL, 128 * EXT2_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(disk == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	close(fd);

	sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
	gd = (struct ext2_group_desc *)(disk + 2 * 1024);
   
	struct ext2_dir_entry_2 *tar_dir = path_traversal(dest_name);
	if (!tar_dir){
		return ENOENT;	
	}
    
   char *token = strtok(source_name, "/");
   char *file_name;
	while (token){
		file_name = token;
		token = strtok(NULL, "/");	
	}
	
	fd = open(source_name, O_RDONLY);
	copy(tar_dir, fd, file_name);
	close(fd);
    
	return 0;
}