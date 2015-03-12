#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "memlayout.h"

struct run {
	struct run *next;
};

int
sys_getframe(void)
{
	int num_frames;
	int num = 0;
	struct run *r;

	if(argint(0, &num_frames) < 0)
	    return 0;

	r = getfreelist();

	//cprintf("kmem is: %x\n", r);
	//cprintf("Num Wanted: %d\n", num_frames);

	//cprintf("Start Count: %d\n", num);

	while(r != 0 && num < num_frames)
	{
		r = r->next;
		num++;	//num++ not working
	}

	//cprintf("Num Found: %d\n", num);

	if(num != num_frames)
		return 0;

	return (unsigned)getfreelist();
}

int
sys_getva(void)
{
	return PGROUNDUP(proc->sz);
}

int
sys_getpde(void)
{
	int index;

	if(argint(0, &index) < 0)
		return -1;

	//if index is out of range
	if(index < 0 || index > (NPTENTRIES - 1))
		return -1;

	return (proc->pgdir[index]);
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
pte_t * getpgtab(pde_t *pgdir, const void *va, int alloc)
{
	pde_t *pde;
	pte_t *pgtab;

	pde = &pgdir[PDX(va)];
	if(*pde & PTE_P){
		pgtab = (pte_t*)p2v(PTE_ADDR(*pde));

		//cprintf("Page table present!\n");
	}
	else
	{
		cprintf("Page table NOT present!\n");

		if(!alloc)
			return 0;

		pgtab = (pte_t*)getfreelist();
		if(pgtab == 0)
			return 0;

		allocadd(proc->pid, (char*)getfreelist());

		cprintf("Page table alloc success!\n");

		// Make sure all those PTE_P bits are zero.
		memset(pgtab, 0, PGSIZE);

		*pde = v2p(pgtab) | PTE_P | PTE_W | PTE_U;
	}

	return &pgtab[PTX(va)];
}

int
sys_getpte(void)
{
	int va;

	if(argint(0, &va) < 0)
		return 0;

	return (uint)getpgtab(proc->pgdir, (void*)va, 0);
}

int
sys_addpage(void)
{
	int va, frame;
	pte_t *pgtab = 0;
	pde_t *pgdir = 0;
	//char *freeframe = 0;

	pgdir = proc->pgdir;

	if(argint(0, &va) < 0)
		return 0;

	if(argint(1, &frame) < 0)
		return 0;

	if(va % PGSIZE != 0)
		return 0;

	//this function should check to see if va is not in use

	//this allocadd needs to happen before any others
	//since the frame is still in the free list
	//and needs to be removed
	if(allocadd(proc->pid, (char*)frame) == 0)
		return 0;

	//add va and frame to a pagetable
	pgtab = getpgtab(pgdir, (void*)va, 1);
	if(!pgtab)
		return 0;

	if(*pgtab & PTE_P)
	{
		cprintf("Page Entry Present!\n");
		//check if entry belongs to the current process
		//return 0;
	}

	*pgtab = (uint)frame | PTE_U | PTE_W | PTE_P;

	proc->sz += PGSIZE;

	return 1;
}