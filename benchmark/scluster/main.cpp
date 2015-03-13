#include <iostream>
#include <sstream>
#include <fstream>

#include "Lloyd.h"
#include <chrono>
#include "utils.h"


#define K 3
#define PATHOUT "./"
#define MAXSTEP 100
#define MAXDELTA 0
#define PARASIZE 1000
//#define PARASIZE 25
using namespace std;


Point ReadDataHelper(stringstream &ss) { // used in ReadData()
  double data;
  vector<double> vec;
  while (ss >> data) {
    vec.push_back(data);
  }
  Point point(vec);
  return point;
}

Points ReadData(string fileName) { //reads from file containing each (any dimensional) point in a row. Returns a Points containing them.
  ifstream file;
  Points pointSet;
  double data;
  string line;
  file.open(fileName.c_str());
  if(file.is_open()) {
    while(file.good()) {
      getline(file,line);
      stringstream ss(line);
      Point point = ReadDataHelper(ss);
      if (point.dimension!=0) pointSet.push_back(point); //this removes the end of file line (contains no data)
    }
  }
  else cout << "can't open file" << endl;

  return pointSet;

  file.close();
}

void WriteOutput(Point mean, Points cluster_points, string path) {

  fstream file;
  file.open(path.c_str(), ios::out | ios::trunc);

  if(file.is_open()) {
    file << "Means: " << endl;
    for(int j = 0; j < mean.dimension; j++) {
      file << mean.coordinate[j] << " " ;
    }
    file << endl;

    file << "Points:" << endl;
    for(int i=0; i < cluster_points.size(); i++) {
      for(int j=0; j < mean.dimension; j++) {
        file << cluster_points[i].coordinate[j] << " ";
      }
      file << endl;
    }
  }
  else cout << "cannot open output file" << endl;
  file.close();
}



extern void init_global(int s, int d);
extern void destory_global(int s);
int main(int argc, char *argv[])
{
  RRecord rr("time.json", "kmeans");
  ParamParser pp;
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  bool bIsPara = (thrd_num > 0);
  if(thrd_num > 0)
    initial_para_env(thrd_num);

  int step;
  string fileName = "../../kmeans/tbb/data/gaussian.txt";
  fstream gaussianFile;
  ofstream out_time_file;

  Points points = ReadData(fileName); //loads data from PATH into Points points
  Lloyd oLloyd(points,K);

  int dimension = oLloyd.getMeans().at(0).dimension; 
  start_record_cache_access();
  init_global(K, dimension);

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  for(step = 0; step < MAXSTEP && !oLloyd.isEnd(MAXDELTA); step++) { //Calculates the new means as the centriod of each cluster. Reapting 4 times.
    //Storing it in oLloyd in class Lloyd.
    oLloyd.update(points, 0, points.size());
  }
  end = chrono::system_clock::now();
  destory_global(K);
std::cout<<"step is "<<step<<std::endl;
  auto time = chrono::duration_cast<chrono::microseconds>(end-start).count();
  rr.put("elapsed-time", time);

  end_record_cache_access(rr);


  return 0;
}
