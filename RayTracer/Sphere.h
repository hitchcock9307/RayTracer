#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>
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

		float a = glm::dot(v, v);
		float b = 2 * glm::dot(u, v);
		float c = glm::dot(u3, u3) - 1;


		float b2 = b*b;
		float ac4 = 4 * a*c;

		if (b2 <= ac4) return isec;


		float root = sqrt(b2 - ac4);
		float top;

		if (b > 0) top = -b - root;
		else top = -b + root;

		float t1 = top / (2 * a);
		float t2 = c / (a*t1);

		float tUse = -1;

		if (t1 < 0)
			if (t2 < 0);
			else tUse = t2;
		else
			if (t2 < 0) tUse = t1;
			else tUse = std::min(t1, t2);

		if (tUse != -1) {
			glm::vec4 intersectPoint = glm::vec4(u3 + v3*tUse, 1.0);
			isec.point = xForm*intersectPoint;
			isec.normal = ixtForm*intersectPoint;
			isec.v = v;
			isec.distance = t2;
			return isec;
		}

		return isec;
	}
};

#endif