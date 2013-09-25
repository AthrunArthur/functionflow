#ifndef LLOYD_H
#define LLOYD_H

#include "point.h"
#include "ff.h"


using namespace ff;
using namespace std;

class Lloyd { //each object of this class contains a vector of k-means, and their respective clusters
	public:
		Lloyd(vector<Point> points, int k, bool bIsPara = false); //constructs an object with int random points vector<Point> and use them as the means
		~Lloyd();
		
		
		bool isEnd(double delta); // Judge whether the cluster is convergent.
		void assignment(Point data); //assigns a point to cluster[i], where means[i] is closest to Point among all means.
		void update(vector<Point> points); // reassigns data points from vector<Point> to class member clusters given new means
		vector<Point> & getMeans();
		vector< vector<Point> > & getClusters();		
	private:
		vector<Point> means; // contains K means		
		vector< vector<Point> > clusters; // contains K clusters
		vector<Point> last_means; // contains last K means
		bool isPara;
		void updateMean(); //this updates the mean given the class member clusters
		int findMinIndex(Point data); //this finds the closest mean in class member means to Point and returns the corresponding index in means 
};

#endif
