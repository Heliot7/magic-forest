#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <QString> 
#include <QDir>

#include "headers/render/obj/box.h"
#include "headers/render/obj/material_lib.h"

using namespace std;

struct Vertex
{
	Vertex(Point _coord) { coord = _coord; }
	Point coord;
	Vector normal;
	vector<int> faces;
};

struct Face
{
	Face() {}
	Face(int i1, int i2, int i3, int i4 = -1)
	{
		vertices.push_back(i1);
		vertices.push_back(i2);
		vertices.push_back(i3);
		if (i4!=-1)
			vertices.push_back(i4);
	}

	// material ID (index) 
	int material;
	// Vector with indexes of vertexs of the face
	vector<int> vertices;
	// Vector with indexes of texture coordenates of the face
	vector<int> texCoords;
	// Normal per face
	Vector normal;
};

// Object - allws to read .obj files
class Object
{
public:
	Object();

	static void setModel(char* path, Object* model)
	{
		QString pathTestObj = QDir::currentPath().append(path);
		if(getenv("windir") != NULL)
			pathTestObj.replace("/", "\\");
		model->readObj(pathTestObj.toAscii().data());
		model->updateBoundingBox();
		model->calculateNormalPerVertex();	
	}

	// Reads the model from and OBJ file
	void readObj(const char* filename);

	// Bounding Box is calculated
	void updateBoundingBox();

	// Getter of Bounding Box
	Box boundingBox() const;

	// Getter of texture Id
	GLuint* getTextureId() { return &textureId; }
	// Setter of texture Id
	void setTextureId(GLuint newTextureId) { textureId = newTextureId; }

	// Getter materials
	MaterialLib* getMatLib() { return &matlib; }

	// Calculate all normals per face from scratch
	void calculateNormalPerFace();

	// Calculate all normals per vertex from scratch
	void calculateNormalPerVertex();

	// Scale factor getters and setters
	float getScale() { return scaleFactor; }
	void setScaleFactor(float scale) { scaleFactor = scale; }
	void updateScaleFactor(float scale) { scaleFactor += scale; }

private:
	void make_face ( char **words, int nwords, int material );


public:
	// Vector with all object vertices
	vector<Vertex>	vertices;
	// Vector with all object faces
	vector<Face>	faces;
	// Vector with all texture coordinates
	vector<Point>	texCoords;

private:
	Box _boundingBox;
	MaterialLib matlib;
	GLuint textureId;
	float scaleFactor;
};


#endif

