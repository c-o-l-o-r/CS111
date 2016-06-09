#include "data.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

Super super;
Group* groups;
int groups_count=0;
Bitmap* bitmaps;
int bitmaps_count=0;
Inode* inodes;
int inodes_count=0;
Directory* directories;
int directories_count=0;
Indirect* indirects;
int indirects_count=0;

/***********************************************************************************
HELPER FUNCTIONS
***********************************************************************************/
char** line_get_tokens(const char* line)
{
	char* str = strdup(line);
	
	const int token_count_max = 100;
	int token_count=0;
	char** tokens = malloc(sizeof(char*)*token_count_max);
	for (int i=0;i<token_count_max;i++) tokens[i] = NULL;
	
	char* token;
	token = strtok(str, ",\n");
	while (token != NULL)
	{
		tokens[token_count]=strdup(token);
		token_count++;
		token=strtok(NULL,",\n");
	}
	tokens[token_count]=NULL;
	tokens[token_count+1]=NULL;
	
	return tokens;
}

char* line_get_field(const char* line, int fieldnumber)
{
	char** tokens = line_get_tokens(line);
	return tokens[fieldnumber];
}

int bitmap_block_or_inode(const Bitmap b)
{
	for (int i=0;i<groups_count;i++)
	{
		Group g = groups[i];
		if (g.inode_bitmap==b.map_location) return 0;
		if (g.block_bitmap==b.map_location) return 1;
	}
	
	// Error: Map location doesn't exist
	return -1;
}

