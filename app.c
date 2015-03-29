#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h"
#include "elf.h"

//#include "ex_fs.h"

#define PHYADDR(va, pte)	(((uint)(va) & 0xFFF) & ((uint)(pte) & ~0xFFF))

void VATest()
{

	char *page = 0, *page2 = 0;
	uint va = (unsigned)ExGetVA();
	//va = (((va) >> 22) & 0x3FF);

	printf(1, "Initial VA: %x\n", ExGetVA());
	printf(1, "Initial Entry: %x\n", ExGetPDE(va));

	page = (char*)malloc(4096);
	va = (unsigned)ExGetVA();

	printf(1, "First VA: %x\n", ExGetVA());
	printf(1, "First Entry: %x\n", ExGetPDE(va));

	page2 = (char*)malloc(4096 * 10);
	va = (unsigned)ExGetVA();

	printf(1, "Second VA: %x\n", ExGetVA());
	printf(1, "Second Entry: %x\n", ExGetPDE(va));

	page2 = (char*)malloc(4096 * 10);
	va = (unsigned)ExGetVA();

	printf(1, "Second VA: %x\n", ExGetVA());
	printf(1, "Third Entry: %x\n", ExGetPDE(va));

	printf(1, "page: %x\n", page);
	printf(1, "page: %x\n", page2);

}

void FrameTest()
{
	char *frame;

	frame = ExGetFrame(56785);	//max = 56785 frames

	if(frame != 0)
		printf(1, "First Free Frame: %x\n", frame);
}

void PageTableTest()
{
	uint va = (unsigned)ExGetVA();
	int i = 0;
	char *page = 0;

	printf(1, "VA PDE: %x\n", ExGetPDE(PDX(va)));

	page = (char*)malloc(4096 * 10);
	va = (unsigned)ExGetVA();

	printf(1, "VA PTE: %x\n", ExGetPTE(va));
	printf(1, "VA PTE2: %x\n", ExGetPTE(va + 4096));

	for(i = 512; i < 1024; i++)
	{
		if((unsigned)ExGetPDE(i) & PTE_P)
			printf(1, "%d (%x)   ", i, PTE_ADDR(ExGetPDE(i)));
	}

	printf(1, "\n");
	printf(1, "Page: %x\n", page);

	//printf(1, "%d\n", ExGetPDE(-1));
	//printf(1, "%d\n", ExGetPDE(1024));
}

void AllocTest()
{
	uint va = (uint)ExGetVA();
	char *page = 0, *page2 = 0;
	int count = 0;

	while(ExGetPTE(va))
	{
		va += PGSIZE;
		count++;
	};

	printf(1, "Count: %d\n", count);
	printf(1, "Directory %d\n", PDX(va));

	page = ExGetFrame(1);
	//printf(1, "PTE: %x\n", (uint)ExGetPTE(va)>>12);

	printf(1, "Current VA: %x\n", va);

	printf(1, "Frame to allocate: %x\n", (uint)page>>12);

	if(ExAddPage((char*)va, page) == 1)
	{
		page = (char*)va;
		page = "HELLO";
		printf(1, "PTE: %x\n", ExGetPTE(va));
		printf(1, "%s\n\n", page);
	}

	va += PGSIZE;//(uint)ExGetVA();
	printf(1, "PTE: %x\n", ExGetPTE(va));
	printf(1, "Current VA: %x\n", va);

	page2 = ExGetFrame(1);

	printf(1, "Frame to allocate: %x\n", page2);

	if(ExAddPage(ExGetVA(), page2) == 1)
	{
		page2 = (char*)va;
		page2 = "WORLD";
		printf(1, "PTE: %x\n", ExGetPTE(va));
		printf(1, "%s\n\n", page2);
	}

	printf(1, "END\n");
}

void FSTest()
{
	//int fd;
		//char* file = "primer";	//uncomment to work
	//struct elfhdr elf;
	//int n;

	//int time = uptime();
		//ExFileRead(file);	//uncomment to work

//	fd = open(file, 0);
//
//	if((n = read(fd, &elf, sizeof(elf))) < 0)
//	{
//		printf(1, "Bytes not read!!\n");
//		return;
//	}
//
//	if(elf.magic != ELF_MAGIC)
//	{
//		printf(1, "Elf not Magic!!\n");
//		return;
//	}
//	else
//	{
//		printf(1, "Read: %d  Actual: %d\n", elf.magic, ELF_MAGIC);
//	}
//
//	printf(1, "Time Taken: %d\n", uptime() - time);
}

void DealWithIt()
{
	printf(1, "Dealt with it!!\n");
}

int
main(int argc, char *argv[])
{
	ExRegister();

	//VATest();
	//FrameTest();
	//PageTableTest();

	//AllocTest();

	//FSTest();

	//function_ptr call;
	//call = DealWithIt;

	//(*call)();

	//printf(1, "Call: %x\n", call);

	//ExReadSector(22, 0, 0);
	char *loc = 0;
	loc = malloc(75000);

	if(!loc)
		printf(1, "Malloc failed!!\n");

	ExResetTransferCount();

	ExFileRead("test.txt", loc);	//last argument shoudl be loc

	printf(1, "Passed ExFile Read!!\n");

	ExReadSector();

	printf(1, "Passed ExReadSector!!\nTransfers: %d\n", ExGetTransferCount());

//	int i = 0;
//	for(; i < 71680; i++)
//	{
//		printf(1, "%s", loc);
//	}

//	uint prevCount = 0;
//	prevCount = ExGetTransferCount();
//
//	while(1)
//	{
//		if(prevCount != ExGetTransferCount())
//		{
//			printf(1, "\n\n =====================COUNT CHANGED ==========================================\n\n");
//			prevCount = ExGetTransferCount();
//		}
//	}

  exit();
}
