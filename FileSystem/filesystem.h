#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <time.h>

#include "disk.h"


#define DEBUG 1
#define MAX_FILE_NAME_SIZE 15
#define MAX_AMOUNT_OF_FILES 256
#define MAX_FILE_DESCRIPTORS 64
#define VALID_FORMAT 33 // is used to check against to see if the filesystem is potentially corrupt
#define CLUSTER_SIZE 512


typedef enum 
{
	FS_FILE,
	FS_DIR
} entry_type;


typedef struct
{
    unsigned char filename[MAX_FILE_NAME_SIZE];
    unsigned char fileExtension[3];
    unsigned int nextPtr; // points to the next file on the FAT; -1 if it is the end of the file
} FatEntry;

typedef struct
{
	char* filename;
	char ext[3];
	char createdTime[10];
	char createdDate[15];
	char modifiedTime[10];
	char modifiedDate[15];
	unsigned int startCluster; // points to the FAT if a file of the dir if is a directory
	unsigned int size;
	entry_type type;
} DirectoryEntry;

typedef struct
{
	char* diskName;
	unsigned int validFormat;
	unsigned int fatSize;
	unsigned int fatOffset;
	unsigned int dirSize;
	unsigned int dirCurrentSize;
	unsigned int dirOffset;
	unsigned int dataSize;
	unsigned int availableFiles;
} SuperBlock;

// global variables
int disk; // holds the memory location of the drive
SuperBlock* sb;

char* fTable;
char* dirTable;
char* dataTable;

// all descriptors are initialized to 0 to indicate that they are free
int fd[MAX_FILE_DESCRIPTORS];
unsigned int fdIndex; // keeps track of the next available file descriptor




void fs_init(void);

// creates the root directory
void create_root(void);

// Creates a new and empty file system on the virtual disk
int make_fs(char *disk_name);

// mounts a file system that is stored on a virtual disk
int mount_fs(char *disk_name);

// returns a file descriptor to the opened file or -1 if an error occured
int fs_open(char* name);

int fs_close(int fildes);

int fs_create(char* name);

int fs_delete(char* name);

int fs_mkdir(char* name);

int fs_write(int fildes, void* buf, size_t nbyte);

int fs_get_filesize(int fildes);

int fs_lseek(int fildes, off_t offset);

int fs_truncate(int fildes, off_t length);

void clearFAT(void);

// use flag 0 to set both the create and modify values or 1 to only set the modily values
void set_meta_time(DirectoryEntry* dir, int flag);

// returns the indexof the next free FAT. Returns -1 if full
int nextFreeFATIndex(void);

#endif