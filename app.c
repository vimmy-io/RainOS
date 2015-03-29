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
void load_bmp(char *file);

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

	ExFileRead("BabyTux.bmp", loc);	//last argument shoudl be loc

	printf(1, "Passed ExFile Read!!\n");

	ExReadSector();

	printf(1, "Passed ExReadSector!!\nTransfers: %d\n", ExGetTransferCount());

	load_bmp(loc);

  exit();
}

typedef struct tagBITMAP              /* the structure for a bitmap. */
{
  ushort width;
  ushort height;
  uchar *data;
} BITMAP;

struct FILE
{
	char *file;
	uint pointer;
};

char ReadF(struct FILE *f)
{
	uint temp = f->pointer;
	f->pointer++;
	return f->file[temp];
}

void ReadSize(void *dst, struct FILE *fp, uint size)
{
	memmove(dst, (void*)&fp->file[fp->pointer], size);
	fp->pointer += size;
}

void load_bmp(char *file)
{
  struct FILE fp;

  fp.file = file;
  fp.pointer = 0;

  /* check to see if it is a valid bitmap file */
  if (ReadF(&fp)!='B' || ReadF(&fp)!='M')
  {
    printf(1, "Not a valid bmp file %c %c!!\n", fp.file[0], fp.file[1]);
    return;
  }

  /* read in the width and height of the image, and the
     number of colors used; ignore the rest */
  fp.pointer += 16;

  BITMAP b;

  ReadSize((void*)&b.width, &fp, sizeof(b.width));
  printf(1, "Width: %d\n", b.width);

  fp.pointer += 2;

  ReadSize((void*)&b.height, &fp, sizeof(b.height));
  printf(1, "Height: %d\n", b.height);

  fp.pointer += 22;

  ushort num_colors;
  ReadSize((void*)&num_colors, &fp, sizeof(num_colors));

  fp.pointer += 6;

  /* assume we are working with an 8-bit file */
  if (num_colors==0) num_colors=256;


  /* try to allocate memory */
  if ((b.data = (uchar *) malloc((ushort)(b.width * b.height))) == 0)
  {
    printf(1, "Error allocating memory for file\n");
    return;
  }

  /* Ignore the palette information for now.
     See palette.c for code to read the palette info. */
  //fskip(fp,num_colors*4);
  fp.pointer += num_colors * 4;

  /* read the bitmap */
  long index = 0;
  int x = 0;

  for(index = (b.height - 1) * b.width; index >= 0; index -= b.width)
    for( x = 0; x < b.width; x++)
      b.data[(ushort)index + x] =(uchar)ReadF(&fp);
}

//void load_bmp(char *file,BITMAP *b)
//{
//  FILE *fp;
//  long index;
//  word num_colors;
//  int x;
//
//  /* open the file */
//  if ((fp = fopen(file,"rb")) == NULL)
//  {
//    printf("Error opening file %s.\n",file);
//    exit(1);
//  }
//
//  /* check to see if it is a valid bitmap file */
//  if (fgetc(fp)!='B' || fgetc(fp)!='M')
//  {
//    fclose(fp);
//    printf("%s is not a bitmap file.\n",file);
//    exit(1);
//  }
//
//  /* read in the width and height of the image, and the
//     number of colors used; ignore the rest */
//  fskip(fp,16);
//  fread(&b->width, sizeof(word), 1, fp);
//  fskip(fp,2);
//  fread(&b->height,sizeof(word), 1, fp);
//  fskip(fp,22);
//  fread(&num_colors,sizeof(word), 1, fp);
//  fskip(fp,6);
//
//  /* assume we are working with an 8-bit file */
//  if (num_colors==0) num_colors=256;
//
//
//  /* try to allocate memory */
//  if ((b->data = (byte *) malloc((word)(b->width*b->height))) == NULL)
//  {
//    fclose(fp);
//    printf("Error allocating memory for file %s.\n",file);
//    exit(1);
//  }
//
//  /* Ignore the palette information for now.
//     See palette.c for code to read the palette info. */
//  fskip(fp,num_colors*4);
//
//  /* read the bitmap */
//  for(index=(b->height-1)*b->width;index>=0;index-=b->width)
//    for(x=0;x<b->width;x++)
//      b->data[(word)index+x]=(byte)fgetc(fp);
//
//  fclose(fp);
//}

