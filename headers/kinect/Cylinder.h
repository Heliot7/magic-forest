#ifndef __CYLINDER_H__
#define __CYLINDER_H__

#include <math.h>
#include <GL/glut.h>

void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric);
void renderCylinder_convenient(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions);

#endif
