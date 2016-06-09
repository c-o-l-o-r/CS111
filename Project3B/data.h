/***********************************************************************************
DATA STRUCTURES
***********************************************************************************/
typedef struct s_Super
{
	int inodes_count;
	int blocks_count;
	int block_size;
	int inodes_per_group;
} Super;
extern Super super;

typedef struct s_Group
{
	int inode_bitmap;
	int block_bitmap;
} Group;
extern Group* groups;
extern int groups_count;

typedef struct s_Bitmap
{
	int map_location;
	int number;
	int type;			// 0 means it's an inode, 1 means it's a block
} Bitmap;
extern Bitmap* bitmaps;
extern int bitmaps_count;

typedef struct s_Inode
{
	int number;
	int link_count;
	int blocks_count;
	int block_pointers[15];
} Inode;
extern Inode* inodes;
extern int inodes_count;

typedef struct s_Directory
{
	int parent_inode;
	int entry;
	int name_length;
	int inode;
	char* name;
} Directory;
extern Directory* directories;
extern int directories_count;

typedef struct s_Indirect
{
	int block_number;
	int entry;
	int value;
} Indirect;
extern Indirect* indirects;
extern int indirects_count;

void data_structure_init();
