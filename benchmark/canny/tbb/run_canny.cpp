#include <tbb/parallel_for_each.h>
#include <tbb/task_scheduler_init.h>
// #include <tbb/concurrent_vector.h>//concurrent_vector
#include "tbb/task_group.h"//task_group
#include "../canny_edge_detector.h"


using namespace std;
using namespace tbb;
extern RRecord gRR;

void CannyEdgeDetector::Hysteresis(uint8_t lowThreshold, uint8_t highThreshold)
{
  start_record_cache_access();

    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    if(isPara)
    {
        this->ParaHysteresis(lowThreshold,highThreshold);
    }
    else {

        for (x = 0; x < height; x++) {
            for (y = 0; y < width; y++) {
                if (GetPixelValue(x, y) >= highThreshold) {
                    SetPixelValue(x, y, 255);
                    /*if(isPara)
                        this->HysteresisRecursion_para(x, y, lowThreshold);
                    else
                        this->*/
                    HysteresisRecursion(x, y, lowThreshold);
                }
            }
        }
    }
    end = chrono::system_clock::now();
    hysteresis_time = chrono::duration_cast<chrono::microseconds>(end-start).count();
    gRR.put("elpased-time", hysteresis_time);

    end_record_cache_access(gRR);
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
    task_group tg;
//     concurrent_vector<std::tuple< uint32_t, uint32_t > > ts;
    typedef vector< std::tuple< uint32_t, uint32_t > > pos_t;
    pos_t ts;
    for (uint32_t tx = 0; tx < height; tx++) {
        for (uint32_t ty = 0; ty < width; ty++) {

            if(GetPixelValue(tx, ty) >= highThreshold)
            {
                ts.push_back(make_tuple(tx, ty));
            }
        }
        if(ts.size() >= 1000 ||(tx==height -1))
        {
            tg.run([this,ts,lowThreshold,highThreshold]() {
//					pos_t tmpTs(ts);
//					for(pos_t::iterator iter = tmpTs.begin(); iter != tmpTs.end(); iter++){
//	                    HysteresisPixel(get<0>(*iter), get<1>(*iter), highThreshold, lowThreshold);
//					}
                parallel_for_each(ts.begin(), ts.end(),
                [this,lowThreshold,highThreshold](std::tuple<uint32_t, uint32_t> pos) {
                    HysteresisPixel(get<0>(pos), get<1>(pos), highThreshold, lowThreshold);
                });
            });
            ts.clear();
        }
    }
    tg.wait();
}


