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

int main(int argc, char **argv){
	char *disk_name = argv[1];
	char *path_name = argv[2];

	int fd = open(disk_name, O_RDWR);
	disk = mmap(NULL, 128 * EXT2_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(disk == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	close(fd);
	sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
	gd = (struct ext2_group_desc *)(disk + 2 * 1024);
	
	//remove the last part of path since that part doesnt exist yet
	char *token = strtok(path_name, "/");
	char *new_dir = NULL;
	if (!token){
		perror("root");
	}
	while (token){
		new_dir = token;
		token = strtok(NULL, "/");	
	}
	int length = strlen(new_dir) + 1;
	char path[strlen(path_name) + 1];
	memcpy(path, path_name, strlen(path_name));
	if (path_name[strlen(path_name)] == '/'){
		length++;	
	}
	strncpy(path, path, strlen(path_name) - length);
	path[strlen(path_name) - length + 1] = '\0';
	struct ext2_dir_entry_2 *dir = path_traversal(path);
	if (!dir){
		return ENOENT;
	}
	if (path_traversal(path_name)){
		return EEXIST;
	}

	create_entry(dir, new_dir);
 
	return 0;
}
