#include "ff.h"
#include "../point.h"
#include "../Lloyd.h"
#include "../header.h"

void kmeans(Points & points, bool)
{
  Lloyd mLloyd(points,K);
  int blockSize = points.size()/PARASIZE + ((points.size()%PARASIZE)? 1 : 0);
  cout << "blockSize="<< blockSize << endl;
  vector<Lloyd> vecLloyd;
  //initialize
  for(int i = 0; i < blockSize; i++)
  {
    Lloyd iLloyd(mLloyd.getMeans());
    vecLloyd.push_back(iLloyd);
  }
  //Optimize here!
  typedef ff::rt::simo_queue<int, 8> MQ_t;
  MQ_t buf_queue;
  std::atomic_bool is_stopped(false);
  auto con_size  = ff::rt::rt_concurrency();
  //update
  for(int step = 0; step < MAXSTEP; step++) {
    ff::paracontainer pg;
    is_stopped = false;
    for(int i = 0; i < con_size; ++i)
    {
      ff::para<> a;
      a([&buf_queue, &is_stopped, con_size, blockSize, &points, &vecLloyd](){
          int t;
          while(!is_stopped || buf_queue.size() != 0)
          {
          if(buf_queue.pop(t)){
          int start = t * PARASIZE,end;
          if(t == blockSize -1)
          end = points.size() - 1;
          else
          end = start + PARASIZE - 1;
          vecLloyd[t].update(points,start,end);
          }
          }
          });
      pg.add(a);
    }

    for(int i=0; i < blockSize; i++)
    {
      if(!buf_queue.push(i))
      {
        int start = i * PARASIZE,end;
        if(i == blockSize -1)
          end = points.size() - 1;
        else
          end = start + PARASIZE - 1;
        vecLloyd[i].update(points,start,end);
      }
    }
    is_stopped = true;
    ff_wait(all(pg));
    Points new_means;
    for(int j = 0; j < mLloyd.getMeans().size(); j++) {
      Point sum(mLloyd.getMeans().at(0).dimension);
      int total_n = 0;
      for(int k=0; k<vecLloyd.size(); k++) {
        int size = vecLloyd[k].getClusters().at(j).size();
        sum += vecLloyd[k].getMeans().at(j) * size;
        total_n += size;
      }
      new_means.push_back(sum/total_n);
    }
    mLloyd.setMeans(new_means);
    new_means.clear();
    for(int i = 0; i < blockSize; i++)
    {
      vecLloyd[i].setMeans(mLloyd.getMeans());
    }
  }
}
