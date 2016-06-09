#include "data.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

FILE* output;

/***********************************************************************************
HELPER FUNCTION
***********************************************************************************/
typedef struct s_AError
{
	int block;
	int error_string_capacity;
	char* error_string;
} AError;
AError* a_errors;
int a_errors_count;
int a_errors_capacity;
char* a_word1;
char* a_word2;
char* a_word3;

void a_errors_init(const char* word1,const char* word2,const char* word3)
{
	a_errors_count = 0;
	a_errors_capacity = 4;
	a_errors = malloc(sizeof(AError)*a_errors_capacity);
	if (a_errors==NULL)
	{
		fprintf(stderr,"Error: malloc failed");
		exit(1);
	}
	
	a_word1 = strdup(word1);
	a_word2 = strdup(word2);
	a_word3 = strdup(word3);
}
void a_errors_add(int block, int inode, int is_indirect, int indirect_block, int entry)
{
	// Array is about to overflow, resize
	if (a_errors_count>a_errors_capacity-3)
	{
		a_errors_capacity *= 2;
		a_errors = realloc(a_errors,sizeof(AError)*a_errors_capacity);
		if (a_errors==NULL)
		{
			fprintf(stderr,"Error: realloc failed");
			exit(1);
		}
	}
	
	for (int i=0;i<a_errors_count;i++)
	{
		// This block has appear in error list, append to error string
		if (a_errors[i].block == block)
		{
			// Check overflow of error string
			if (strlen(a_errors[i].error_string)>a_errors[i].error_string_capacity-100)
			{
				a_errors[i].error_string_capacity *= 2;
				a_errors[i].error_string = realloc(a_errors[i].error_string,sizeof(char)*a_errors[i].error_string_capacity);
				if (a_errors[i].error_string==NULL)
				{
					fprintf(stderr,"Error: realloc failed");
					exit(1);
				}
			}
			
			if (is_indirect)
			{
				sprintf(a_errors[i].error_string+strlen(a_errors[i].error_string)," %s < %d > INDIRECT BLOCK < %d > ENTRY < %d >",a_word3,inode,indirect_block,entry);
			}
			else
			{
				sprintf(a_errors[i].error_string+strlen(a_errors[i].error_string)," %s < %d > ENTRY < %d >",a_word3,inode,entry);
			}
			
			return;
		}
	}
	
	// Loop ends, meaning that this block has never appear in error list before, create one.
	
	a_errors[a_errors_count].block = block;
	a_errors[a_errors_count].error_string_capacity = 101;
	a_errors[a_errors_count].error_string = malloc(sizeof(char)*a_errors[a_errors_count].error_string_capacity);
	if (a_errors[a_errors_count].error_string==NULL)
	{
		fprintf(stderr,"Error: malloc failed");
		exit(1);
	}
	
	sprintf(a_errors[a_errors_count].error_string,"%s < %d > %s",a_word1,block,a_word2);
	if (is_indirect)
	{
		sprintf(a_errors[a_errors_count].error_string+strlen(a_errors[a_errors_count].error_string)," %s < %d > INDIRECT BLOCK < %d > ENTRY < %d >",a_word3,inode,indirect_block,entry);
	}
	else
	{
		sprintf(a_errors[a_errors_count].error_string+strlen(a_errors[a_errors_count].error_string)," %s < %d > ENTRY < %d >",a_word3,inode,entry);
	}
	
	a_errors_count++;
	return;
}

void a_errors_print()
{
	for (int i=0;i<a_errors_count;i++)
	{
		fprintf(output,"%s\n",a_errors[i].error_string);
	}
}

void a_errors_clean()
{
	for (int i=0;i<a_errors_count;i++)
	{
		free(a_errors[i].error_string);
	}
	free(a_errors);
}

