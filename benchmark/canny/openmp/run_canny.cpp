
#include <omp.h>

#include "../canny_edge_detector.h"


using namespace std;

void CannyEdgeDetector::Hysteresis(uint8_t lowThreshold, uint8_t highThreshold)
{
  chrono::time_point<chrono::system_clock> start, end;
  start = chrono::system_clock::now();
  if(isPara)
  {
#pragma omp parallel
#pragma omp single
    {
      this->ParaHysteresis(lowThreshold,highThreshold);
    }
#pragma omp taskwait
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
  typedef vector< std::tuple< uint32_t, uint32_t > > pos_t;
  pos_t ts;
  for (uint32_t tx = 0; tx < height; tx++) {
    ts.clear();
    for (uint32_t ty = 0; ty < width; ty++) {

      if(GetPixelValue(tx, ty) >= highThreshold)
      {
        ts.push_back(make_tuple(tx, ty));
      }
    }
    if(ts.size() >= 1000 ||(tx==height -1))
    {
#pragma omp task
      {
        pos_t tmpTs(ts);

        for(pos_t::iterator iter = tmpTs.begin(); iter != tmpTs.end(); iter++){
          HysteresisPixel(get<0>(*iter), get<1>(*iter), highThreshold, lowThreshold);
        }
      }
    }
  }
  //	#pragma omp taskwait
}

void initial_para_env(int thrd_num)
{
  omp_set_num_threads(8);
}
