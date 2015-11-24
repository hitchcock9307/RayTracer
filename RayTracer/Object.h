#ifndef OBJECT_H
#define OBJECT_H


#include <iostream>
#include <glm/glm.hpp>
#include "Intersection.h"

class Object{
private:
public:
	glm::dvec3 md = glm::dvec3(1, .1, .1);
	glm::dvec3 ms = glm::dvec3(1, .1, .1);
	double shininess = 60.0;


	glm::dmat4 xForm;
	glm::dmat4 ixForm;
	glm::dmat4 ixtForm;

	virtual Intersection intersect(Ray ray){ return Intersection(); };
};

#endif