/***********************************************************************************
UNALLOCATED BLOCK
***********************************************************************************/
void unallocated_block()
{
	a_errors_init("UNALLOCATED BLOCK","REFERENCED BY","INODE");
	// Iterate all inodes
	for (int i=0;i<inodes_count;i++)
	{
		// Direct pointers
		for (int j=0;j<=11;j++)
		{
			if (inodes[i].block_pointers[j]!=0)
			{
				// If this block pointer is in free list, output error report
				for (int k=0;k<bitmaps_count;k++)
				{
					if ((bitmaps[k].type==1)&&(inodes[i].block_pointers[j]==bitmaps[k].number))
					{
						a_errors_add(bitmaps[k].number, inodes[i].number, 0, 0, j);
					}
				}
			}
		}
		
		// Indirect pointer
		if (inodes[i].block_pointers[12]!=0)
		{
			Indirect p1;
			for (int i1=0;i1<=indirects_count;i1++)
			{
				if (indirects[i1].block_number==inodes[i].block_pointers[12])
				{
					p1 = indirects[i1];
					// If this block pointer is in free list, output error report
					for (int k=0;k<bitmaps_count;k++)
					{
						if ((bitmaps[k].type==1)&&(p1.value==bitmaps[k].number))
						{
							a_errors_add(bitmaps[k].number, inodes[i].number, 1, p1.block_number, p1.entry);
						}
					}
				}
			}
		}
		
		// Double Indirect pointer
		if (inodes[i].block_pointers[13]!=0)
		{
			Indirect p2;
			for (int i2=0;i2<=indirects_count;i2++)
			{
				if (indirects[i2].block_number==inodes[i].block_pointers[13])
				{
					p2 = indirects[i2];
					Indirect p1;
					for (int i1=0;i1<=indirects_count;i1++)
					{
						if (indirects[i1].block_number==p2.value)
						{
							p1 = indirects[i1];
							// If this block pointer is in free list, output error report
							for (int k=0;k<bitmaps_count;k++)
							{
								if ((bitmaps[k].type==1)&&(p1.value==bitmaps[k].number))
								{
									a_errors_add(bitmaps[k].number, inodes[i].number, 1, p1.block_number, p1.entry);
								}
							}
						}
					}						
				}
			}
		}
		
		// Triple Indirect pointer
		if (inodes[i].block_pointers[14]!=0)
		{
			Indirect p3;
			for (int i3=0;i3<=indirects_count;i3++)
			{
				if (indirects[i3].block_number==inodes[i].block_pointers[14])
				{
					p3 = indirects[i3];
					Indirect p2;
					for (int i2=0;i2<=indirects_count;i2++)
					{
						if (indirects[i2].block_number==inodes[i].block_pointers[13])
						{
							p2 = indirects[i2];
							Indirect p1;
							for (int i1=0;i1<=indirects_count;i1++)
							{
								if (indirects[i1].block_number==p2.value)
								{
									p1 = indirects[i1];
									// If this block pointer is in free list, output error report
									for (int k=0;k<bitmaps_count;k++)
									{
										if ((bitmaps[k].type==1)&&(p1.value==bitmaps[k].number))
										{
											a_errors_add(bitmaps[k].number, inodes[i].number, 1, p1.block_number, p1.entry);
										}
									}
								}
							}						
						}
					}								
				}
			}
		}
	}
	
	a_errors_print();
	a_errors_clean();
}

/***********************************************************************************
DUPLICATELY ALLOCATED BLOCK
***********************************************************************************/
// format: block_inode_count[block] = count, with count is the number of inodes reference it
int* block_inode_count;
void block_inode_count_init()
{
	int size = super.blocks_count+1;
	block_inode_count = malloc(sizeof(int)*size);
	for (int i=0;i<size;i++)
	{
		block_inode_count[i] = 0;
	}
}
void block_inode_count_clean()
{
	free(block_inode_count);
}

