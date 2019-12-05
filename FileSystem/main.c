// create a structure for a FAT filesystem
/*
*   boot sector
*   FAT #1
*   FAT #2 (duplicate ofFAT 1 maybe)
*   root directory
*   the data area
*/

// max file size is 32mb

#include <stdlib.h>

#include "filesystem.h"



int main(void)
{
	fs_init();
    //if (make_fs("testing") < 0)
    //{
    //    // an error occured
    //    exit(1);
    //}
	
	make_fs("testing");
	mount_fs("testing");

    return 0;
}