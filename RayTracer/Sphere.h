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

		glm::dvec4 v = ixForm*ray.direction;
		glm::dvec4 u = ixForm*ray.point;
		glm::dvec3 v3 = glm::dvec3(v);
		glm::dvec3 u3 = glm::dvec3(u);

		double a = glm::dot(v, v);
		double b = 2 * glm::dot(u, v);
		double c = glm::dot(u3, u3) - 1;


		double b2 = b*b;
		double ac4 = 4 * a*c;

		if (b2 <= ac4) return isec;


		double root = sqrt(b2 - ac4);
		double top;

		if (b > 0) top = -b - root;
		else top = -b + root;

		double t1 = top / (2 * a);
		double t2 = c / (a*t1);

		double tUse = -1;

		if (t1 < 0)
			if (t2 < 0);
			else tUse = t2;
		else
			if (t2 < 0) tUse = t1;
			else tUse = std::min(t1, t2);

		if (tUse != -1) {
			glm::dvec4 intersectPoint = glm::dvec4(u3 + v3*tUse, 1.0);
			isec.point = xForm*intersectPoint;
			isec.normal = ixtForm*intersectPoint;
			isec.v = v;
			isec.obj = this;
			isec.distance = t2;
			return isec;
		}

		return isec;
	}
};

#endif