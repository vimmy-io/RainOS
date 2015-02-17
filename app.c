#include "types.h"
#include "stat.h"
#include "user.h"

void VATest()
{

	char *page = 0, *page2 = 0;

	printf(1, "Initial VA: %x\n", getva());
	page = (char*)malloc(4096);
	printf(1, "First VA: %x\n", getva());
	page2 = (char*)malloc(4096 * 10);
	printf(1, "Second VA: %x\n", getva());



	printf(1, "page: %x\n", page);
	printf(1, "page: %x\n", page2);

}

void FrameTest()
{
	char *frame;

	frame = getframe(56785);	//max = 56785 frames

	if(frame != 0)
		printf(1, "First Free Frame: %x\n", frame);
}

int
main(int argc, char *argv[])
{
	//VATest();
	//FrameTest();

  exit();
}
