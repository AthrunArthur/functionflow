#include "ff.h"
#include <stdio.h>
#include "common/log.h"

using namespace ff;

int main(int argc, char *argv[])
{
  _DEBUG(ff::fflog<>::init(ff::TRACE, "log.txt"))
    _DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
	para<> a;
//	a([](){printf("hello world\n");});
//	ff_wait(a);
//	printf("goodbye world\n");

	a([](){printf("hello world\n");}).then([](){
		printf("goodbye world\n");});
	return 0;
}
