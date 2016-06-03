#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int ifd;

// super
int block_size, blocks_count, blocks_per_group, inodes_per_group;

// group
int block_groups_count;
int* block_bitmap;
int* inode_bitmap;
int* inode_table;

// bitmap
int* inode_address;
int inode_address_count;

/***********************************************************************************
HELPER FUNCTIONS
***********************************************************************************/
// Reverse
void reverse(unsigned char* s, int len) 
{
   int c;
   unsigned char *begin, *end, temp;
 
   begin = s;
   end = s;
 
   for (c = 0; c < len - 1; c++)
      end++;
 
   for (c = 0; c < len/2; c++)
   {        
      temp   = *end;
      *end   = *begin;
      *begin = temp;
 
      begin++;
      end--;
   }
}

int int_pow(int base, int exp)
{
    int result = 1;
	for (int i=0;i<exp;i++)
    {
        result *= base;
    }
    return result;
}

// Read buffer from disk image
void get_buffer(unsigned char* buf, int count, int offset)
{
	if (pread(ifd,buf,count,offset)<0)
	{
		fprintf(stderr, "read error\n");
		exit(-1);
	}
}

void get_buffer_reverse(unsigned char* buf, int count, int offset)
{
	if (pread(ifd,buf,count,offset)<0)
	{
		fprintf(stderr, "read error\n");
		exit(-1);
	}
	
	// Convert from little endian to big endian
	reverse(buf, count);
}

// Convert buffer to a decimal number
unsigned int buffer_to_dec(const unsigned char* s, int len)
{
	unsigned int result=0;
	for (int i=0;i<len;i++)
	{
		result += s[i]*int_pow(2,8*(len-1-i));
	}
	return result;
}

// Convert buffer to a string of hex
char* buffer_to_hex(const unsigned char* s, int len)
{
	char* output = malloc(sizeof(char)*(len*2+1));
	if (output == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}

	unsigned int x = buffer_to_dec(s,len);
	sprintf(output, "%x", x);
	
    return output;
}