void duplicately_allocated_block()
{
	a_errors_init("MULTIPLY REFERENCED BLOCK","BY","INODE");
	block_inode_count_init();
	
	// Iterate all inodes, counting hash
	for (int i=0;i<inodes_count;i++)
	{
		// Direct pointers
		for (int j=0;j<=11;j++)
		{
			if (inodes[i].block_pointers[j]!=0)
			{
				block_inode_count[inodes[i].block_pointers[j]]++;
			}
		}
		
		// Indirect pointer
		if (inodes[i].block_pointers[12]!=0)
		{
			Indirect p1;
			for (int i1=0;i1<=indirects_count;i1++)
			{
				if (indirects[i1].block_number==inodes[i].block_pointers[12])
				{
					p1 = indirects[i1];
					block_inode_count[p1.value]++;
				}
			}
		}
		
		// Double Indirect pointer
		if (inodes[i].block_pointers[13]!=0)
		{
			Indirect p2;
			for (int i2=0;i2<=indirects_count;i2++)
			{
				if (indirects[i2].block_number==inodes[i].block_pointers[13])
				{
					p2 = indirects[i2];
					Indirect p1;
					for (int i1=0;i1<=indirects_count;i1++)
					{
						if (indirects[i1].block_number==p2.value)
						{
							p1 = indirects[i1];
							block_inode_count[p1.value]++;
						}
					}						
				}
			}
		}
		
		// Triple Indirect pointer
		
		if (inodes[i].block_pointers[14]!=0)
		{
			Indirect p3;
			for (int i3=0;i3<=indirects_count;i3++)
			{
				if (indirects[i3].block_number==inodes[i].block_pointers[14])
				{
					p3 = indirects[i3];
					Indirect p2;
					for (int i2=0;i2<=indirects_count;i2++)
					{
						if (indirects[i2].block_number==inodes[i].block_pointers[13])
						{
							p2 = indirects[i2];
							Indirect p1;
							for (int i1=0;i1<=indirects_count;i1++)
							{
								if (indirects[i1].block_number==p2.value)
								{
									p1 = indirects[i1];
									block_inode_count[p1.value]++;
								}
							}						
						}
					}								
				}
			}
		}
	}
	
	// Iterate all inodes, error if counting > 1
	for (int i=0;i<inodes_count;i++)
	{
		// Direct pointers
		for (int j=0;j<=11;j++)
		{
			if (inodes[i].block_pointers[j]!=0)
			{
				if (block_inode_count[inodes[i].block_pointers[j]]>1) a_errors_add(inodes[i].block_pointers[j],inodes[i].number,0,0,j);
			}
		}
		
		// Indirect pointer
		if (inodes[i].block_pointers[12]!=0)
		{
			Indirect p1;
			for (int i1=0;i1<=indirects_count;i1++)
			{
				if (indirects[i1].block_number==inodes[i].block_pointers[12])
				{
					p1 = indirects[i1];
					if (block_inode_count[p1.value]>1) a_errors_add(p1.value,inodes[i].number,1,p1.block_number,p1.entry);
				}
			}
		}
		
		// Double Indirect pointer
		if (inodes[i].block_pointers[13]!=0)
		{
			Indirect p2;
			for (int i2=0;i2<=indirects_count;i2++)
			{
				if (indirects[i2].block_number==inodes[i].block_pointers[13])
				{
					p2 = indirects[i2];
					Indirect p1;
					for (int i1=0;i1<=indirects_count;i1++)
					{
						if (indirects[i1].block_number==p2.value)
						{
							p1 = indirects[i1];
							if (block_inode_count[p1.value]>1) a_errors_add(p1.value,inodes[i].number,1,p1.block_number,p1.entry);
						}
					}						
				}
			}
		}

		// Triple Indirect pointer
		if (inodes[i].block_pointers[14]!=0)
		{
			Indirect p3;
			for (int i3=0;i3<=indirects_count;i3++)
			{
				if (indirects[i3].block_number==inodes[i].block_pointers[14])
				{
					p3 = indirects[i3];
					Indirect p2;
					for (int i2=0;i2<=indirects_count;i2++)
					{
						if (indirects[i2].block_number==inodes[i].block_pointers[13])
						{
							p2 = indirects[i2];
							Indirect p1;
							for (int i1=0;i1<=indirects_count;i1++)
							{
								if (indirects[i1].block_number==p2.value)
								{
									p1 = indirects[i1];
									if (block_inode_count[p1.value]>1) a_errors_add(p1.value,inodes[i].number,1,p1.block_number,p1.entry);
								}
							}						
						}
					}								
				}
			}
		}
	}
	
	a_errors_print();
	a_errors_clean();
	block_inode_count_clean();
}

