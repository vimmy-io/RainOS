#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "memlayout.h"

#include "dma.h"
#include "elf.h"
#include "fs.h"
#include "file.h"

#define DMA0_SLAVE			1	//16bit
#define DMA1_MASTER			2	//8bit

#define HDD_DMA_CHANNEL		3
#define DMA_TRANSFER	60000

void SetDMA()
{
#define ENABLE_DMA
#ifdef ENABLE_DMA
	//reset dma slave since
	//it has channel 3 (HDD)
	reset_dma(DMA0_SLAVE);

	//disable the hard disk channel
	disable_dma(HDD_DMA_CHANNEL);

	//set the page to transfer to
	clear_dma_ff(HDD_DMA_CHANNEL);
	set_dma_page(HDD_DMA_CHANNEL, (char)FOURMEG);

	//just doing 512 atm
	clear_dma_ff(HDD_DMA_CHANNEL);
	set_dma_count(HDD_DMA_CHANNEL, DMA_TRANSFER);

	set_dma_mode(HDD_DMA_CHANNEL, DMA_MODE_READ | DMA_AUTOINIT | DMA_MODE_TRANSFER_BLOCK);

	enable_dma(HDD_DMA_CHANNEL);

	//set_dma_mode(HDD_DMA_CHANNEL, DMA_MODE_READ | DMA_AUTOINIT | DMA_MODE_TRANSFER_BLOCK);
#endif
}

int
sys_ExFileRead(void)
{
	char *file = 0;
	int stradd;
	//int fd;
	//int n;
	struct inode *ip = 0;
	//struct elfhdr elf;

	if(argint(0, &stradd) < 0)
	   return 0;

	file = (char*)stradd;

	//cprintf(file);

	//cprintf("\n");

	ip = namei(file);

	cprintf("Inode number: %d\nType: %d\nSize: %d\n", ip->inum, ip->type, ip->size);


	uint add = PGROUNDDOWN((uint)proc->sz);
	mappages(proc->pgdir, (void*)add, DMA_TRANSFER, FOURMEG, PTE_U | PTE_W | PTE_P);

	//readfile(ip, (char*)add, DMA_TRANSFER);

#define MONO
#ifdef MONO
	uint start_time, time_taken, average_time = 0;
	int i = 0;
	for(i = 0; i < 25; i++)
	{
		SetDMA();

		start_time = ticks;

		ilock(ip);

		stradd = readi(ip, (char*)add, 0, DMA_TRANSFER);

		time_taken = ticks - start_time;
		cprintf("Time Taken: %d\n", time_taken);
		average_time = (average_time + time_taken) / 2;

		iunlock(ip);
	}

	cprintf("Average Time DMA: %d\n", average_time);

	for(i = 0; i < 25; i++)
	{
		start_time = ticks;

		ilock(ip);

		stradd = readi(ip, (char*)add, 0, DMA_TRANSFER);

		time_taken = ticks - start_time;
		cprintf("Time Taken: %d\n", time_taken);
		average_time = (average_time + time_taken) / 2;

		iunlock(ip);
	}

	cprintf("Average Time CPU: %d\n", average_time);
#endif

//	char *contents = (char*)add;
//	cprintf("Contents: ");
//
//	int i = 0;
//	for(i = 0; i < 512; i++)
//	{
//		consputc(contents[i]);
//	}

	cprintf("\n");

	return 0;
}
