// Memory layout

#define EXTMEM  0x100000            // Start of extended memory 1MB
#define PHYSTOP 0xE000000           // Top physical memory 224 MB
#define DEVSPACE 0xFE000000         // Other devices are at high addresses (4096 MB)

#define FOURMEG 0x400000	        // 4MB limitation where the kernel ends

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0x80000000         // First kernel virtual address (2048 MB) Not-Original: 3FE800
#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

#ifndef __ASSEMBLER__

static inline uint v2p(void *a) { return ((uint) (a))  - KERNBASE; }
static inline void *p2v(uint a) { return (void *) ((a) + KERNBASE); }

#endif

#define V2P(a) (((uint) (a)) - KERNBASE)
#define P2V(a) (((void *) (a)) + KERNBASE)

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as V2P, but without casts
