#ifndef _LIBOS_H
#define _LIBOS_H

#include "user.h"

struct header
{
	unsigned int size;
	union {
		unsigned int i;
		char *p;
	};
};

struct page
{
	struct header *start;
	struct header *freelist;

	struct page *next;
};

static struct page *pagelist = 0;

void init()
{
	char* va = ExGetVA();

	ExAddPage(va, ExGetFrame(1));

	pagelist = (struct page*)va;

	pagelist->start = 0;
	pagelist->freelist = 0;

	pagelist->next = 0;
}

char* alloc(unsigned int size)
{


	return 0;
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
