// Simple PIO-based (non-DMA) IDE driver code.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "buf.h"

#define IDE_BSY       0x80
#define IDE_DRDY      0x40
#define IDE_DF        0x20
#define IDE_ERR       0x01

#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30

// idequeue points to the buf now being read/written to the disk.
// idequeue->qnext points to the next buf to be processed.
// You must hold idelock while manipulating queue.

struct buffer
{
	uint device;
	uint sector;
	char *location;
	struct buffer *next;
};

static struct spinlock bufferlock;
static struct spinlock transferlock;
static struct buffer *idequeue;
static struct buffer *last;

static uint transfercount;

void ResetTransferCount(void)
{
	acquire(&transferlock);
	transfercount = 0;
	release(&transferlock);

	acquire(&bufferlock);
	idequeue = 0;
	last = 0;
	release(&bufferlock);
}

uint GetTransferCount(void)
{
	uint temp = 0;
	acquire(&transferlock);
	temp = transfercount;
	release(&transferlock);

	return temp;
}

// Wait for IDE disk to become ready.
static int
idewait(int checkerr)
{
  int r;

  while(((r = inb(0x1f7)) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
    ;
  if(checkerr && (r & (IDE_DF|IDE_ERR)) != 0)
    return -1;
  return 0;
}

//function to read, no buffer, no queues
void
ideread(uint sector, uint device)
{
	idewait(0);
	outb(0x3f6, 0);  // generate interrupt
	outb(0x1f2, 1);  // number of sectors
	outb(0x1f3, sector & 0xff);
	outb(0x1f4, (sector >> 8) & 0xff);
	outb(0x1f5, (sector >> 16) & 0xff);
	outb(0x1f6, 0xe0 | ((device&1)<<4) | ((sector>>24)&0x0f));
	outb(0x1f7, IDE_CMD_READ);

	idewait(1); //this line needs to be here and I DON'T KNOW WHYYYY
}

void ProcessBuffer()
{
	if(idequeue == 0)
		return;

	ideread(idequeue->sector, idequeue->device);
}

void
ex_addbuffer(uint sector, uint device, char *location)
{
	struct buffer *temp;
	temp = (struct buffer*)location;

	temp->device = device;
	temp->sector = sector;
	temp->location = location;
	temp->next = 0;

	acquire(&bufferlock);

	if(idequeue == 0)
	{
		idequeue = temp;
		last = temp;

	}
	else
	{
		last->next = temp;
		last = temp;
	}

	release(&bufferlock);

}

// Interrupt handler.
void
ex_ideintr(void)
{
	char *location = 0;
	// First queued buffer is the active request.
	acquire(&bufferlock);

	location = (char*)idequeue->location;

	idequeue = idequeue->next;

	if(idequeue == 0)
		last = 0;

	// Read data if needed.
	if(idewait(1) >= 0)
	{
		insl(0x1f0, location, 512/4);

		acquire(&transferlock);
		++transfercount;
		release(&transferlock);
	}

	release(&bufferlock);

	ProcessBuffer();
}
