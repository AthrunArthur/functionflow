#include <iostream>
#include "point.h"
#include <vector>



using namespace std;


Point::Point() {
}

Point::Point(int n) {
    dimension = n;
    coordinate.resize(n);
}
Point::Point(vector<double> input_coordinate) {
    dimension = input_coordinate.size();
    for (int i = 0; i < dimension; i++) {
        coordinate.push_back(input_coordinate[i]);
    }
}

double Point::distanceSquared(Point p2) {
    if( p2.dimension != dimension) {
        cout << "Dimensions do not match" << endl;
        return 0;
    }
    else {
        double sum = 0;
        for (int i = 0; i < dimension; i++) {
            sum += (coordinate[i] - p2.coordinate[i])*(coordinate[i] - p2.coordinate[i]);
        }
        return sum;
    }
}

const Point operator+(const Point& leftP, const Point& rightP)
{
    if( leftP.dimension != rightP.dimension) {
        cout << "Dimensions do not match" << endl;
        return Point(leftP.dimension);
    }
    else {
        vector<double> vec;
        for (int i = 0; i < leftP.dimension; i++) {
            vec.push_back(leftP.coordinate[i] + rightP.coordinate[i]);
        }
        Point sum(vec);
        return sum;
    }
}

Point& operator+=(Point& leftP, const Point rightP)
{
    if( leftP.dimension != rightP.dimension) {
        cout << "Dimensions do not match" << endl;
        return leftP;
    }
    else {
        for (int i = 0; i < leftP.dimension; i++) {
            leftP.coordinate[i] = leftP.coordinate[i] + rightP.coordinate[i];
        }
        return leftP;
    }
}

const Point operator/(const Point& p, const double n)
{
    vector<double> vec;
    for (int i = 0; i < p.dimension; i++) {
        vec.push_back(p.coordinate[i]/n);
    }
    Point p3(vec);
    return p3;
}

const Point operator*(const Point& p, const double n)
{
    vector<double> vec;
    for (int i = 0; i < p.dimension; i++) {
        vec.push_back(p.coordinate[i]*n);
    }
    Point p3(vec);
    return p3;
}

