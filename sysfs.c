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

typedef void (*function_ptr)(void);

#define min(a, b) ((a) < (b) ? (a) : (b))

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
	struct inode *ip = 0;

	if(argint(0, &stradd) < 0)
	   return 0;

	file = (char*)stradd;

	if(argint(1, &stradd) < 0)
		   return 0;

	char *location = 0;

	location = (char*)stradd;

	ip = namei(file);

	ilock(ip);

	int offset = 0;

	while(offset < ip->size)
	{
		ex_addbuffer(bmap(ip, offset / BSIZE), ip->dev, location);
		offset += 512;
		location += 512;
	}

	location -= 512;
	offset -= 512;
	location += ip->size - offset;
	offset += ip->size - offset;

	ex_addbuffer(bmap(ip, offset / BSIZE), ip->dev, location);

	iunlock(ip);

	return ip->size;
}

int sys_ExReadSector(void)
{
	ProcessBuffer();
	return 0;
}

int sys_ExResetTransferCount(void)
{
	ResetTransferCount();
	return 0;
}

int sys_ExGetTransferCount(void)
{
	return GetTransferCount();
}
