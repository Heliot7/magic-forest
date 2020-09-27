#include <fstream>

#include "headers/render/obj/material_lib.h"

MaterialLib::MaterialLib()
{
	// Adds a material by default to position 0
	dict["default material"]=0;
	materials.push_back(Material("default material"));
	// Adds a 2nd material (red) at position 1
	dict["red material"]=1;
	materials.push_back(Material("red material", Color(.4,0,0),Color(.8,0,0))); 

}

void MaterialLib::readMtl(const char* filename)
{
	ifstream f(filename); 	

	if (f.fail()) 
	{
		cerr << "mtl file cannot be opened <" << filename << ">" << endl;
	}
	else
	{
		Material* mat=0;
		string cad;
		f >> cad;
		while (!f.eof())
		{
			if (cad=="newmtl") 
			{
				// Creates a new material
				string name;
				f >> name;
				dict[name]=materials.size();
				materials.push_back(Material(name));
				mat = &materials[materials.size()-1];
			}
			else if (cad=="Ka")
			{
				f >> mat->ka.r >> mat->ka.g >> mat->ka.b; 
			}
			else if (cad=="Kd")
			{
				f >> mat->kd.r >> mat->kd.g >> mat->kd.b; 
			}
			else if (cad=="Ks")
			{
				f >> mat->ks.r >> mat->ks.g >> mat->ks.b; 
			}
			else if (cad=="d")
			{
				float alfa;
				f >> alfa;
				mat->ka.a = mat->kd.a = mat->ks.a = alfa;
			}
			else if (cad=="Ns")
			{
				f >> mat->shininess;
			}

			f >> cad;
		}
	}
}

Material MaterialLib::material(int index)
{
	if (index >=0 && index < int(materials.size()))
		return materials[index];
	else return materials[0];
}

int MaterialLib::index(const string& name) const
{
	return dict[name];
}


