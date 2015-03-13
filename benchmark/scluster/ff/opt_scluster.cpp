#include "../header.h"
#include "../Lloyd.h"
#include "../point.h"
#include "ff.h"

using namespace ff;

void Lloyd::update(Points & points, int start, int end)
{
  if(!clusters.empty())
  {
    clusters.clear();
    clusters.resize(means.size());
  }
  if(!last_means.empty())
    last_means.clear();
  copy(means.begin(),means.end(),back_inserter(last_means));

  int dimension = means.at(0).dimension;
  Point sum0(dimension);
  std::vector<accumulator<Point> * > accs;
  for(int i = 0; i < means.size(); ++i)
  {
    accs.push_back(new accumulator<Point>(sum0, [](const Point& x, const Point& y)
          {
          return x + y;
          }));
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
    a([&buf_queue, &is_stopped,&points, this, &accs](){
        int t;
        while(!is_stopped || buf_queue.size() != 0)
        {
        if(buf_queue.pop(t)){
        int ci = assignment(points.at(t));
        accs[ci]->increase(points.at(t));
        }
        }
        });
    pg.add(a);
  }

  for(int i = start; i < end; i++)
  {
    if(!buf_queue.push(i))
    {
      int ci = assignment(points.at(i));
      accs[ci]->increase(points.at(i));
    }
  }
  is_stopped = true;
  ff_wait(all(pg));

  for(int i = 0; i < means.size(); ++i)
  {
    means[i] = accs[i]->get()/clusters[i].size();
    delete accs[i];
  }
}
