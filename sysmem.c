#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

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

	cprintf("kmem is: %x\n", r);
	cprintf("Num Wanted: %d\n", num_frames);

	cprintf("Start Count: %d\n", num);

	while(r != 0 && num < num_frames)
	{
		r = r->next;
		num++;	//num++ not working
	}

	cprintf("Num Found: %d\n", num);

	if(num != num_frames)
		return 0;

	return (unsigned)getfreelist();
}

int
sys_getva(void)
{
	return proc->sz;
}


