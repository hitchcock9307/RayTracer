#ifndef SCENE_H
#define SCENE_H

#include "Viewport.h"
#include "Sphere.h"
#include "Light.h"

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

class Scene{
private:
public:
	Viewport view;
	std::vector<Object*> objects;
	std::vector<Light*> lights;

	glm::dvec3 backColor;
	glm::dvec3 ambColor = glm::dvec3(0,0,0);

	double delta = .001;
	int maxDepth = 10000;

	Scene(){
	}

	void load(std::string filename){

		std::ifstream file(filename);

		std::string line;

		glm::dmat4 xForm = glm::dmat4(1.0);
		double shininess = 0;
		glm::dvec3 diff, spec;

		std::vector<glm::dmat4> groups(200);
		groups.push_back(xForm);
		int currentGroup = 0;


		while (!file.eof()){
			getline(file, line);
			std::vector<std::string> args;
			std::stringstream ss(line);
			std::string token;

			//std::cout << "Line!\n";
			while (std::getline(ss, token, ' ')){
				if (token.length() == 0) continue;
				args.push_back(token);
			}
			if (args.size() == 0) continue;
			

			if (!args[0].compare("view")){
				double n = atof(args[1].c_str());
				double d = atof(args[2].c_str());
				view.height = n;
				view.width = n;
				view.recalculate();
				view.d = d*2;

			}
			else if (!args[0].compare("sphere")){
				Sphere* sphere = new Sphere();
				sphere->xForm = groups[currentGroup];
				sphere->ixForm = glm::inverse(groups[currentGroup]);
				sphere->ixtForm = glm::transpose(sphere->ixForm);

				sphere->md = diff;
				sphere->ms = spec;
				sphere->shininess = shininess;

				objects.push_back(sphere);

			}
			else if (!args[0].compare("scale")){
				double sx = atof(args[1].c_str());
				double sy = atof(args[2].c_str());
				double sz = atof(args[3].c_str());
				groups[currentGroup] = glm::scale(groups[currentGroup], glm::dvec3(sx, sy, sz));

			}
			else if (!args[0].compare("move")){
				double tx = atof(args[1].c_str());
				double ty = atof(args[2].c_str());
				double tz = atof(args[3].c_str());
				groups[currentGroup] = glm::translate(groups[currentGroup], glm::dvec3(tx, ty, tz));
			}
			else if (!args[0].compare("rotate")){
				double angle = atof(args[1].c_str());
				double rx = atof(args[2].c_str());
				double ry = atof(args[3].c_str());
				double rz = atof(args[4].c_str());
				groups[currentGroup] = glm::rotate(groups[currentGroup], (double)glm::radians(angle), glm::dvec3(rx, ry, rz));

			}
			else if (!args[0].compare("light")){
				double r = atof(args[1].c_str());
				double g = atof(args[2].c_str());
				double b = atof(args[3].c_str());
				double x = atof(args[4].c_str());
				double y = atof(args[5].c_str());
				double z = atof(args[6].c_str());
				Light *l = new Light();
				l->color = glm::dvec3(r, g, b);
				l->pos = glm::dvec4(x, y, z, 1);
				lights.push_back(l);

			}
			else if (!args[0].compare("background")){
				double r = atof(args[1].c_str());
				double g = atof(args[2].c_str());
				double b = atof(args[3].c_str());
				backColor = glm::dvec3(r, g, b);

			}
			else if (!args[0].compare("ambient")){
				double r = atof(args[1].c_str());
				double g = atof(args[2].c_str());
				double b = atof(args[3].c_str());
				ambColor = glm::dvec3(r, g, b);

			}
			else if (!args[0].compare("group")){
				groups[currentGroup+1] = groups[currentGroup];
			    currentGroup++;
			}
			else if (!args[0].compare("groupend")){
				//groups.pop_back();
				currentGroup--;
			}
			else if (!args[0].compare("material")){
				double dr = atof(args[1].c_str());
				double dg = atof(args[2].c_str());
				double db = atof(args[3].c_str());
				double sr = atof(args[4].c_str());
				double sg = atof(args[5].c_str());
				double sb = atof(args[6].c_str());
				double p = atof(args[7].c_str());

				shininess = p;
				diff = glm::dvec3(dr, dg, db);
				spec = glm::dvec3(sr, sg, sb);
			}
			else if (!args[0].compare("refraction")){

			}
		}


	}

};

/*
#	A line that begins with a # is a comment line, ignore the entire line.
view n d	The scene should be nXn pixels. d is the distance to the corner of the image plane. The perspective camera is always located at (0,0,1) and points toward the origin with (0,1,0) being the up vector. The image plane therefore runs from (-d,-d,0) to (d,d,0).
sphere	draw a sphere using the current material and transformed by the current transformation.
scale sx sy sz	Adds a scale by sx sy sz to the current transformation.
move x y z	Adds a translate to x y z to the current transformation.
rotate angle x y z	Adds a rotate angle degrees about the axis defined by the vector (x y z)
light r g b x y z	Define a light located at (x y z) with color (r g b)
background r g b	Set the scene background color to (r g b) , the default is black (0,0,0)
ambient r g b	Set the scene ambient light to (r g b).
group	group the following objects, transforms and materials together.
groupend	Signifies the end of a group.
material dr dg db sr sg sb p	Sets the current material value to have a diffuse color of (dr,dg,db) a specular color of (sr,sg,sb) and a phong highlight coefficient of p. The material becomes part of the state and all subsequent objects are of this material.
refraction r g b i	Sets the refraction coefficients for the current material. Default is (0 0 0 1.0002926). The default index for the scene will be 1.0002926 (air). This becomes part of the state, just like material.*/


#endif