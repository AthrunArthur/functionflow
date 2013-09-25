#include <iostream>
#include <sstream>
#include <fstream>

#include "Lloyd.h"
#include <chrono>

#define K 3
#define PATHOUT "./"
#define MAXSTEP 100
#define MAXDELTA 0
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

vector<Point> ReadData(string fileName) { //reads from file containing each (any dimensional) point in a row. Returns a vector<Point> containing them.
    ifstream file;
    vector<Point> pointSet;
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

void WriteOutput(Point mean, vector<Point> cluster_points, string path) {

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

int main(int argc, char *argv[])
{
    int step;
    string fileName = "../kmeans/tbb/data/gaussian.txt";
    fstream gaussianFile;
    bool bIsPara = false;
    ofstream out_time_file;
    if(argc > 1) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[1];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;
    }

    vector<Point> points = ReadData(fileName); //loads data from PATH into vector<Point> points

    Lloyd oLloyd(points,K);	//chooses K points randomly from the vector points as starting means

    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    for(step = 0; step < MAXSTEP && !oLloyd.isEnd(MAXDELTA); step++) { //Calculates the new means as the centriod of each cluster. Reapting 4 times.
        //Storing it in oLloyd in class Lloyd.
        oLloyd.update(points);
    }

    end = chrono::system_clock::now();
    int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
    cout << "elapsed time: " << elapsed_seconds << "us" << endl;

//     cout << "Steps: " << step << endl;

    //Writes the updated means and clusters to files -- USEFUL!
//     for(int i = 0; i < K; i++) { 
//         ostringstream fn;
//         fn << PATHOUT << K << "cluster" << i <<  "_step"<< step <<".txt";
//         string s = fn.str();
//         WriteOutput(oLloyd.getMeans().at(i), oLloyd.getClusters().at(i), s);
//     }
    
    if(bIsPara) {
        // write para time file
        out_time_file.open("para_time.txt",ios::app);
        if(!out_time_file.is_open()) {
            cout << "Can't open the file para_time.txt" << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }
    else {
        // write seq time file
        out_time_file.open("time.txt");
        if(!out_time_file.is_open()) {
            cout << "Can't open the file time.txt" << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }

    return 0;
}
