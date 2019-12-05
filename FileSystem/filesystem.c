#include "filesystem.h"

void fs_init()
{
	sb = NULL;

	fdIndex = 0;
	disk = 0;
	// set all fd to -1 to show unused
	for (unsigned int i = 0; i < MAX_FILE_DESCRIPTORS; i++)
		fd[i] = -1;

}

void create_root()
{
	DirectoryEntry testDir;
	testDir.filename = "/";
	testDir.size = sizeof(testDir);
	testDir.type = FS_DIR;
	testDir.startCluster = 0;

	// set the create time
	set_meta_time(&testDir, 0);
	printf("root size is: %ld\n", sizeof(testDir));

	// add entry to FAT
	// used two entries in FAT
	fTable[0] = 1;
	fTable[1] = -1;

	memcpy(&dirTable[0], &testDir, sizeof(DirectoryEntry));
}

int make_fs(char *disk_name)
{
    if (make_disk(disk_name) < 0)
    {
        printf("Error creating disk: %s\n", disk_name);
        return 1;
    }

    if (DEBUG)
        printf("Created disk: %s\n", disk_name);
    
    // open the disk and write the superblock
    if (open_disk(disk_name) < 0)
    {
        printf("Error opening disk: %s\n", disk_name);
        return 1;
    }

    if (DEBUG)
        printf("Opened disk: %s\n", disk_name);
    
	disk = open(disk_name, O_RDWR);
	if (disk < 0)
	{
		printf("Failed to open disk: %s\n", disk_name);
		return -1;
	}

	if ((sb = (SuperBlock*)mmap(NULL, sizeof(SuperBlock), PROT_READ | PROT_WRITE, MAP_SHARED, disk, 0)) == MAP_FAILED)
	{
		printf("SuperBlock map failed\n");
	}
	sb->diskName = (char*)malloc(14);
	strcpy(sb->diskName, disk_name);
	sb->validFormat = VALID_FORMAT;
	sb->fatSize = BLOCK_SIZE;
	sb->fatOffset = 4096;
	sb->dirSize = sizeof(DirectoryEntry) * MAX_AMOUNT_OF_FILES;
	sb->dirOffset = 8192;
	
	if ((fTable = (char*)mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, disk, 4096)) == MAP_FAILED)
	{
		printf("ftable map failed\n");
	}

	if ((dirTable = (char*)mmap(NULL, sizeof(DirectoryEntry) * MAX_AMOUNT_OF_FILES, PROT_READ | PROT_WRITE, MAP_SHARED, disk, 8192)) == MAP_FAILED)
	{
		printf("dir map failed\n");
	}
	
	create_root();

	sb->availableFiles = MAX_AMOUNT_OF_FILES - 1;
	sb->dirCurrentSize = 1;

	block_write(0, (char*)sb);
	block_write(1, fTable);
	block_write(2, dirTable);
;
	close(disk);
    return 0;
}

int mount_fs(char *disk_name)
{
	disk = open(disk_name, O_RDWR);
	if (disk < 0)
	{
		printf("Failed to open disk: %s\n", disk_name);
		return -1;
	}
	
	// try to get SuperBlock from disk
	sb = (SuperBlock*)mmap(0, sizeof(SuperBlock), PROT_READ | PROT_WRITE, MAP_SHARED, disk, 0);
	fTable = mmap(NULL, sb->fatSize, PROT_READ | PROT_WRITE, MAP_SHARED, disk, sb->fatOffset);
	if ((dirTable = (char*)mmap(NULL, sb->dirSize, PROT_READ | PROT_WRITE, MAP_SHARED, disk, sb->dirOffset)) == MAP_FAILED)
	{
		printf("dir map failed\n");
	}

	if (sb == NULL)
	{
		printf("Failed to load SuperBlock.\n");
		return -1;
	}
	if (sb->validFormat != VALID_FORMAT)
	{
		printf("Disk format is invalid. %d\n", sb->validFormat);
		return -1;
	}

	
	printf("fat: %d\n", fTable[0]);

	// test print dir
	DirectoryEntry test2;
	memcpy(&test2, &dirTable[0], sizeof(DirectoryEntry));
	printf("Fat free entry: %d\n", nextFreeFATIndex());
	//printf("dir create date: %s\n", test2.createdDate);
	//printf("dir create time: %s\n", test2.createdTime);
	// if the program made it this far then the disk was mounted successfully
	return 0;
}

