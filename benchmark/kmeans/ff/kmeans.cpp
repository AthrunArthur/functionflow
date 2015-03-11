#include "ff.h"
#include "../point.h"
#include "../Lloyd.h"
#include "../header.h"

using namespace std;
using namespace ff;

void kmeans(Points & points, bool isPara)
{
  Lloyd mLloyd(points,K,isPara);
  int blockSize = points.size()/PARASIZE + ((points.size()%PARASIZE)? 1 : 0);
  cout << "blockSize="<< blockSize << endl;
  vector<Lloyd> vecLloyd;
  //initialize
  for(int i = 0; i < blockSize; i++)
  {
    Lloyd iLloyd(mLloyd.getMeans(),isPara);
    vecLloyd.push_back(iLloyd);
  }
  //update
  for(int step = 0; step < MAXSTEP; step++) {
    if(isPara) {
      ff::paracontainer pg;
      for(int i = 0; i < blockSize; i++)
      {
        ff::para<void> p;
        p([&vecLloyd,&points,blockSize,i](){
            int start = i * PARASIZE,end;
            if(i == blockSize -1)
            end = points.size() - 1;
            else
            end = start + PARASIZE - 1;
            vecLloyd[i].update(points,start,end);
            });
        pg.add(p);
      }

      ff_wait(all(pg));
    }
    else {
      for(int i = 0; i< blockSize; i++)
      {
        int start = i * PARASIZE,end;
        if(i == blockSize -1)
          end = points.size() - 1;
        else
          end = start + PARASIZE - 1;
        vecLloyd[i].update(points,start,end);
      }
    }
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