// Direct block ids. Return the list of direct block ids
unsigned* direct_block_ids(int inode_number, unsigned* pcount)
{
	*pcount = 0;
	unsigned pcap = 4;
	unsigned* p = malloc(sizeof(int)*pcap);
	if (p==NULL)
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	// init
	unsigned char* buf = malloc(sizeof(char)*block_size*2);
	if (buf == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	int group = (inode_number-1) / inodes_per_group;
	int index = (inode_number-1) % inodes_per_group;
	int initial_offset = inode_table[group]*block_size + index*128;
	
	// Get direct block pointers
	for (int i=0;i<12;i++)
	{
		get_buffer_reverse(buf,4,initial_offset+40+i*4);
		unsigned int p0 = buffer_to_dec(buf,4);
		
		// Get into the pointer if it's not NULL
		if (p0!=0)
		{
			if (*pcount>pcap-2)
			{
				pcap*=2;
				p = realloc(p,sizeof(int)*pcap);
				if (p==NULL)
				{
					fprintf(stderr, "malloc failed\n");
					exit(-1);
				}
			}
			p[*pcount]=p0;
			(*pcount)++;
		}
	}
	
	// Get direct block pointers from indirect block pointer
	get_buffer_reverse(buf,4,initial_offset+88);
	unsigned int p1 = buffer_to_dec(buf,4);
	if (p1!=0)
	{
		for (int i=0;i<block_size;i+=4)
		{
			get_buffer_reverse(buf,block_size,p1*block_size+i);
			unsigned int p0 = buffer_to_dec(buf,4);
			
			if (p0!=0)
			{
				if (*pcount>pcap-2)
				{
					pcap*=2;
					p = realloc(p,sizeof(int)*pcap);
					if (p==NULL)
					{
						fprintf(stderr, "malloc failed\n");
						exit(-1);
					}
				}
				p[*pcount]=p0;
				(*pcount)++;;
			}
		}
	}
	
	// Get direct block pointers from doubly block pointer
	get_buffer_reverse(buf,4,initial_offset+92);
	unsigned int p2 = buffer_to_dec(buf,4);
	if (p2!=0)
	{
		for (int j=0;j<block_size;j+=4)
		{
			get_buffer_reverse(buf,block_size,p2*block_size+j);
			unsigned int p1 = buffer_to_dec(buf,4);
			
			if (p1!=0)
			{
				for (int i=0;i<block_size;i+=4)
				{
					get_buffer_reverse(buf,block_size,p1*block_size+i);
					unsigned int p0 = buffer_to_dec(buf,4);
					
					if (p0!=0)
					{
						if (*pcount>pcap-2)
						{
							pcap*=2;
							p = realloc(p,sizeof(int)*pcap);
							if (p==NULL)
							{
								fprintf(stderr, "malloc failed\n");
								exit(-1);
							}
						}
						p[*pcount]=p0;
						(*pcount)++;
					}
				}
			}
		}
	}
	
	// Get direct block pointers from triply block pointer
	get_buffer_reverse(buf,4,initial_offset+92);
	unsigned int p3 = buffer_to_dec(buf,4);
	if (p3!=0)
	{
		for (int k=0;k<block_size;k+=4)
		{
			get_buffer_reverse(buf,block_size,p3*block_size+k);
			unsigned int p2 = buffer_to_dec(buf,4);
			
			if (p2!=0)
			{
				for (int j=0;j<block_size;j+=4)
				{
					get_buffer_reverse(buf,block_size,p2*block_size+j);
					unsigned int p1 = buffer_to_dec(buf,4);
					
					if (p1!=0)
					{
						for (int i=0;i<block_size;i+=4)
						{
							get_buffer_reverse(buf,block_size,p1*block_size+i);
							unsigned int p0 = buffer_to_dec(buf,4);
							
							if (p0!=0)
							{
								if (*pcount>pcap-2)
								{
									pcap*=2;
									p = realloc(p,sizeof(int)*pcap);
									if (p==NULL)
									{
										fprintf(stderr, "malloc failed\n");
										exit(-1);
									}
								}
								p[*pcount]=p0;
								(*pcount)++;
							}
						}
					}
				}
			}
		}
	}
	
	return p;
}

// Read inode's content to buffer. Return the number of bytes read, or return -1 if fail
unsigned read_inode(const int inode_number, unsigned char* buf, const unsigned count, const unsigned offset)
{
	unsigned* p;
	unsigned pcount;
	p = direct_block_ids(inode_number,&pcount);
	
	unsigned p_index = offset / block_size;
	unsigned iter = offset % block_size;
	for (int i=0;i<count;i++)
	{
		unsigned address = p[p_index]*block_size + iter;
		unsigned char c;
		get_buffer(&c,1,address);
		buf[i]=c;
		
		iter++;
		
		// End of data block, go to next one
		if (iter>=block_size)
		{
			p_index++;
			iter=0;
			if (p_index>=pcount)
			{
				return -1;		
			}
		}
	}
	
	return count;
}

unsigned read_inode_reverse(const int inode_number, unsigned char* buf, const unsigned count, const unsigned offset)
{
	int c = read_inode(inode_number, buf, count, offset);
	reverse(buf,count);
}

/***********************************************************************************
GENERATE CSV
***********************************************************************************/
// super.csv
void super()
{
	FILE* fp;
	fp = fopen("super.csv", "w");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	unsigned char* buf = malloc(sizeof(char)*100);
	if (buf == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	const int initial_offset = 1024;
	
	// magic number
	get_buffer_reverse(buf,2,initial_offset+56);
	char* s_magic = buffer_to_hex(buf,2);
	
	// total number of inodes
	get_buffer_reverse(buf,4,initial_offset+0);
	unsigned int s_inodes_count = buffer_to_dec(buf,4);
	
	// total number of blocks
	get_buffer_reverse(buf,4,initial_offset+4);
	unsigned int s_blocks_count = buffer_to_dec(buf,4);
	blocks_count = s_blocks_count;
	
	// block size
	get_buffer_reverse(buf,4,initial_offset+24);
	unsigned int s_log_block_size = buffer_to_dec(buf,4);
	unsigned int s_block_size = 1024 << s_log_block_size;
	block_size = s_block_size;
	
	// fragment size
	get_buffer_reverse(buf,4,initial_offset+28);
	unsigned int s_log_frag_size = buffer_to_dec(buf,4);
	unsigned int s_frag_size;
	if (s_log_frag_size>>31==0)
	{
		s_frag_size = 1024 << s_log_frag_size;
	}
	else
	{
		s_frag_size = 1024 >> (~s_log_frag_size+1);
	}
	
	// blocks per group
	get_buffer_reverse(buf,4,initial_offset+32);
	unsigned int s_blocks_per_group = buffer_to_dec(buf,4);
	blocks_per_group = s_blocks_per_group;
	
	// inodes per group
	get_buffer_reverse(buf,4,initial_offset+40);
	unsigned int s_inodes_per_group = buffer_to_dec(buf,4);
	inodes_per_group = s_inodes_per_group;
	
	// fragments per group
	get_buffer_reverse(buf,4,initial_offset+36);
	unsigned int s_frags_per_group = buffer_to_dec(buf,4);
	
	// first data block
	get_buffer_reverse(buf,4,initial_offset+20);
	unsigned int s_first_data_block = buffer_to_dec(buf,4);
	
	// Print the result to csv 
	fprintf(fp,"%s,%d,%d,%d,%d,%d,%d,%d,%d\n",s_magic,s_inodes_count,s_blocks_count,s_block_size,s_frag_size,s_blocks_per_group,s_inodes_per_group,s_frags_per_group,s_first_data_block);
	
	free(buf);
	fclose(fp);
}

// group.csv
void group()
{
	FILE* fp;
	fp = fopen("group.csv", "w");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	unsigned char* buf = malloc(sizeof(char)*100);
	if (buf == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	// Calculate number of block groups
	block_groups_count = 0;
	int g_blocks_count = blocks_count;
	while (g_blocks_count>0)
	{
		g_blocks_count -= blocks_per_group;
		block_groups_count++;
	}
	
	// 
	block_bitmap = malloc(sizeof(int)*block_groups_count);
	if (block_bitmap == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	inode_bitmap = malloc(sizeof(int)*block_groups_count);
	if (inode_bitmap == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	inode_table = malloc(sizeof(int)*block_groups_count);
	if (inode_table == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	
	// Iterate for each block group
	for (int i=0;i<block_groups_count;i++)
	{
		int initial_offset = 2048+32*i;
		
		// number of contained blocks
		int g_contained_blocks_count;
		if (i==block_groups_count-1)
		{
			if (blocks_count%blocks_per_group == 0)
				g_contained_blocks_count = blocks_per_group;
			else g_contained_blocks_count = blocks_count%blocks_per_group;
		}
		else g_contained_blocks_count = blocks_per_group;
			
		// number of free blocks
		get_buffer_reverse(buf,2,initial_offset+12);
		unsigned int g_free_blocks_count = buffer_to_dec(buf,2);
		
		// number of free inodes
		get_buffer_reverse(buf,2,initial_offset+14);
		unsigned int g_free_inodes_count = buffer_to_dec(buf,2);
		
		// number of directories
		get_buffer_reverse(buf,2,initial_offset+16);
		unsigned int g_used_dirs_count = buffer_to_dec(buf,2);
		
		// (free) inode bitmap block
		get_buffer_reverse(buf,4,initial_offset+4);
		char* g_inode_bitmap = buffer_to_hex(buf,4);
		inode_bitmap[i] = buffer_to_dec(buf,4);
		
		// (free) block bitmap block
		get_buffer_reverse(buf,4,initial_offset+0);
		char* g_block_bitmap = buffer_to_hex(buf,4);
		block_bitmap[i] = buffer_to_dec(buf,4);
		
		// inode table (start) block
		get_buffer_reverse(buf,4,initial_offset+8);
		char* g_inode_table = buffer_to_hex(buf,4);
		inode_table[i] = buffer_to_dec(buf,4);
		
		// Print the result to csv 
		fprintf(fp,"%d,%d,%d,%d,%s,%s,%s\n",g_contained_blocks_count,g_free_blocks_count,g_free_inodes_count,g_used_dirs_count,g_inode_bitmap,g_block_bitmap,g_inode_table);
	}
	
	free(buf);
	fclose(fp);
}

// bitmap.csv
void bitmap()
{
	FILE* fp;
	fp = fopen("bitmap.csv", "w");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	unsigned char* buf = malloc(sizeof(char)*block_size*2);
	if (buf == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	inode_address = malloc(sizeof(int)*inodes_per_group*block_groups_count);
	if (inode_address == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	inode_address_count=0;
	
	// Iterate for each block group
	for (int i=0;i<block_groups_count;i++)
	{
		int initial_offset;
		int data_blockoffset,data_inodeoffset;
		unsigned char* block;
		
		// Find free blocks
		initial_offset = block_bitmap[i]*block_size;
		data_blockoffset=i*blocks_per_group+1;
		
		get_buffer(buf,block_size,initial_offset);
		for (int j=0;j<block_size;j++)
		{
			unsigned int byte=buf[j];
			unsigned int bit;
			for (int k=0;k<8;k++)
			{
				bit = byte & 1;
				if (!bit)
				{
					// Print the result to csv 
					fprintf(fp,"%x,%d\n",block_bitmap[i],data_blockoffset+8*j+k);
				}
				byte = byte >> 1;
			}
		}
		
		// Find free inodes
		initial_offset = inode_bitmap[i]*block_size;
		data_inodeoffset=i*inodes_per_group+1;
		
		get_buffer(buf,block_size,initial_offset);
		for (int j=0;j<block_size;j++)
		{
			unsigned int byte=buf[j];
			unsigned int bit;
			for (int k=0;k<8;k++)
			{
				bit = byte & 1;
				int inode_number = data_inodeoffset+8*j+k;
				if (!bit)
				{
					// Print the result to csv 
					fprintf(fp,"%x,%d\n",inode_bitmap[i],inode_number);
				}
				else if (inode_number< data_inodeoffset + inodes_per_group)
				{
					// This inode are taken, add it to the address list
					inode_address[inode_address_count] = inode_number;
					inode_address_count++;
				}
				byte = byte >> 1;
			}
		}

	}
	
	free(buf);
	fclose(fp);
}

// inode.csv
void inode()
{
	FILE* fp;
	fp = fopen("inode.csv", "w");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	unsigned char* buf = malloc(sizeof(char)*100);
	if (buf == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	for (int i=0;i<inode_address_count;i++)
	{
		// inode number
		int inode_number = inode_address[i];
		
		// Find the table entry
		int group = (inode_number-1) / inodes_per_group;
		int index = (inode_number-1) % inodes_per_group;
		int initial_offset = inode_table[group]*block_size + index*128;

		// file type
		get_buffer_reverse(buf,2,initial_offset+0);
		unsigned int i_mode = buffer_to_dec(buf,2);
		unsigned int t = i_mode >> 12;
		char i_type;
		if (t==8) i_type = 'f';
		else if (t==4) i_type = 'd';
		else if (t==10) i_type = 's';
		else i_type = '?';
		
		// owner
		get_buffer_reverse(buf,2,initial_offset+2);
		unsigned int i_uid = buffer_to_dec(buf,2);
		
		// group
		get_buffer_reverse(buf,24,initial_offset+2);
		unsigned int i_gid = buffer_to_dec(buf,2);
		
		// link count
		get_buffer_reverse(buf,26,initial_offset+2);
		unsigned int i_links_count = buffer_to_dec(buf,2);
		
		// creation time
		get_buffer_reverse(buf,12,initial_offset+4);
		char* i_ctime = buffer_to_hex(buf,4);
		
		// modification time
		get_buffer_reverse(buf,16,initial_offset+4);
		char* i_mtime = buffer_to_hex(buf,4);
		
		// access time 
		get_buffer_reverse(buf,8,initial_offset+4);
		char* i_atime = buffer_to_hex(buf,4);
		
		// file size
		get_buffer_reverse(buf,4,initial_offset+4);
		unsigned int i_size = buffer_to_dec(buf,4);
		
		// number of blocks
		get_buffer_reverse(buf,4,initial_offset+28);
		unsigned int i_blocks  = buffer_to_dec(buf,4);
		unsigned int i_blocks_count = i_blocks / (block_size / 512);
		if (i_blocks % (block_size / 512) != 0) i_blocks_count++;
		
		// Print the result to csv 
		fprintf(fp,"%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d,",inode_number,i_type,i_mode,i_uid,i_gid,i_links_count,i_ctime,i_mtime,i_atime,i_size,i_blocks_count);
		
		// block pointers
		for (int j=0;j<15;j++)
		{
			get_buffer_reverse(buf,4,initial_offset+40+j*4);
			char* i_block_pointer = buffer_to_hex(buf,4);
			if (j==14) fprintf(fp,"%s\n",i_block_pointer);
			else fprintf(fp,"%s,",i_block_pointer);
		}
	}
	
	free(buf);
	fclose(fp);
}

// directory.csv
void directory()
{
	FILE* fp;
	fp = fopen("directory.csv", "w");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	unsigned char* buf = malloc(sizeof(char)*100);
	if (buf == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	for (int i=0;i<inode_address_count;i++)
	{
		// inode number
		int inode_number = inode_address[i];
		
		// Find the table entry
		int group = (inode_number-1) / inodes_per_group;
		int index = (inode_number-1) % inodes_per_group;
		int initial_offset = inode_table[group]*block_size + index*128;

		// file type
		get_buffer_reverse(buf,2,initial_offset+0);
		unsigned int i_mode = buffer_to_dec(buf,2);
		unsigned int t = i_mode >> 12;
		char i_type;
		
		// Only process directory
		if (t==4) 
		{
			unsigned* p;
			unsigned pcount;
			p = direct_block_ids(inode_number,&pcount);
			
			// For each data block
			unsigned entry_number = 0;
			for (int j=0;j<pcount;j++)
			{
				unsigned iter = 0;
				while (1)
				{
					unsigned address = p[j]*block_size + iter;
					
					// Get the whole entry
					get_buffer_reverse(buf,4,address+0);
					unsigned int entry_inode = buffer_to_dec(buf,4);
						
					get_buffer_reverse(buf,2,address+4);
					unsigned int entry_rec_len = buffer_to_dec(buf,2);
					
					get_buffer_reverse(buf,1,address+6);
					unsigned int entry_name_len = buffer_to_dec(buf,1);
					
					get_buffer_reverse(buf,1,address+7);
					unsigned int entry_file_type = buffer_to_dec(buf,1);
					
					get_buffer(buf,entry_name_len,address+8);
					char* entry_name = strndup(buf,entry_name_len);
					
					if (entry_inode!=0) fprintf(fp,"%d,%d,%d,%d,%d,\"%s\"\n",inode_number,entry_number,entry_rec_len,entry_name_len,entry_inode,entry_name);
		
					// Iterate
					iter += entry_rec_len;
					entry_number++;
					if (iter>=block_size) break;
				}
			}
		}
	}
	
	free(buf);
	fclose(fp);
}

// indirect.csv
void indirect()
{
	FILE* fp;
	fp = fopen("indirect.csv", "w");
	if (fp==NULL)
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	unsigned char* buf = malloc(sizeof(char)*100);
	if (buf == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	for (int in=0;in<inode_address_count;in++)
	{
		// inode number
		int inode_number = inode_address[in];
		
		// Find the table entry
		int group = (inode_number-1) / inodes_per_group;
		int index = (inode_number-1) % inodes_per_group;
		int initial_offset = inode_table[group]*block_size + index*128;

		// indirect block pointer
		get_buffer_reverse(buf,4,initial_offset+88);
		unsigned int p1 = buffer_to_dec(buf,4);
		
		// A value of 0 will terminate the array
		if (p1==0) continue;
		if (p1!=0)
		{
			unsigned int entry0 = 0;
			for (int i=0;i<block_size;i+=4)
			{
				get_buffer_reverse(buf,4,p1*block_size+i);
				unsigned int p0 = buffer_to_dec(buf,4);
				
				
				if (p0!=0)
				{
					fprintf(fp,"%x,%d,%x\n",p1,entry0,p0);
				}
				entry0++;
			}
		}
		
		// doubly block pointer
		get_buffer_reverse(buf,4,initial_offset+92);
		unsigned int p2 = buffer_to_dec(buf,4);
		if (p2==0) continue;
		if (p2!=0)
		{
			unsigned int entry1 = 0;
			for (int j=0;j<block_size;j+=4)
			{
				get_buffer_reverse(buf,4,p2*block_size+j);
				unsigned int p1 = buffer_to_dec(buf,4);
				
				if (p1!=0)
				{
					fprintf(fp,"%x,%d,%x\n",p2,entry1,p1);
				}
				entry1++;
			}
			
			for (int j=0;j<block_size;j+=4)
			{
				get_buffer_reverse(buf,4,p2*block_size+j);
				unsigned int p1 = buffer_to_dec(buf,4);
				
				if (p1!=0)
				{
					unsigned int entry0 = 0;
					for (int i=0;i<block_size;i+=4)
					{
						get_buffer_reverse(buf,4,p1*block_size+i);
						unsigned int p0 = buffer_to_dec(buf,4);
						
						if (p0!=0)
						{
							fprintf(fp,"%x,%d,%x\n",p1,entry0,p0);
						}
						entry0++;
					}
				}
			}
		}
		
		// triply block pointer
		get_buffer_reverse(buf,4,initial_offset+96);
		unsigned int p3 = buffer_to_dec(buf,4);
		if (p3==0) continue;
		if (p3!=0)
		{
			unsigned int entry2 = 0;
			for (int k=0;k<block_size;k+=4)
			{
				get_buffer_reverse(buf,4,p3*block_size+k);
				unsigned int p2 = buffer_to_dec(buf,4);
				
				if (p2!=0)
				{
					fprintf(fp,"%x,%d,%x\n",p3,entry2,p2);
				}
				entry2++;
			}
			
			for (int k=0;k<block_size;k+=4)
			{
				get_buffer_reverse(buf,4,p3*block_size+k);
				unsigned int p2 = buffer_to_dec(buf,4);
				
				if (p2!=0)
				{
					unsigned int entry1 = 0;
					for (int j=0;j<block_size;j+=4)
					{
						get_buffer_reverse(buf,4,p2*block_size+j);
						unsigned int p1 = buffer_to_dec(buf,4);
						
						if (p1!=0)
						{
							fprintf(fp,"%x,%d,%x\n",p2,entry1,p1);
						}
						entry1++;
					}
					
					for (int j=0;j<block_size;j+=4)
					{
						get_buffer_reverse(buf,4,p2*block_size+j);
						unsigned int p1 = buffer_to_dec(buf,4);
						
						if (p1!=0)
						{
							unsigned int entry0 = 0;
							for (int i=0;i<block_size;i+=4)
							{
								get_buffer_reverse(buf,4,p1*block_size+i);
								unsigned int p0 = buffer_to_dec(buf,4);
								
								if (p0!=0)
								{
									fprintf(fp,"%x,%d,%x\n",p1,entry0,p0);
								}
								entry0++;
							}
						}
					}
				}
			}
		}
		
		
	}
	
	free(buf);
	fclose(fp);
}

int main(int argc, char **argv)
{
	if (argc != 2) 
    {
        fprintf(stderr,"wrong number of argument");
		exit(1);
    }
	ifd = open(argv[1], O_RDONLY);
	if (ifd < 0) 
	{
		fprintf(stderr,"File open error");
		exit(1);
	}
	
	super();
	group();
	bitmap();
	inode();
	directory();
	indirect();
	
    exit(0);
}

