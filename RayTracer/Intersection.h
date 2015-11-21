#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <glm/glm.hpp>

class Intersection{
private:
public:
	glm::vec4 point;
	glm::vec4 normal;
	glm::vec4 v;
	float distance = -1;
};

#endif