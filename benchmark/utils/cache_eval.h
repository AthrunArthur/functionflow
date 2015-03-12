#ifndef BENCHMARK_CACHE_EVAL_H_
#define BENCHMARK_CACHE_EVAL_H_
#ifdef CACHE_EVAL
#include <papi.h>
#endif
#include <assert.h>
#include "rrecord.h"

void start_record_cache_access();
void end_record_cache_access(RRecord & rr);


#endif
