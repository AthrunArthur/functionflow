#include "ff.h"
#include "common/log.h"
#include "../canny_edge_detector.h"

void CannyEdgeDetector::Hysteresis(uint8_t lowThreshold, uint8_t highThreshold)
{
#ifdef CACHE_EVAL
    /*Add papi to trace cache miss*/
    int EventSet,retVal;
    long long startRecords[2], endRecords[2];
    retVal = PAPI_library_init(PAPI_VER_CURRENT);
    assert(retVal == PAPI_VER_CURRENT);
    EventSet = PAPI_NULL;
    retVal = PAPI_create_eventset(&EventSet);
    assert(retVal == PAPI_OK);
    //L1 TCM & TCA
    retVal = PAPI_add_event(EventSet, PAPI_L1_TCM);
    assert(retVal == PAPI_OK);
    retVal = PAPI_add_event(EventSet, PAPI_L1_TCA);
    assert(retVal == PAPI_OK);
    
    //L2 TCM & TCA
//     retVal = PAPI_add_event(EventSet, PAPI_L2_TCM);
//     assert(retVal == PAPI_OK);
//     retVal = PAPI_add_event(EventSet, PAPI_L2_TCA);
//     assert(retVal == PAPI_OK);
    
    //L3 TCM & TCA
//     retVal = PAPI_add_event(EventSet, PAPI_L3_TCM);
//     assert(retVal == PAPI_OK);
//     retVal = PAPI_add_event(EventSet, PAPI_L3_TCA);
//     assert(retVal == PAPI_OK);    
    
    retVal = PAPI_start(EventSet);
    assert(retVal == PAPI_OK);
    retVal = PAPI_read(EventSet, startRecords);
    assert(retVal == PAPI_OK);
    /*Add papi to trace cache miss*/
#endif    
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    if(isPara)
    {
        this->ParaHysteresis(lowThreshold,highThreshold);
    }
    else {
        bool no_modify = false;
        //while(!no_modify)
        {
            no_modify = true;
            for (x = 0; x < height; x++) {
                for (y = 0; y < width; y++) {
                    if (GetPixelValue(x, y) >= highThreshold) {
                        if(GetPixelValue(x, y) != 255)
                            no_modify = false;
                        SetPixelValue(x, y, 255);
                        /*if(isPara)
                            this->HysteresisRecursion_para(x, y, lowThreshold);
                        else
                            this->*/
                        no_modify = HysteresisRecursion(x, y, lowThreshold);
                    }
                }
            }
        }
    }
    end = chrono::system_clock::now();
    hysteresis_time = chrono::duration_cast<chrono::microseconds>(end-start).count();

#ifdef CACHE_EVAL
    /*Stop papi trace*/
    retVal = PAPI_stop(EventSet, endRecords);
    assert(retVal == PAPI_OK);
    retVal = PAPI_cleanup_eventset(EventSet);
    assert(retVal == PAPI_OK);
    retVal = PAPI_destroy_eventset(&EventSet);
    assert(retVal == PAPI_OK);
    PAPI_shutdown(); 
    //L1 result
    std::cout << "L1 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
    std::cout << "L1 total cache access = " << endRecords[1] - startRecords[1] << std::endl;
    //L2 result
//     std::cout << "L2 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
//     std::cout << "L2 total cache access = " << endRecords[0] - startRecords[0] << std::endl;
    //L3 result
//     std::cout << "L3 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
//     std::cout << "L3 total cache access = " << endRecords[0] - startRecords[0] << std::endl;
    /*Stop papi trace*/
#endif    
    for (x = 0; x < height; x++) {//no big differences?
        for (y = 0; y < width; y++) {
            if (GetPixelValue(x, y) != 255) {
                SetPixelValue(x, y, 0);
            }
        }
    }
}

void CannyEdgeDetector::ParaHysteresis(uint8_t lowThreshold, uint8_t highThreshold)
{
    ff::paracontainer pg;
    typedef std::vector<std::tuple<uint32_t, uint32_t> > pos_t;
    pos_t ts;
    for (uint32_t tx = 0; tx < height; tx++) {
        for (uint32_t ty = 0; ty < width; ty++) {

            if(GetPixelValue(tx, ty) >= highThreshold)
            {
                ts.push_back(make_tuple(tx, ty));
            }
        }
        
        //if(ff::is_idle() ||(tx==height -1))
        if(ts.size() >= 1000 ||(tx==height -1))
        {
            ff::para<void> p;
	    p([this, ts, lowThreshold, highThreshold]() {
	      
                ff::paragroup pp;
		
                pp.for_each(ts.begin(), ts.end(), [this, lowThreshold, highThreshold](std::tuple<uint32_t, uint32_t> pos) {
		  //std::cout<<get<0>(pos)<<", "<<get<1>(pos)<<std::endl;
                    HysteresisPixel(get<0>(pos), get<1>(pos), highThreshold, lowThreshold);
		    //std::cout<<"done: "<<get<0>(pos)<<", "<<get<1>(pos)<<std::endl;
                });
                ff_wait(all(pp));
            });
            pg.add(p);
            ts.clear();
        }
    }
//    std::cout<<"all done!"<<std::endl;
    _DEBUG(LOG_INFO(main)<<"add all para entities");
    ff_wait(all(pg));
//	std::cout << "all done2!" << std::endl;
}


void initial_para_env(int thrd_num)
{
  ff::rt::set_hardware_concurrency(8);//Set concurrency
  _DEBUG(ff::fflog<>::init(ff::INFO, "log.txt"))
    _DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
  ff::para<int> a;
  int num = 10;
  a([&num]() {
      return num;
      }).then([](int x) {});
  ff::para<> b;
  b[a]([&num, &a]() {
      num + a.get();
      }).then([]() {});
}
