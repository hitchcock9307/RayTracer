#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <iostream>
#include "Object.h"

class Sphere : public Object{
private:
public:
	glm::vec3 sphereColor;
	glm::mat4 xForm = glm::mat4(1);
	float intersect(Ray ray, glm::vec3 &color){
		glm::mat4 ixForm = glm::inverse(xForm);
		glm::vec4 v = ixForm*ray.direction;
		glm::vec4 u = ixForm*ray.point;

		float a = glm::dot(glm::vec3(v), glm::vec3(v));
		float b = 2 * glm::dot(glm::vec3(u), glm::vec3(v));
		float c = glm::dot(glm::vec3(u), glm::vec3(u)) - 1;

		//std::cout << "calling intersect!\n";

		float b2 = b*b;
		float ac4 = 4 * a*c;

		if (b2 <= ac4) return -1;
		if (b < 0) b = b * -1;

		float root = sqrt(b2- ac4);
		float top = b + root;
		float t1 = top / (2 * a);
		float t2 = c / (a*t1);


		if (t1 < 0){
			if (t2 < 0);
			else{
				glm::vec4 intersectPoint = u + v*t2;
				color = glm::vec3(glm::transpose(ixForm)*intersectPoint);
				return t2;
			}
		}
		else{
			glm::vec4 intersectPoint = u + v*t1;
			color = glm::vec3(glm::transpose(ixForm)*intersectPoint);
			return t1;
		}
		return -1;
	}
};

#endif