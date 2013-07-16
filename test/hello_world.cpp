#include "ff.h"
#include <stdio.h>
using namespace ff;

int main(int argc, char *argv[])
{
	para<> a;
//	a([](){printf("hello world\n");});
//	ff_wait(a);
//	printf("goodbye world\n");

	a([](){printf("hello world\n");}).then([](){
		printf("goodbye world\n");});
	return 0;
}
