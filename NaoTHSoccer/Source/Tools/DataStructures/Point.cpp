//
// Created by Etienne Couque on 01.06.2018.
//

#include "Point.h"

Point::Point(const double& a, const double& b) {
	x = a; y = b;
}

Point::Point() {}

double Point::getX() const {
	return x;
}

double Point::getY() const {
	return y;
}

bool Point::operator<(const Point &p) const {
	return x<p.getX() || (x == p.getX() && y<p.getY());
}

bool Point::operator==(const Point &p) const {
	return (x == p.getX()) && (y == p.getY());
}