/***********************************************************************************
DATA STRUCTURES FUNCTIONS
***********************************************************************************/
void data_structure_init()
{
	FILE* fp;
	char buf[10000];
	int capacity;
	
	// Super
	fp = fopen("super.csv", "r");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	if (fgets(buf,10000,fp))
	{
		char** tokens = line_get_tokens(buf);
		super.inodes_count = atoi(tokens[1]);
		super.blocks_count = atoi(tokens[2]);
		super.block_size = atoi(tokens[3]);
		super.inodes_per_group = atoi(tokens[6]);
	}
	fclose(fp);
	
	
	// Group
	fp = fopen("group.csv", "r");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	capacity = 8;
	groups = malloc(sizeof(Group)*capacity);
	if (groups==NULL)
	{
		fprintf(stderr,"Error: malloc failed");
		exit(1);
	}
	
	while (fgets(buf,10000,fp))
	{
		// Array about to overflow, resize
		if (groups_count>capacity-3)
		{
			capacity *= 2;
			groups = realloc(groups,sizeof(Group)*capacity);
			if (groups==NULL)
			{
				fprintf(stderr,"Error: realloc failed");
				exit(1);
			}
		}
		
		char** tokens = line_get_tokens(buf);
		groups[groups_count].inode_bitmap = strtol(tokens[4],NULL,16);
		groups[groups_count].block_bitmap = strtol(tokens[5],NULL,16);
		
		groups_count++;
	}
	fclose(fp);
	
	
	// Bitmap
	fp = fopen("bitmap.csv", "r");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	capacity = 8;
	bitmaps = malloc(sizeof(Bitmap)*capacity);
	if (bitmaps==NULL)
	{
		fprintf(stderr,"Error: malloc failed");
		exit(1);
	}
	
	while (fgets(buf,10000,fp))
	{
		// Array about to overflow, resize
		if (bitmaps_count>capacity-3)
		{
			capacity *= 2;
			bitmaps = realloc(bitmaps,sizeof(Bitmap)*capacity);
			if (bitmaps==NULL)
			{
				fprintf(stderr,"Error: realloc failed");
				exit(1);
			}
		}
		
		char** tokens = line_get_tokens(buf);
		bitmaps[bitmaps_count].map_location = strtol(tokens[0],NULL,16);
		bitmaps[bitmaps_count].number = atoi(tokens[1]);
		bitmaps[bitmaps_count].type = bitmap_block_or_inode(bitmaps[bitmaps_count]);
				
		bitmaps_count++;
	}
	fclose(fp);
	
	
	// Inode
	fp = fopen("inode.csv", "r");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	capacity = 8;
	inodes = malloc(sizeof(Inode)*capacity);
	if (inodes==NULL)
	{
		fprintf(stderr,"Error: malloc failed");
		exit(1);
	}
	
	while (fgets(buf,10000,fp))
	{
		// Array about to overflow, resize
		if (inodes_count>capacity-3)
		{
			capacity *= 2;
			inodes = realloc(inodes,sizeof(Inode)*capacity);
			if (inodes==NULL)
			{
				fprintf(stderr,"Error: realloc failed");
				exit(1);
			}
		}
		
		char** tokens = line_get_tokens(buf);
		inodes[inodes_count].number = atoi(tokens[0]);
		inodes[inodes_count].link_count = atoi(tokens[5]);
		inodes[inodes_count].blocks_count = atoi(tokens[10]);
		for (int i=0;i<15;i++) inodes[inodes_count].block_pointers[i] = strtol(tokens[11+i],NULL,16);
		
		inodes_count++;
	}
	fclose(fp);
	
	
	// Directory
	fp = fopen("directory.csv", "r");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	capacity = 8;
	directories = malloc(sizeof(Directory)*capacity);
	if (directories==NULL)
	{
		fprintf(stderr,"Error: malloc failed");
		exit(1);
	}
	
	int token_index = 0;
	int iter = 0;
	int quote_flag = 0;
	int break_flag = 0;
	while (1)
	{
		if (break_flag) break;
		
		// Array about to overflow, resize
		if (directories_count>capacity-3)
		{
			capacity *= 2;
			directories = realloc(directories,sizeof(Directory)*capacity);
			if (directories==NULL)
			{
				fprintf(stderr,"Error: realloc failed");
				exit(1);
			}
		}
		
		// Note: We cannot use strtok to read directory info, since the file name might contain '\n', so we have to read char by char
		char c;
		c = fgetc(fp);
		if (c<=0) break;
			
		switch (token_index)
		{
		case 0:
			buf[iter]=c;
			if (buf[iter]==',')
			{
				buf[iter]='\0';
				directories[directories_count].parent_inode = atoi(buf);
				token_index++;
				iter=0;
				continue;
			}
			iter++;
			break;
		case 1:
			buf[iter]=c;
			if (buf[iter]==',')
			{
				buf[iter]='\0';
				directories[directories_count].entry = atoi(buf);
				token_index++;
				iter=0;
				continue;
			}
			iter++;
			break;
		case 2:
			if (c==',')
			{
				token_index++;
				continue;
			}
			break;
		case 3:
			buf[iter]=c;
			if (buf[iter]==',')
			{
				buf[iter]='\0';
				directories[directories_count].name_length = atoi(buf);
				token_index++;
				iter=0;
				continue;
			}
			iter++;
			break;
		case 4:
			buf[iter]=c;
			if (buf[iter]==',')
			{
				buf[iter]='\0';
				directories[directories_count].inode = atoi(buf);
				token_index++;
				iter=0;
				continue;
			}
			iter++;
			break;
		case 5:
			buf[iter]=c;
			if (buf[iter]=='\"')
			{
				// If quote_flag is not set, meaning it's open quote, else it's close quote
				if (!quote_flag) 
				{
					quote_flag = 1;
					continue;
				}
				else
				{
					buf[iter]='\0';	
					directories[directories_count].name = strdup(buf);
					directories_count++;
					
					token_index = 0;
					iter = 0;
					quote_flag = 0;
					
					// Read the final \n
					fgetc(fp);
					continue;
				}
			}
			iter++;
			break;
		}
	}
	fclose(fp);
	
	
	// Indirect
	fp = fopen("indirect.csv", "r");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	capacity = 8;
	indirects = malloc(sizeof(Indirect)*capacity);
	if (indirects==NULL)
	{
		fprintf(stderr,"Error: malloc failed");
		exit(1);
	}
	
	while (fgets(buf,10000,fp))
	{
		// Array about to overflow, resize
		if (indirects_count>capacity-3)
		{
			capacity *= 2;
			indirects = realloc(indirects,sizeof(Indirect)*capacity);
			if (indirects==NULL)
			{
				fprintf(stderr,"Error: realloc failed");
				exit(1);
			}
		}
		
		char** tokens = line_get_tokens(buf);
		indirects[indirects_count].block_number = strtol(tokens[0],NULL,16);
		indirects[indirects_count].entry = atoi(tokens[1]);
		indirects[indirects_count].value = strtol(tokens[2],NULL,16);
		
		indirects_count++;
	}
	fclose(fp);
}

