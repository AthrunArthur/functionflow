#include "../matrix.h"
#include "tbb/task.h"
#include "tbb/tbb.h"
using namespace tbb;
//Thanks, Siyuan
//

void para_mul(const Matrix & m1, const Matrix &m2, Matrix &res)
{
	task_group tg;
}
