#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <iostream>
#include "Object.h"
#include "Intersection.h"

class Sphere : public Object{
private:
public:
	Intersection intersect(Ray ray){
		Intersection isec;

		glm::vec4 v = ixForm*ray.direction;
		glm::vec4 u = ixForm*ray.point;
		glm::vec3 v3 = glm::vec3(v);
		glm::vec3 u3 = glm::vec3(u);

		float a = glm::dot(v3, v3);
		float b = 2 * glm::dot(u3, v3);
		float c = glm::dot(u3, u3) - 1;


		float b2 = b*b;
		float ac4 = 4 * a*c;

		if (b2 <= ac4) return isec;
		if (b < 0) b = b * -1;

		float root = sqrt(b2- ac4);
		float top = b + root;
		float t1 = top / (2 * a);
		float t2 = c / (a*t1);


		if (t1 < 0){
			if (t2 < 0);
			else{
				glm::vec4 intersectPoint = u + v*t2;
				isec.point = xForm*intersectPoint;
				isec.normal = ixtForm*intersectPoint;
				isec.distance = t2;
				return isec;
			}
		}
		else{
			glm::vec4 intersectPoint = u + v*t1;
			isec.point = xForm*intersectPoint;
			isec.normal = ixtForm*intersectPoint;
			isec.distance = t1;
			return isec;


		}
		return isec;
	}
};

#endif