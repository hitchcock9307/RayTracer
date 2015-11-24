#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>
#include "Object.h"
class Object;
class Intersection{
private:
public:
	Intersection(){
		obj = NULL;
	}

	Object *obj;
	glm::dvec4 point;
	glm::dvec4 normal;
	glm::dvec4 v;
	float distance = -1;
};

#endif