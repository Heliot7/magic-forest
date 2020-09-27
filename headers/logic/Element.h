#ifndef ELEMENT_H
#define ELEMENT_H

#include <iostream>
#include <string>
#include <vector>
#include <QString>
#include <QDir>

#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"

#include "headers/render/Geometry.h"
#include "headers/render/obj/object.h"
#include "headers/render/obj/material_lib.h"

struct CollisionCallback : public btCollisionWorld::ContactResultCallback
{
	bool collision; 
        virtual btScalar addSingleResult(btManifoldPoint& cp,
		const btCollisionObject* colObj0, int partId0, int index0,
		const btCollisionObject* colObj1, int partId1, int index1)
	{
		collision = true;
		return 0;
	}
};

class Element
{
public:

	Element(Point aPos = Point(), Vector aDir = Vector())
	{
		position = aPos; direction = aDir;
		body = NULL;
		scale = Point(1.0f,1.0f,1.0f);
		type = 0;
	}
	~Element() {}

	// Getters
	Point getPos() { return position; }
	Vector getDir() { return direction; }
	Object* getModel() { return model; }
	GLuint* getTextureId() { return model->getTextureId(); }
	Box getBB() { return model->boundingBox(); }
	MaterialLib* getMaterials() { return model->getMatLib(); }
	btRigidBody* getBody() { return body; }
	Point getScale() { return scale; }
	int getType() { return type; }
	// Setters
	void setPos(Point aPos) { position = aPos; }
	void setDir(Vector aDir) { direction = aDir; }
	void setScale(Point scale) { this->scale = scale; }
	void updatePos(Vector offsetPos) { position += offsetPos; }
	void updatePosX(float x) { position.x = x; }
	void updatePosY(float y) { position.y = y; }
	void updatePosZ(float z) { position.z = z; }
	void setModel(Object* obj) { model = obj; }
	void setModel(char* path)
	{
		QString pathTestObj = QDir::currentPath().append(path);
		if(getenv("windir") != NULL)
			pathTestObj.replace("/", "\\");
		model = new Object();
		model->readObj(pathTestObj.toAscii().data());
		model->updateBoundingBox();
		model->calculateNormalPerVertex();
	}
	void setBody(btScalar mass, btDefaultMotionState* myMotionState, btCollisionShape* shape, btVector3& localInertia)
	{
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		body = new btRigidBody(rbInfo);
	}
	void setType(int type) { this->type = type; }

protected:

	Point position;
	Vector direction;
	Object* model;
	btRigidBody *body;
	Point scale;
	int type;
};

#endif
