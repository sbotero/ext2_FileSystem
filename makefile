all: ls cp mkdir ln rm

ls: ext2.h ext2_ls.c
	gcc -Wall ext2_ls.c -o ext2_ls
cp: ext2.h ext2_cp.c
	gcc -Wall ext2_cp.c -lm -o ext2_cp
mkdir: ext2.h ext2_mkdir.c
	gcc -Wall ext2_mkdir.c -o ext2_mkdir
ln: ext2.h ext2_ln.c
	gcc -Wall ext2_ln.c -o ext2_ln
rm: ext2.h ext2_rm.c
	gcc -Wall ext2_rm.c -o ext2_rm
