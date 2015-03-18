#include "ff.h"
#include "common/log.h"
#include "../canny_edge_detector.h"
using namespace ff;
#include "utils.h"

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
    ff::paracontainer pg;
	for(uint32_t tx = 0; tx < height; tx++){
		for(uint32_t ty = 0; ty < width; ty++)
		{
			if(GetPixelValue(tx, ty) >= highThreshold)
			{
              ff::para<> a;
              a([this, highThreshold, lowThreshold, tx, ty]()
              {
                HysteresisPixel(tx, ty, highThreshold, lowThreshold);
              });
              pg.add(a);
            }
        }
    }
    ff_wait(all(pg));
}