/***********************************************************************************
UNALLOCATED INODE
***********************************************************************************/
// format: inode_used[inode] = 1 if it's shown up in inode.csv, 0 otherwise
int* inode_used;
void inode_used_init()
{
	int size = super.inodes_count+1;
	inode_used = malloc(sizeof(int)*size);
	for (int i=0;i<size;i++)
	{
		inode_used[i] = 0;
	}
}
void inode_used_clean()
{
	free(inode_used);
}

void unallocated_inode()
{
	a_errors_init("UNALLOCATED INODE","REFERENCED BY","DIRECTORY");
	inode_used_init();
	
	// Iterate all inodes, updating hash
	for (int i=0;i<inodes_count;i++)
	{
		inode_used[inodes[i].number] = 1;
	}
	
	// Iterate all directories, check inode
	for (int i=0;i<directories_count;i++)
	{
		if (!inode_used[directories[i].inode])
		{
			a_errors_add(directories[i].inode,directories[i].parent_inode,0,0,directories[i].entry);
		}
	}
	
	a_errors_print();
	a_errors_clean();
	inode_used_clean();
}

/***********************************************************************************
MISSING INODE
***********************************************************************************/
// format: inode_present[inode] = 1 if it's not missing, 0 otherwise. A missing inode does not show up in
// directory entry AND not show up in free list
int* inode_present;
void inode_present_init()
{
	int size = super.inodes_count+1;
	inode_present = malloc(sizeof(int)*size);
	for (int i=0;i<size;i++)
	{
		inode_present[i] = 0;
	}
	
	// Inode before index 11 are reserved, always present
	for (int i=0;i<11;i++)
	{
		inode_present[i] = 1;
	}
}
void inode_present_clean()
{
	free(inode_present);
}

void missing_inode()
{
	inode_present_init();
	
	// Iterate all directories, mark all inodes showing up as present
	for (int i=0;i<directories_count;i++)
	{
		inode_present[directories[i].inode] = 1;
	}
	
	// Iterate bitmaps, mark all inodes showing up as present
	for (int i=0;i<bitmaps_count;i++)
	{
		if (bitmaps[i].type==0)
		{
			inode_present[bitmaps[i].number] = 1;
		}
	}
	
	// If an inode is not present in both directories and bitmaps, it's missing
	for (int i=0;i<super.inodes_count+1;i++)
	{
		if (!inode_present[i])
		{
			// Find out which free list this inode be in
			int group_index = (i - 1) / super.inodes_per_group;
			int free_list = groups[group_index].inode_bitmap;
			
			fprintf(output,"MISSING INODE < %d > SHOULD BE IN FREE LIST < %d >\n",i,free_list);
		}
	}
	
	inode_present_clean();
}

/***********************************************************************************
INCORRECT LINK COUNT
***********************************************************************************/
// format: inode_link_count[inode] = number of directory entries that point to it
int* inode_link_count;
void inode_link_count_init()
{
	int size = super.inodes_count+1;
	inode_link_count = malloc(sizeof(int)*size);
	for (int i=0;i<size;i++)
	{
		inode_link_count[i] = 0;
	}
}
void inode_link_count_clean()
{
	free(inode_link_count);
}

void incorrect_link_count()
{
	inode_link_count_init();
	
	// Iterate all directories, update count
	for (int i=0;i<directories_count;i++)
	{
		inode_link_count[directories[i].inode]++;
	}
	
	// Iterate inodes, report if link count is incorrect
	for (int i=0;i<inodes_count;i++)
	{
		if (inodes[i].link_count!=inode_link_count[inodes[i].number])
		{
			fprintf(output,"LINKCOUNT < %d > IS < %d > SHOULD BE < %d >\n",inodes[i].number,inodes[i].link_count,inode_link_count[inodes[i].number]);
		}
	}
	
	inode_link_count_clean();
}

