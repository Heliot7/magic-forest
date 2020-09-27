#ifndef _MATERIAL_LIB_H_
#define _MATERIAL_LIB_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// RGA Colour definition
struct Color
{
	Color(float ar = 0.0f, float ag = 0.0f, float ab = 0.0f, float aa = 1.0f)
	{
		r = ar; g = ag; b = ab; a = aa;
	}

	float r, g, b, a;
};

// Material definition with ambient, diffuse, specular colours and shininess (identified by a "name")
struct Material
{
	Material(string _name = "default", 
		const Color& _ka = Color(0.2f, 0.2f, 0.2f), 
		const Color& _kd = Color(0.8f, 0.8f, 0.8f), 
		const Color& _ks = Color(1.0f, 1.0f, 1.0f), 
		float _shininess = 10.0f)
	{
		name = _name;
		ka = _ka;
		kd = _kd;
		ks = _ks;
		shininess = _shininess;
	}

	Color ka, kd, ks;
	float shininess;

	string name;
};

class MaterialLib
{
public:

	MaterialLib();

	// Read the materials of a file .mtl, adding them to the library
	void readMtl(const char* filename);

	// returns the material that corresponds to the given index
	// if there is not material in that index, default material is returned (index 0)
	Material material(int index);

	// Given a material name, returns its index
	int index(const string& name) const; 

private:

	// List of materials
	vector<Material> materials;
	mutable map<string, int> dict;

};

#endif

