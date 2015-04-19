#include "types.h"
#include "stat.h"
//#include "user.h"
#include "libos.h"
#include "mmu.h"
#include "elf.h"

//#include "ex_fs.h"

#define PHYADDR(va, pte)	(((uint)(va) & 0xFFF) & ((uint)(pte) & ~0xFFF))

#define SET_NUM	100

void load_bmp(char *file, char*);

int
main(int argc, char *argv[])
{
	int time = 0;

	//time = uptime();

	//load_bmp("BabyTux.bmp");
	//time = uptime() - time;

	char skippableBuffer[256 * 4];

	int i = 0;
	for(; i < SET_NUM; i++)
	{
		time = uptime();

		load_bmp("BabyTux.bmp", skippableBuffer);

		time = uptime() - time;

		printf(1, "%d\n", time);
	}

	//printf(1, "Time: %d", time);

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

void load_bmp(char *file, char *skippableBuffer)
{
  int fd = 0;

  fd = open(file, 0);
  struct FILE fp;

  fp.file = file;
  fp.pointer = 0;

  char c, d;
  read(fd, &c, sizeof(c));
  read(fd, &d, sizeof(d));

  /* check to see if it is a valid bitmap file */
  if (c !='B' || d !='M')
  {
    printf(1, "Not a valid bmp file %c %c!!\n", fp.file[0], fp.file[1]);
    return;
  }

  BITMAP b;

  /* read in the width and height of the image, and the
       number of colors used; ignore the rest */
  read(fd, skippableBuffer, 16);

  read(fd, &b.width, sizeof(b.width));

  	  //ReadSize((void*)&b.width, &fp, sizeof(b.width));
  	  //printf(1, "Width: %d\n", b.width);

  read(fd, skippableBuffer, 2);

  read(fd, &b.height, sizeof(b.height));
  	  //ReadSize((void*)&b.height, &fp, sizeof(b.height));
  	  //printf(1, "Height: %d\n", b.height);

  read(fd, skippableBuffer, 22);

  ushort num_colors;
  read(fd, &num_colors, sizeof(num_colors));
  	  //ReadSize((void*)&num_colors, &fp, sizeof(num_colors));

  read(fd, skippableBuffer, 6);

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
  //fp.pointer += num_colors * 4;
  read(fd, skippableBuffer, num_colors * 4);

  /* read the bitmap */
  long index = 0;
  int x = 0;

  for(index = (b.height - 1) * b.width; index >= 0; index -= b.width)
    for( x = 0; x < b.width; x++)
      b.data[(ushort)index + x] =(uchar)read(fd, &c, sizeof(c));//ReadF(&fp);

  close(fd);
}

