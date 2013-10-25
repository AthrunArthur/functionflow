#include "Lloyd.h"

#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

Lloyd::Lloyd(Points & points, int k, bool bIsPara):isPara(bIsPara)
{
    Points randomPoints;
    for(int i = 0; i < k; i++) {
        srand(time(NULL)+i);					//crappy random number generator. But here it suffices.
        int random1 = rand() % points.size();
        randomPoints.push_back(points.at(random1));
    }
    means = randomPoints;
    clusters.resize(means.size());
}

Lloyd::Lloyd(Points & omeans, bool bIsPara):isPara(bIsPara)
{
    for(int i = 0; i < omeans.size(); i++) {
        means.push_back(omeans.at(i));
    }
    clusters.resize(means.size());
}

Lloyd::~Lloyd()
{
}

void Lloyd::setMeans(Points & omeans)
{
    if(!means.empty()) {
        if(!last_means.empty())
            last_means.clear();
        for(int i = 0; i < means.size(); i++) {
            last_means.push_back(means.at(i));
        }
        means.clear();
    }
    for(int i = 0; i < omeans.size(); i++) {
        means.push_back(omeans.at(i));
    }
}


void Lloyd::updateMean() {
    int means_size = means.size();
    int dimension = means.at(0).dimension;
//         Points new_means;// parallel_reduce check
    if(!last_means.empty())
        last_means.clear();
    for(int i = 0; i < means.size(); i++) {
        last_means.push_back(means.at(i));
    }

//     if(isPara) {
//         vector<int> iVec;
//         for(int i = 0; i < means_size; i++) {
//             iVec.push_back(i);
//         }
//         ff::paragroup pp;
//         pp.for_each(iVec.begin(),iVec.end(),[this,dimension](int i) {
// // 	    Without parallel_reduce
// // 	    Point sum(dimension);
// //             int cluster_size = clusters[i].size();
// //             for(int j = 0; j < cluster_size; j++) {
// //                 sum += clusters[i].at(j);
// //             }
// //             means[i] = sum/cluster_size;
//
// // 	    With parallel_reduce : data_wrapper.h altered!
//             int cluster_size = clusters[i].size();
//             Point sum0(dimension);
//             ff::accumulator<Point> sum(sum0, [](const Point & x, const Point & y) {
//                 return x + y;
//             });
//             ff::paragroup pg1;
//             pg1.for_each(clusters[i].begin(), clusters[i].end(), [&sum](Point x) {
//                 sum.increase(x);
//             });
//             ff_wait(all(pg1));
//             means[i] = sum.get()/cluster_size;
//
//         });
//         ff_wait(all(pp));
//     }
//     else {
    means.clear();
    for (int i = 0; i < means_size; i++) {
        Point sum(dimension);
        int cluster_size = clusters[i].size();
        for(int j = 0; j < cluster_size; j++) {
            sum += clusters[i].at(j);
        }
//          new_means.push_back(sum/cluster_size);
        means.push_back(sum/cluster_size);
    }
//     }

}

void Lloyd::update(Points & points, int start, int end) {
    if(!clusters.empty()) {
        clusters.clear();
        clusters.resize(means.size());
    }
//     if(isPara)
//     {
//         ff::paragroup pg;
//         pg.for_each(points.begin(),points.end(),[this](Point data) {
//             assignment(data);
//         });
//         vector<int> ivec;
//         for(int i = 0; i  < points.size(); i++) {
//             ivec.push_back(i);
//             if(ff::is_idle() || (i == points.size() - 1)) {
//                 ff::para<void> p;
//                 p([this, ivec, points]() {
//                     ff::paragroup pp;
//                     pp.for_each(ivec.begin(), ivec.end(), [this, points](int i) {
//                         assignment(points[i]);
//                     });
//                     ff_wait(all(pp));
//                 });
//                 pg.add(p);
//                 ivec.clear();
//             }
//         }
//         ff_wait(all(pg));
//     }
//     else {
    for(int i = start; i < end; i++) {
        assignment(points.at(i));
    }
//     }
    updateMean();
}

bool Lloyd::isEnd(double delta)
{
    bool retVal = true;
    if(last_means.empty())
        return false;
    for(int i = 0; i < means.size(); i++) {
        double distance = means.at(i).distanceSquared(last_means.at(i));
        if(distance >= delta * delta) {
            retVal = false;
            break;
        }
    }
    return retVal;
}

vector< Points > & Lloyd::getClusters()
{
    return clusters;
}

Points & Lloyd::getMeans()
{
    return means;
}


int Lloyd::findMinIndex(Point data) {
    double min_distance = data.distanceSquared(means.at(0));
    int mean_index = 0;
    for(int i = 1; i < means.size(); i++) {
        double distance = data.distanceSquared(means.at(i));
        if(distance < min_distance) {
            min_distance = distance;
            mean_index = i;
        }
    }
    return mean_index;
}
void Lloyd::assignment(Point data) {
    int mean_index = findMinIndex(data);
    clusters[mean_index].push_back(data);
}
