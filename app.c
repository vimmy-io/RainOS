#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"

#define PHYADDR(va, pte)	(((uint)(va) & 0xFFF) & ((uint)(pte) & ~0xFFF))

void VATest()
{

	char *page = 0, *page2 = 0;
	uint va = (unsigned)getva();
	//va = (((va) >> 22) & 0x3FF);

	printf(1, "Initial VA: %x\n", getva());
	printf(1, "Initial Entry: %x\n", getpde(va));

	page = (char*)malloc(4096);
	va = (unsigned)getva();

	printf(1, "First VA: %x\n", getva());
	printf(1, "First Entry: %x\n", getpde(va));

	page2 = (char*)malloc(4096 * 10);
	va = (unsigned)getva();

	printf(1, "Second VA: %x\n", getva());
	printf(1, "Second Entry: %x\n", getpde(va));

	page2 = (char*)malloc(4096 * 10);
	va = (unsigned)getva();

	printf(1, "Second VA: %x\n", getva());
	printf(1, "Third Entry: %x\n", getpde(va));

	printf(1, "page: %x\n", page);
	printf(1, "page: %x\n", page2);

}

void FrameTest()
{
	char *frame;

	frame = getframe(56785);	//max = 56785 frames

	if(frame != 0)
		printf(1, "First Free Frame: %x\n", frame);
}

void PageTableTest()
{
	uint va = (unsigned)getva();
	int i = 0;
	char *page = 0;

	printf(1, "VA PDE: %x\n", getpde(PDX(va)));

	page = (char*)malloc(4096 * 10);
	va = (unsigned)getva();

	printf(1, "VA PTE: %x\n", getpte(va));
	printf(1, "VA PTE2: %x\n", getpte(va + 4096));

	for(i = 512; i < 1024; i++)
	{
		if((unsigned)getpde(i) & PTE_P)
			printf(1, "%d (%x)   ", i, PTE_ADDR(getpde(i)));
	}

	printf(1, "\n");
	printf(1, "Page: %x\n", page);

	//printf(1, "%d\n", getpde(-1));
	//printf(1, "%d\n", getpde(1024));
}

void AllocTest()
{
	uint va = (uint)getva();
	char *page = 0, *page2 = 0;
	int count = 0;

	while(getpte(va))
	{
		va += PGSIZE;
		count++;
	};

	printf(1, "Count: %d\n", count);
	printf(1, "Directory %d\n", PDX(va));

	page = getframe(1);
	//printf(1, "PTE: %x\n", (uint)getpte(va)>>12);

	printf(1, "Current VA: %x\n", va);

	printf(1, "Frame to allocate: %x\n", (uint)page>>12);

	if(addpage((char*)va, page) == 1)
	{
		page = (char*)va;
		page = "HELLO";
		printf(1, "PTE: %x\n", getpte(va));
		printf(1, "%s\n\n", page);
	}

	va += PGSIZE;//(uint)getva();
	printf(1, "PTE: %x\n", getpte(va));
	printf(1, "Current VA: %x\n", va);

	page2 = getframe(1);

	printf(1, "Frame to allocate: %x\n", page2);

	if(addpage(getva(), page2) == 1)
	{
		page2 = (char*)va;
		page2 = "WORLD";
		printf(1, "PTE: %x\n", getpte(va));
		printf(1, "%s\n\n", page2);
	}
}

int
main(int argc, char *argv[])
{
	//VATest();
	//FrameTest();
	//PageTableTest();

	AllocTest();

  exit();
}
