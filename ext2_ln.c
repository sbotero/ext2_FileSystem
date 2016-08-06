#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "ext2.h"

int flag_s;

int find_free_block() {

	// Find a free block 
    int block_bitmap_block_num = gd->bg_block_bitmap;
    int num_of_bytes_block = (sb->s_blocks_count) / 8;
    int free_block_num, current_block_num, i, k, j;
    current_block_num = free_block_num = 0;
    
    for (i = 0; i < num_of_bytes_block; i++) {
        k = *(disk + EXT2_BLOCK_SIZE * block_bitmap_block_num + i);
        for (j = 0; j < 8; j++) {
            current_block_num++;
            if ((free_block_num == 0) && k % 2 == 0) {
                free_block_num = current_block_num;
            }
            k = k/2;
        }
    }
    
    if (free_block_num == 0) {
        return -1;
    }

    return free_block_num;

}




struct ext2_dir_entry_2 * get_prev_dir(char * path_name) {

	int i, found;
	struct ext2_inode * curr_inode = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * gd->bg_inode_table + sb->s_inode_size);
	struct ext2_dir_entry_2 * curr = (struct ext2_dir_entry_2 *) NULL;
	struct ext2_dir_entry_2 * prev = (struct ext2_dir_entry_2 *) NULL;

	char *token = strtok(path_name, "/");

	while (token) {

		found = 0;
		for (i = 0; i < BLOCK_N; i++) {

			int size;
			size = 0;

			curr = (struct ext2_dir_entry_2 *) (disk + (curr_inode->i_block[i]) * EXT2_BLOCK_SIZE);

			if (curr_inode->i_block[i]) {
				while (size < EXT2_BLOCK_SIZE) {

					char buf[curr->name_len + 1];
		            strncpy(buf, curr->name, curr->name_len);
		            buf[curr->name_len] = '\0';

					if (!strcmp(buf, token)){

						found = 1;
						break;   
		            }
		            
		            size += curr->rec_len;
		            prev = curr;
		            curr = (struct ext2_dir_entry_2 *) ((disk + (curr_inode->i_block[i]) * EXT2_BLOCK_SIZE) + size);
				}

				if (found) {
					break;
				}
			}
		}

		curr_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (curr->inode - 1)));
		token = strtok(NULL, "/");
	}

    if (found) {
    	return prev;
    }
    return NULL;

}

int main(int argc, char **argv){
	char *disk_name;
	char *source_path_temp;
	char *target_path;
	int opt;

	if (argc > 5) {
		fprintf(stderr, "ext2_ln disk_name source_name path_name [-s]\n" );
		exit(1);
	}
	
	while ((opt = getopt(argc, argv, "s")) != -1) {
		switch (opt) {
		case 's':
			flag_s = 1;
			break;
		default:
			fprintf(stderr, "ext2_ln disk_name source_name path_name [-s]\n" );
			exit(1);
		}
	}
	
	if (flag_s) {
		disk_name = argv[2];
		source_path_temp = argv[3];
		target_path = argv[4];
	} else {
		disk_name = argv[1];
		source_path_temp = argv[2];
		target_path = argv[3];
	}
	

	char source_path[strlen(source_path_temp)];
	strncpy(source_path, source_path_temp, strlen(source_path_temp) + 1);
	
	int fd = open(disk_name, O_RDWR);
	disk = mmap(NULL, 128 * EXT2_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(disk == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	close(fd);
	sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
	gd = (struct ext2_group_desc *)(disk + 2 * EXT2_BLOCK_SIZE);
 
 	struct ext2_dir_entry_2 *source_entry = path_traversal(source_path);
 	struct ext2_dir_entry_2 *target_entry = path_traversal(target_path);


 	if (!target_entry){
		return ENOENT;	
	}

	if ((source_entry && source_entry -> file_type == EXT2_FT_DIR) || target_entry -> file_type == EXT2_FT_DIR) {
		return EISDIR;
	}

	if (source_entry){
		return EEXIST;	
	}

	strncpy(source_path, source_path_temp, strlen(source_path_temp) + 1);

	char temp[strlen(source_path)];
	char * filename;
	char * token = strtok(source_path, "/");

	int name_len;
	while (token) {

		name_len = strlen(token);
		strncpy(temp, token, name_len + 1);


		if (!(token = strtok(NULL, "/"))) {
			filename = temp;
		}
	}

	strncpy(source_path, source_path_temp, strlen(source_path_temp) + 1);
	source_path[strlen(source_path_temp) - name_len] = 0;

	struct ext2_dir_entry_2 * hold_link_dir = path_traversal(source_path);

	if (!hold_link_dir) {
		return ENOENT;
	}

	struct ext2_dir_entry_2 *new_entry = (struct ext2_dir_entry_2 *) NULL;

	int free_block_num;

 	if (flag_s) {

 		struct ext2_inode * new_inode = (struct ext2_inode *) NULL;
 		int need_block = 0;

 		new_inode -> i_mode = EXT2_S_IFLNK;
 		new_inode -> i_ctime = (unsigned int) time(NULL);
 		new_inode -> i_dtime = 0;
 		new_inode -> i_size = sizeof(struct ext2_inode);
 		new_inode -> i_links_count = 2; //. and ..
 		new_inode -> i_blocks = 2;
 		if (strlen(filename)<= 60) {

 			memcpy((char *) new_inode -> i_block, filename, strlen(filename));
 		} else {
 			need_block = 1;
 		}

 		if (need_block) {
 			// Store to symbolic link path in block

	 		if ((free_block_num = find_free_block()) == -1) {

				perror("No Free Blocks");
				exit(1);
			}
 		}

 	}
 	else {

		new_entry->inode = target_entry -> inode;
		new_entry->rec_len = target_entry -> rec_len;
		new_entry->file_type = target_entry -> file_type;
		new_entry->name_len = name_len;
		strncpy(new_entry->name, filename, name_len);


		int free_block_num;

		if ((free_block_num = find_free_block()) == -1) {

			perror("No Free Blocks");
			exit(1);
		}

		struct ext2_inode * inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (hold_link_dir->inode - 1)));

		int i;
		for (i = 0; i < BLOCK_N; i++) {

			if (i < 12) {

				if (!inode->i_block[i]) {

					inode->i_block[i] = free_block_num;
					break;
				}
			}
		}

		if (i == BLOCK_N) {
			perror("No Space to Store Link\n");
			exit(1);
		}


		memcpy((disk + free_block_num * EXT2_BLOCK_SIZE), new_entry, sizeof(new_entry));
		inode->i_links_count++;
		gd->bg_free_blocks_count--;
		sb->s_free_blocks_count--;

 	}
 	
	return 0;
}
