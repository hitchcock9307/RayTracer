#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <glm/glm.hpp>
#include "Intersection.h"

class Object{
private:
public:
	glm::vec3 md = glm::vec3(1, .1, .1);
	glm::vec3 ms = glm::vec3(1, .1, .1);
	float shininess = 60.0;


	glm::mat4 xForm;
	glm::mat4 ixForm;
	glm::mat4 ixtForm;

	virtual Intersection intersect(Ray ray){ return Intersection(); };
};

#endif