#include "ff.h"
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

	typedef std::tuple<uint32_t, uint32_t> pos_t;
	typedef std::vector<pos_t> vec_pos_t;

	ff::paracontainer pg;

	vec_pos_t vt;
	//size_t check_step_threshold = 64;
	//size_t check_step = 0;
	size_t thres_hold = (1<<16)/sizeof(pos_t);

	for(uint32_t tx = 0; tx < height; tx++){
		for(uint32_t ty = 0; ty < width; ty++)
		{
			if(GetPixelValue(tx, ty) >= highThreshold)
			{
				vt.push_back(make_tuple(tx, ty));
				/*
				   if(check_step >= check_step_threshold)
				   {
				   auto rtstate = ff::rt::current_task_counter();
				   auto gap = std::get<1>(rtstate) - std::get<0>(rtstate);
				   check_step = 0;
				   }*/
				if(vt.size() >= thres_hold)
				{
					//	check_step ++;
					ff::para<> a;
					a([this, vt, highThreshold, lowThreshold](){
							for(size_t i = 0; i < vt.size(); ++i)
							{
							pos_t t = vt[i];
							HysteresisPixel(get<0>(t), get<1>(t), highThreshold, lowThreshold);
							}
							});
					pg.add(a);
					vt.clear();
				}
			}
		}
	}
	ff::paragroup pg2;
	pg2.for_each(vt.begin(), vt.end(), [this, lowThreshold, highThreshold](pos_t t)
			{
			HysteresisPixel(get<0>(t), get<1>(t), highThreshold, lowThreshold);
			});
	ff_wait(all(pg));
	ff_wait(all(pg2));
}


