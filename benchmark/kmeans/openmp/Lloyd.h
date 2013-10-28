#ifndef LLOYD_H
#define LLOYD_H

#include "point.h"


using namespace std;

class Lloyd { //each object of this class contains a vector of k-means, and their respective clusters
	public:
		Lloyd(Points & points, int k, bool bIsPara = false); //constructs an object with int random points Points and use them as the means
		Lloyd(Points & omeans, bool bIsPara = false);//constructs an object with omeans provided
		~Lloyd();
		
		
		bool isEnd(double delta); // Judge whether the cluster is convergent.
		void assignment(Point data); //assigns a point to cluster[i], where means[i] is closest to Point among all means.
		void update(Points & points, int start, int end); // reassigns data points from Points to class member clusters given new means
		Points & getMeans();
		void setMeans(Points & omeans);
		vector< Points > & getClusters();		
	private:
		Points means; // contains K means		
		vector< Points > clusters; // contains K clusters
		Points last_means; // contains last K means
		bool isPara;
		void updateMean(); //this updates the mean given the class member clusters
		int findMinIndex(Point data); //this finds the closest mean in class member means to Point and returns the corresponding index in means 
};

#endif
