#ifndef _BOX_H_
#define _BOX_H_

#include <GL/glut.h>

#include "../headers/render/Geometry.h"

#ifdef min 
#undef min
#endif

#ifdef max 
#undef max
#endif

// Box - permet representar una capsa aliniada amb els eixos (ex. capsa englobant)
class Box
{
public:

	// Constructor
	Box(const Point& minimum=Point(), const Point& maximum=Point());

	// Update - Expands the bounding box to include "p"
	void update(const Point& p);
	
	// Borders of the Box
	Point min, max;	

	// Returns the distance between the 2 most separated points of the BB
	float max_dist();
	// Returns the distance in XYZ-axis from max to min points
	float distX() { return abs(max.x - min.x); }
	float distY() { return abs(max.y - min.y); }
	float distZ() { return abs(max.z - min.z); }
	// Bounding Box center of the base
	Point base_centre();
	// Center of Bounding Box (min point is 0,0,0)
	Point centre_box_local();
	// Center of Bounding Box
	Point centre_box();
	// Draws the Bounding Box with OpenGL commands
	void drawBB();
};

#endif
