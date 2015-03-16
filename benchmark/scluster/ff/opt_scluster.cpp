#include "../header.h"
#include "../Lloyd.h"
#include "../point.h"
#include "ff.h"

using namespace ff;
  std::vector<accumulator<Point> * > accs;
  std::vector<accumulator<int> * > accnums;
void init_global(int s, int dimension)
{
  Point sum0(dimension);
  for(int i = 0; i < s; ++i)
  {
    accs.push_back(new accumulator<Point>(sum0, [](const Point& x, const Point& y)
          {
          return x + y;
          }));
	accnums.push_back(new accumulator<int>(0, [](const int& x, const int& y)
          {
          return x + y;
          }));
  }
}

void destory_global(int s)
{
  for(int i = 0; i < s; ++i)
  {
    delete accs[i];
    delete accnums[i];
  }
}

int blockSize = 50;
void Lloyd::update(Points & points, int start, int end)
{
  if(!last_means.empty())
    last_means.clear();
  //copy(means.begin(),means.end(),back_inserter(last_means));

  int dimension = means.at(0).dimension;
  Point sum0(dimension);
  for(int i = 0; i < means.size(); ++i)
  {
	accs[i]->reset(sum0);
	accnums[i]->reset(0);
  }
  //Optimize here!
  typedef ff::rt::simo_queue<int, 8> MQ_t;
  MQ_t buf_queue;
  std::atomic_bool is_stopped(false);
  auto con_size  = ff::rt::rt_concurrency();
  //update
  ff::paracontainer pg;
  is_stopped = false;
  for(int i = 0; i < con_size; ++i)
  {
    ff::para<> a;
    a([&buf_queue, &is_stopped,&points, this](){
        int t;
        while(!is_stopped || buf_queue.size() != 0)
        {
        if(buf_queue.pop(t)){
	for(int j = t*blockSize; j < (t+1) * blockSize && j < points.size(); ++j){

        int ci = assignment(points.at(j));
		accnums[ci]->increase(1);
        accs[ci]->increase(points.at(j));
}
        }
        }
        });
    pg.add(a);
  }

  for(int i = start; i < end; i+= blockSize)
  {
    int t = i/blockSize;
    if(!buf_queue.push(t))
    {
      for(int j = t*blockSize; j <(t+1)*blockSize && j < end; ++j){
      int ci = assignment(points.at(j));
      accs[ci]->increase(points.at(j));}
    }
  }
  is_stopped = true;
  ff_wait(all(pg));

  for(int i = 0; i < means.size(); ++i)
  {
    means[i] = accs[i]->get()/accnums[i]->get();
  }
}
