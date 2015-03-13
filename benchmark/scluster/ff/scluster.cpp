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

  ff::paracontainer pc;
  for(int i = start; i < end; i++)
  {
    ff::para<> a;
    a([this, &points, i, &accs](){
        std::cout<<"i : "<<i<<std::endl;
        int ci = assignment(points.at(i));
        accs[ci]->increase(points.at(i));
        });
    pc.add(a);
  }

  ff_wait(all(pc));

  for(int i = 0; i < means.size(); ++i)
  {
    means[i] = accs[i]->get()/clusters[i].size();
    delete accs[i];
  }
}
