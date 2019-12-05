#include "filesystem.h"

int main()
{
	// create a virtual disk and filesystem
	// mount virtual disk
	// copy a file from real OS
	// copy a file from virtual to real OS
	// show contents of virtual disk
	char* diskName = "filesys";
	
	if (make_fs(diskName) < 0)
		return -1;
	
	if (mount_fs(diskName) < 0)
		return -1;

	

	return 0;
}