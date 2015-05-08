typedef unsigned int uint;
#include "user.h"

const uint OBJ0 = 5122;
const uint OBJ1 = 4096;
const uint OBJ2 = 200;
const uint OBJ3 = 100;
const uint OBJ4 = 20;
const uint OBJ5 = 6;

const uint NUM = 10;

int
main(int argc, char *argv[])
{
	char *obj0[NUM];
	char *obj1[NUM];
	char *obj2[NUM];
	char *obj3[NUM];
	char *obj4[NUM];
	char *obj5[NUM];

	int i = 0;

	//allocate memory in all objects!!
	for(i = 0; i < NUM; i++)
	{
		obj0[i] = malloc(OBJ0);
	}

	for(i = 0; i < NUM; i++)
	{
		obj1[i] = malloc(OBJ1);
	}

	for(i = 0; i < NUM; i++)
	{
		obj2[i] = malloc(OBJ2);
	}

	for(i = 0; i < NUM; i++)
	{
		obj3[i] = malloc(OBJ3);
	}

	for(i = 0; i < NUM; i++)
	{
		obj4[i] = malloc(OBJ4);
	}

	for(i = 0; i < NUM; i++)
	{
		obj5[i] = malloc(OBJ5);
	}

	//deallocate some
	for(i = 0; i < NUM; i++)
	{
		free(obj2[i]);
	}

	for(i = 0; i < NUM; i++)
	{
		free(obj1[i]);
	}

	for(i = 0; i < NUM; i++)
	{
		free(obj3[i]);
	}

	for(i = 0; i < NUM; i++)
	{
		free(obj5[i]);
	}

	//allocate them again
	for(i = 0; i < NUM; i++)
	{
		obj3[i] = malloc(OBJ3);
	}

	for(i = 0; i < NUM; i++)
	{
		obj1[i] = malloc(OBJ1);
	}

	for(i = 0; i < NUM; i++)
	{
		obj5[i] = malloc(OBJ5);
	}

	for(i = 0; i < NUM; i++)
	{
		obj2[i] = malloc(OBJ2);
	}

	//deallocate some more
	for(i = 0; i < NUM; i++)
	{
		free(obj1[i]);
	}

	for(i = 0; i < NUM; i++)
	{
		free(obj4[i]);
	}

	for(i = 0; i < NUM; i++)
	{
		free(obj0[i]);
	}

	//reallocate
	for(i = 0; i < NUM; i++)
	{
		obj0[i] = malloc(OBJ0);
	}

	for(i = 0; i < NUM; i++)
	{
		obj1[i] = malloc(OBJ1);
	}

	for(i = 0; i < NUM; i++)
	{
		obj4[i] = malloc(OBJ4);
	}

	//check fragmentation

  exit();
}

