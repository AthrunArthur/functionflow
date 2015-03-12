#include "cache_eval.h"

#ifdef CACHE_EVAL
int g_event_set;
long long startRecords[2], endRecords[2];
void start_record_cache_access()
{
  int retVal = PAPI_library_init(PAPI_VER_CURRENT);
  assert(retVal == PAPI_VER_CURRENT);
  EventSet = PAPI_NULL;
  retVal = PAPI_create_eventset(&g_event_set);
  assert(retVal == PAPI_OK);

  retVal = PAPI_add_event(g_event_set, PAPI_L1_TCM);
  assert(retVal == PAPI_OK);

  retVal = PAPI_start(g_event_set);
  assert(retVal == PAPI_OK);
  retVal = PAPI_read(g_event_set, startRecords);
  assert(retVal == PAPI_OK);
}
void end_record_cache_access(RRecord & rr)
{
  int retVal;
  retVal = PAPI_stop(g_event_set, endRecords);
  assert(retVal == PAPI_OK);
  retVal = PAPI_cleanup_eventset(g_event_set);
  assert(retVal == PAPI_OK);
  retVal = PAPI_destroy_eventset(&g_event_set);
  assert(retVal == PAPI_OK);
  PAPI_shutdown(); 
  rr.put("L1-cache-misss", endRecords[0] - startRecords[0]);
  rr.put("L1-cache-access", endRecords[1] - startRecords[1]);
}
#else
void start_record_cache_access(){}
void end_record_cache_access(RRecord & rr){}
#endif
