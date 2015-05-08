#include "libos.h"

int
main(int argc, char *argv[])
{
	ExRegister();

	init();

	//allocations

	defrag();
	exit();
}