/***********************************************************************************
INCORRECT DIRECTORY ENTRY
***********************************************************************************/
void incorrect_directory_entry()
{
	for (int i=0;i<directories_count;i++)
	{
		// Find incorrect "."
		if (strcmp(directories[i].name,".")==0)
		{
			if (directories[i].parent_inode!=directories[i].inode)
			{
				fprintf(output,"INCORRECT ENTRY IN < %d > NAME < . > LINK TO < %d > SHOULD BE < %d >\n",directories[i].parent_inode,directories[i].inode,directories[i].parent_inode);
			}
		}
		
		// Find incorrect ".."
		if (strcmp(directories[i].name,"..")==0)
		{
			int parent = directories[i].inode;
			int true_parent = -1;
			
			// Find where it's "true parent" is
			for (int j=0;j<directories_count;j++)
			{
				if ((strcmp(directories[j].name,".")!=0)&&(strcmp(directories[j].name,"..")!=0)&&(directories[j].inode==directories[i].parent_inode))
				{
					true_parent = directories[j].parent_inode;
				}
			}
			
			if (true_parent==-1)
			{
				// Root directory, in this case ".." should point to itself
				if (directories[i].parent_inode!=directories[i].inode)
				{
					fprintf(output,"INCORRECT ENTRY IN < %d > NAME < .. > LINK TO < %d > SHOULD BE < %d >\n",directories[i].parent_inode,directories[i].inode,directories[i].parent_inode);
				}
			}
			else
			{
				// Report if true parent is diffent than parent
				if (parent!=true_parent)
				{
					fprintf(output,"INCORRECT ENTRY IN < %d > NAME < .. > LINK TO < %d > SHOULD BE < %d >\n",directories[i].parent_inode,parent,true_parent);
				}
			}
		}
	}
}

