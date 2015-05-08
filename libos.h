#ifndef _LIBOS_H
#define _LIBOS_H

typedef unsigned int uint;

#include "user.h"

struct chunk
{
	uint size;
	struct chunk *next;
};

const uint PGSZ = 4096;			//page size
const uint CSZ = sizeof(struct chunk); //chunk size

//need c++
//NO TYPESAFETY IN C????
//template<typename T>
//union address
//{
//	T *p;
//	uint i;
//};

struct page
{
	uint pages;
	uint current_size;

	struct chunk *freelist;
	struct chunk *alloclist;

	struct page *next;
};

static struct page *pagelist = 0;

void init()
{
	char* va = ExGetVA();

	ExAddPage(va, ExGetFrame(1));

	pagelist = (struct page*)va;

	pagelist->pages = 1;
	pagelist->current_size = PGSZ - sizeof(struct page);

	pagelist->freelist = 0;
	pagelist->alloclist = 0;

	pagelist->next = 0;
}

//page aligned allocation, for easy swapping on revocation
//although revocation is not implemented
char* alloc(unsigned int size)
{
	char *location = 0;

	struct page *it = pagelist;

	//needs to check freelist of each page first
	//while(it != 0)
	//{
		if((it->current_size + CSZ) < size)
		{
			location = (char*)(it->current_size + CSZ);

			struct chunk *current = 0;

			//ideally should be in a function
			current = (struct chunk*)it->current_size;

			current->size = size;
			current->next = it->alloclist;

			it->current_size -= size + CSZ;

			//break;
		}
		else if(size < (PGSZ - sizeof(struct page)))	//if cannot fit in current page but can fit within one page
		{
			//new page needs allocation
			char* va = ExGetVA();

			ExAddPage(va, ExGetFrame(1));

			struct page *temp = (struct page*)va;

			temp->pages = 1;
			temp->current_size = PGSZ - sizeof(struct page);

			temp->alloclist = 0;

			temp->next = it;

			it = temp;

			struct chunk *current = 0;

			current = (struct chunk*)it->current_size;

			current->size = size;
			current->next = it->alloclist;

			location = (char*)(it->current_size + CSZ);
			it->current_size -= size + CSZ;

			//break;

		}
		else	//if more than one page
		{
			uint num = 0;
			if(size % PGSZ == 0)
				num = size / PGSZ;
			else
				num = (size / PGSZ) + 1;

			int i = 0;

			struct page *temp = 0;

			//ideally should be in a function
			for(; i < num; i++)
			{
				char* va = ExGetVA();

				ExAddPage(va, ExGetFrame(1));

				if(i == 0)
					temp = (struct page*)va;
			}

			temp->pages = num;
			temp->current_size = (PGSZ * num) - sizeof(struct page);

			temp->alloclist = 0;
			temp->next = it;

			struct chunk *current = 0;

			current = (struct chunk*)it->current_size;

			current->size = size;
			current->next = it->alloclist;

			location = (char*)(it->current_size + CSZ);
			it->current_size -= size + CSZ;

			//break;
		}
	//}

	return location;
}

//can be used for freelist and alloclist
struct chunk* remove_chunk(char *address, struct chunk *list)
{
	struct chunk *prev = list;

	struct chunk *ret = 0;

	while(list->next != 0)
	{
		if(list == (struct chunk*)address)
		{
			ret = list;

			//remove node
			prev->next = list->next;
			break;
		}

		prev = list;
		list = list->next;
	}

	return ret;
}

void dalloc(char *address)
{
	struct page *it = pagelist;

	char *location = (char*)((uint)address - CSZ);

	while(it != 0)
	{
		struct chunk *temp = remove_chunk(location, it->alloclist);

		if(temp != 0)
		{
			temp->next = it->freelist;

			it->freelist = temp;
			break;
		}

		it = it->next;
	}
}

void defrag()
{
	struct page *it = pagelist;

	while(it != 0)
	{
		struct chunk *temp = it->freelist;

		while(temp != 0)
		{
			//since list is backwards
			if((uint)temp->next + temp->next->size == (uint)temp)
			{
				struct chunk *current = remove_chunk((char*)temp, it->freelist);

				//combine contiguous memory
				temp->next->size += current->size;
			}
		}

		it = it->next;
	}
}

 //returns address of memory mapped region
 char* FileTransfer(char *name, uint size)
 {
	char *loc = 0;

	loc = malloc(size);

	ExResetTransferCount();

	ExFileRead(name, loc);

	ExReadSector();

	return loc;
 }

#endif
