#define DISCSIZE 65536
#define CLUSTERSIZE 2000
#define ROOTSIZE 512

#define NAMESIZE 256
#define EXTSIZE 4


#define FILE_TYPE 0
#define DIRECTORY_TYPE 1

// FAT structure
// it has DISCSIZE blocks
typedef struct FAT {
	int block[DISCSIZE];
}FAT;

// ROOT structure
// type can be 0 (for directory) or 1 (for file)
// name of the data can have NAMESIZE length
// ext of the data can have EXTSIZE length
// timeModified is given in raw seconds, convertible to a date
// actual size of the data
// a char for the attribute
// an integer showing the first position of it on the HDD
typedef struct ROOT {
	unsigned short type;
	char name[NAMESIZE];
	char ext[EXTSIZE];
	long int timeModified;
	int size;
	char attr;
	int firstBlock;
}ROOT;

// CLUSTER structure
// it has CLUSTERSIZE data, made of char
typedef struct CLUSTER {
	char data[CLUSTERSIZE];
}CLUSTER;


// GLOBAL VARIABLES
FAT fat;
ROOT root[ROOTSIZE];
CLUSTER cluster;
// GLOBAL VARIABLES