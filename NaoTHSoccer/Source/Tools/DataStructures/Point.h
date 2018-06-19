//Implementation of a Point class with included operator for easy use of convexhull and armcollisiondetector2018
//Note: the operator overload is not used
#ifndef _POINT_H_
#define _POINT_H_


class Point {
private:
	double x;
	double y;
public:
	explicit Point(const double& a, const double& b);
	Point();
	double getX() const;
	double getY() const;
	bool operator < (const Point &p) const;
	bool operator == (const Point &p) const;
};


#endif //_POINT_H