/***********************************************************************************
INVALID BLOCK POINTER
***********************************************************************************/
void invalid_block_pointer()
{
	for (int i=0;i<inodes_count;i++)
	{
		int perblock = super.block_size/4;
		int bcount = inodes[i].blocks_count;
		int iter = 0;
		
		// Iterate all pointers
		while (iter<bcount)
		{
			if (iter < 12)
			{
				// Direct pointers
				if ((inodes[i].block_pointers[iter]<=0)||(inodes[i].block_pointers[iter]>=super.blocks_count))
				{
					fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > ENTRY < %d >\n",inodes[i].block_pointers[iter],inodes[i].number,iter);
				}
				
				goto ENDLOOP;
			}
			else if (iter < 12 + perblock)
			{
				// Indirect pointers
				int offset1 = iter - 12;
				
				// Check if the indirect pointer itself is invalid
				if ((inodes[i].block_pointers[12]<=0)||(inodes[i].block_pointers[12]>=super.blocks_count))
				{
					fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > ENTRY < %d >\n",12,inodes[i].number,12);
					goto ENDLOOP;
				}
				else
				{
					Indirect p1;
					for (int i1=0;i1<=indirects_count;i1++)
					{
						if ((indirects[i1].block_number==inodes[i].block_pointers[12])&&(indirects[i1].entry==offset1))
						{
							p1 = indirects[i1];
							if ((p1.value<=0)||(p1.value>=super.blocks_count))
							{
								fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > INDIRECT BLOCK < %d > ENTRY < %d >\n",p1.value,inodes[i].number,p1.block_number,p1.entry);
							}
						}
					}
					goto ENDLOOP;
				}
			}
			else if (iter < 12 + perblock + perblock*perblock)
			{
				// Doubly indirect pointers
				int offset2 = (iter - 12 - perblock)/perblock;
				int offset1 = iter - 12 - perblock - offset2*perblock;
				
				// Check if the doubly indirect pointer itself is invalid
				if ((inodes[i].block_pointers[13]<=0)||(inodes[i].block_pointers[13]>=super.blocks_count))
				{
					fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > ENTRY < %d >\n",13,inodes[i].number,13);
					goto ENDLOOP;
				}
				else 
				{
					Indirect p2;
					for (int i2=0;i2<=indirects_count;i2++)
					{
						if ((indirects[i2].block_number==inodes[i].block_pointers[13])&&(indirects[i2].entry==offset2))
						{
							p2 = indirects[i2];
							
							// Check if the indirect pointer itself is invalid
							if ((p2.value<=0)||(p2.value>=super.blocks_count))
							{
								fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > INDIRECT BLOCK < %d > ENTRY < %d >\n",p2.value,inodes[i].number,p2.block_number,p2.entry);
								goto ENDLOOP;
							}
							else
							{
								Indirect p1;
								for (int i1=0;i1<=indirects_count;i1++)
								{
									if ((indirects[i1].block_number==p2.value)&&(indirects[i1].entry==offset1))
									{
										p1 = indirects[i1];
										if ((p1.value<=0)||(p1.value>=super.blocks_count))
										{
											fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > INDIRECT BLOCK < %d > ENTRY < %d >\n",p1.value,inodes[i].number,p1.block_number,p1.entry);
										}
									}
								}
								goto ENDLOOP;
							}
						}
					}
					goto ENDLOOP;
				}
			}
			else
			{
				// Triply indirect pointers
				int offset3 = (iter - 12 - perblock - perblock*perblock)/(perblock*perblock);
				int offset2 = (iter - 12 - perblock - perblock*perblock - offset3*perblock*perblock)/perblock;
				int offset1 = iter - 12 - perblock - perblock*perblock - offset3*perblock*perblock - offset2*perblock;
				
				// Check if the triply indirect pointer itself is invalid
				if ((inodes[i].block_pointers[14]<=0)||(inodes[i].block_pointers[14]>=super.blocks_count))
				{
					fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > ENTRY < %d >\n",14,inodes[i].number,14);
					goto ENDLOOP;
				}
				else 
				{
					Indirect p3;
					for (int i3=0;i3<=indirects_count;i3++)
					{
						if ((indirects[i3].block_number==inodes[i].block_pointers[14])&&(indirects[i3].entry==offset3))
						{
							p3 = indirects[i3];
							
							// Check if the indirect pointer itself is invalid
							if ((p3.value<=0)||(p3.value>=super.blocks_count))
							{
								fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > INDIRECT BLOCK < %d > ENTRY < %d >\n",p3.value,inodes[i].number,p3.block_number,p3.entry);
								goto ENDLOOP;
							}
							else
							{
								Indirect p2;
								for (int i2=0;i2<=indirects_count;i2++)
								{
									if ((indirects[i2].block_number==p3.value)&&(indirects[i2].entry==offset2))
									{
										p2 = indirects[i2];
										
										// Check if the indirect pointer itself is invalid
										if ((p2.value<=0)||(p2.value>=super.blocks_count))
										{
											fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > INDIRECT BLOCK < %d > ENTRY < %d >\n",p2.value,inodes[i].number,p2.block_number,p2.entry);
											goto ENDLOOP;
										}
										else
										{
											Indirect p1;
											for (int i1=0;i1<=indirects_count;i1++)
											{
												if ((indirects[i1].block_number==p2.value)&&(indirects[i1].entry==offset1))
												{
													p1 = indirects[i1];
													if ((p1.value<=0)||(p1.value>=super.blocks_count))
													{
														fprintf(output,"INVALID BLOCK < %d > IN INODE < %d > INDIRECT BLOCK < %d > ENTRY < %d >\n",p1.value,inodes[i].number,p1.block_number,p1.entry);
													}
												}
											}
											goto ENDLOOP;
										}
									}
								}
								goto ENDLOOP;
							}
						}
					}
					goto ENDLOOP;
				}
			}
			ENDLOOP: iter++;
		}
	}
}

/***********************************************************************************
MAIN
***********************************************************************************/
int main(int argc, char **argv)
{
	output = fopen("lab3b_check.txt", "w");
	if (output==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	data_structure_init();
	
	unallocated_block();
	duplicately_allocated_block();
	unallocated_inode();
	missing_inode();
	incorrect_link_count();
	incorrect_directory_entry();
	invalid_block_pointer();
	
	fclose(output);
    exit(0);
}