// returns a file descriptor or -1 if an erorr occured
int fs_open(char* name)
{
	int foundIndex = 0;

	if (fdIndex >= MAX_FILE_DESCRIPTORS)
	{
		// check for previous space that was freed
		for (unsigned int i = 0; i < MAX_FILE_DESCRIPTORS; i++)
		{
			// found a unused spot
			if (fd[i] == -1)
			{
				fdIndex = i;
				foundIndex = 1;
				break;
			}
		}
	}

	// loop through directory to find the file if it exists
	if (foundIndex || fdIndex < MAX_FILE_DESCRIPTORS)
	{
		char* nameCpy = strdup(name);
		char* ptr = strtok(nameCpy, "/");
		char* nameArr[strlen(name)];
		int len = 0; // how many directories deep the file is to be created

		while (ptr != NULL)
		{
			nameArr[len++] = ptr;
			ptr = strtok(NULL, "/");
		}

		// get directory table and look for file or directory
		DirectoryEntry tempDir;
		memcpy(&tempDir, &dirTable[0], sizeof(DirectoryEntry));
		int i = 0;
		int location = -1;

		while (i < len)
		{
			if (strcmp(tempDir.filename, nameArr[i]))
			{
				// found a match for file or directory
				location = tempDir.startCluster;
				i++;
			}
		}
		return location;
	}

}

int fs_close(int fildes)
{
	// make sure number is in bounds
	if (fildes < MAX_FILE_DESCRIPTORS && fildes >= 0)
	{
		// check to see if a file is currently opened
		if (fd[fildes] != -1)
		{
			close(fd[fildes]);
			fd[fildes] = -1;
			return 0;
		}
	}
	// file is currently not opened or file descriptor doesn't exist
	return -1;
}

int fs_create(char* name)
{
	char* nameCpy = strdup(name);
	char* ptr = strtok(nameCpy, "/");
	char *nameArr[strlen(name)];
	int len = 0; // how many directories deep the file is to be created

	while (ptr != NULL)
	{
		nameArr[len++] = ptr;
		ptr = strtok(NULL, "/");
	}

	// check if directory or file exists
	DirectoryEntry tempDir;
	
	int i = 0;
	while (i < sb->dirCurrentSize)
	{
		if (i >= len)
			return -1;

		memcpy(&tempDir, &dirTable[i], sizeof(DirectoryEntry));

	}
	
}

int fs_delete(char* name)
{}

int fs_mkdir(char* name)
{}

int fs_write(int fildes, void* buf, size_t nbyte)
{}

int fs_get_filesize(int fildes)
{}

int fs_lseek(int fildes, off_t offset)
{}

int fs_truncate(int fildes, off_t length)
{}




















void clearFAT()
{
	for (unsigned int i = 0; i < DISK_BLOCKS; i++)
		fTable[i] = -1;
}

void set_meta_time(DirectoryEntry* dir, int flag)
{
	// set the create time
	time_t t = time(NULL);
	struct tm ct = *localtime(&t);

	// set both create and modify values
	if (flag == 0)
	{
		sprintf(dir->createdDate, "%d/%d/%d", (ct.tm_mon + 1), ct.tm_mday, (1900 + ct.tm_year));
		sprintf(dir->modifiedDate, "%d/%d/%d", ct.tm_mon, ct.tm_mday, ct.tm_year);
		sprintf(dir->createdTime, "%d:%d", ct.tm_hour, ct.tm_min);
		sprintf(dir->modifiedTime, "%d:%d", ct.tm_hour, ct.tm_min);
	}

	// set only modify values
	if (flag == 1)
	{
		sprintf(dir->modifiedDate, "%d/%d/%d", ct.tm_mon, ct.tm_mday, ct.tm_year);
		sprintf(dir->modifiedTime, "%d:%d", ct.tm_hour, ct.tm_min);
	}
}

int nextFreeFATIndex(void)
{
	for (int i = 0; i < sb->fatSize; i++)
	{
		if (fTable[i] == 0)
			return i;
	}
	return -1;
}