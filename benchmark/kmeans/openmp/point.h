#ifndef POINT_H
#define POINT_H

#include <vector>

using namespace std;

class Point {
	public:
		Point(); // It's always good idea to explicitly write a default constructor when other contructors exist
		Point(int); //Makes an object with dimension int and a zero vector as the coordinate
		Point(vector<double>); // constructs a point with coordinates specified by vector<double>

		double distanceSquared(Point);	//calculates the Euclidean distance squared to Point

		int dimension;		
		vector<double> coordinate;

		friend const Point operator+(const Point & leftP, const Point & rightP);
		friend const Point operator/(const Point & p, const double n);
		friend const Point operator*(const Point & p, const double n);
		friend Point & operator+=(Point & leftP, const Point rightP);
		
	private:
};

typedef vector<Point> Points;
#endif
