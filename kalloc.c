// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

//debug?
//#define LOCAL_DEBUG

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

//eight byte aligned
//since 1 page => 4096bytes => 512 entries
struct framelist {
	int pid;
	char *frame;
	struct framelist *next;
	char *padding;
};

struct framelist start;

//number of frames allocated
int frames = 0;

char* allocadd(int);

void
initframes()
{
#ifdef LOCAL_DEBUG
	struct framelist *f;
#endif

	frames = 0;

	start.next = (struct framelist*)P2V(PHYSTOP);
	start.next->next = 0;
	start.frame = 0;
	start.pid = -1;

#ifdef LOCAL_DEBUG
	allocadd(1);
	allocadd(2);
	allocadd(3);


	f = &start;

	while(f != 0)
	{
		cprintf("PID: %d\n", f->pid);
		cprintf("ADD: %x\n", f);
		f = f->next;
	}
#endif
//	cprintf("Current Frame: %x\n", &start);
//	cprintf("Current PID: %d\n\n", start.pid);
//
//	cprintf("Next    Frame: %x\n", start.next);
//	cprintf("Current PID: %d\n\n", start.next->pid);
//
//	cprintf("Next    Frame: %x\n", f);
//	cprintf("Current PID: %d\n\n", f->pid);
}

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}

//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || v2p(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  //calls stosb (store string byte)
  //which stores a byte of data in 2nd argument(1)
  //for n bytes (PGSIZE)
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

//Allocate a page and add it to the allocated pages list
char*
allocadd(int pid)
{
  struct run *r;
  struct framelist *f, *new_slot;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  if(kmem.use_lock)
    release(&kmem.lock);

  //add the page r to the allocated frames list

  //find the next free slot
  f = &start;
  while(f->next != 0) f = f->next;

#ifdef LOCAL_DEBUG
  cprintf("Process PID: %d\n", pid);
  cprintf("Free Slot: %x\n", f);
#endif

  //set the frame in the list
  f->pid = pid;
  f->frame = (char*)r;

#ifdef LOCAL_DEBUG
  cprintf("Free Frame: %x\n", r);
#endif

  new_slot = f + 1;// + sizeof(struct framelist);
  new_slot->next = 0;

#ifdef LOCAL_DEBUG
  cprintf("New Slot: %x\n", new_slot);
  cprintf("New Slot Next: %x\n\n", new_slot->next);
#endif

  f->next = new_slot;

  return (char*)r;
}


