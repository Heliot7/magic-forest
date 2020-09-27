#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <iostream>
#include <math.h>

#define PI 3.14159265f

using namespace std;

// Point - Allows to represent a point in the space
class Point
{
public:
	
	Point(float x = 0.0f, float y = 0.0f, float z = 0.0f);

	// Attributes - Coordinates
	float x, y, z;
};

// Vector - Alloes to represent a vector in the space
class Vector
{
public:
	Vector (float vx=0.0f, float vy=0.0f, float vz=0.0f);
	explicit Vector(const Point &p);

	float  operator[](int i) const;
	float& operator[](int i); 

	// Vector module
	float length() const; 
	// Vector module^2
	float squaredLength() const;

	void normalize();

	Vector static normal(Point *p1, Point *p2, Point *p3)
	{
		Vector n = Vector();
		// Both vector calculated with the 3 face points
		float Px = p2->x - p1->x; 
		float Py = p2->y - p1->y;
		float Pz = p2->z - p1->z;
		float Qx = p3->x - p1->x;
		float Qy = p3->y - p1->y;
		float Qz = p3->z - p1->z;
		// Obtenim el vector normal i el normalitzem
		n.x = Py * Qz - Pz * Qy;
		n.y = Pz * Qx - Px * Qz;
		n.z = Px * Qy - Py * Qx;
		n.x /= n.length();
		n.y /= n.length();
		n.z /= n.length();
		return n;
	}

	// Attributes - Vector components
	float x, y, z;
};


// Basic operations between points and vectors

// P+Q
Point   operator+(const Point&, const Point&);
Point&  operator+=(Point&, const Point&);

// P-Q
Vector operator-(const Point&, const Point&);

// P+v
Point  operator+(const Point&, const Vector &v);
Point& operator+=(Point&, const Vector &v);

// P-v 
Point  operator-(const Point&, const Vector&);
Point& operator-=(Point&, const Vector &v);

// v+u
Vector   operator+(const Vector&, const Vector&);
Vector&  operator+=(Vector&, const Vector&);

// v-u
Vector operator-(const Vector&, const Vector&);
Vector& operator-=(Vector&, const Vector &v);

// aP
Point operator* (const Point&, float f);
Point operator* (float f, const Point&);

// P/a
Point operator/ (const Point&, float f);
Point&operator/=(Point&, float f);

// av
Vector operator* (const Vector&, float f);
Vector operator* (float f, const Vector&);

// v/a
Vector operator/ (const Vector&, float f);
Vector&operator/=(Vector&, float f);


// Writing
ostream& operator<< (ostream &os, const Point &p);
ostream& operator<< (ostream &os, const Vector &p);

#endif

