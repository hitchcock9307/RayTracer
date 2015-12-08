#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <iostream>
#include <algorithm>
#include "Object.h"
#include "Intersection.h"

extern glm::dmat4 globalTransform;

class Sphere : public Object{
private:
public:
	Intersection intersect(Ray ray){
		Intersection isec;

		//Create convenience variables (ignore speed :P)
		glm::dvec4 v = ixForm*globalTransform*ray.direction;
		glm::dvec4 u = ixForm*globalTransform*ray.point;
		glm::dvec3 v3 = glm::dvec3(v);
		glm::dvec3 u3 = glm::dvec3(u);

		//Calculate abcd per slides
		double a = glm::dot(v3, v3);
		double b = 2.0 * glm::dot(u3, v3);
		double c = glm::dot(u3, u3) - 1;
		double d = b*b - 4 * a*c;

		//Per slides, get correct t0 and t1 values
		double t0, t1, tUse = -1;
		if (d <= 0.0) return isec;
		if (b > 0) t0 = (-b - sqrt(d)) / (2.0 * a);
		else t0 = (-b + sqrt(d)) / (2.0 * a);
		t1 = c / (a*t0);

		//Find smallest non-zero

		//t0 neg t1 neg -> doesnt matter
		//t0 pos t1 neg -> return t0
		//t0 neg t1 pos -> return t1
		//t0 pos t1 pos -> return min(t0,t1)
		if (t0 < 0)
			if (t1 < 0);
			else tUse = t1;
		else
			if (t1 < 0) tUse = t0;
			else tUse = std::min(t0, t1);

		//if neg, return and let outer loop continue
		if (tUse != -1) {
			glm::dvec4 intersectPoint = glm::dvec4(u3 + v3*tUse, 1.0); //Calc hit point
			isec.point = xForm*intersectPoint; // transform back (no scaling issues)
			isec.normal = ixtForm*intersectPoint; //inverse transpoe transform back (because normal scales differently)
			isec.v = v; //Need the view vector (really the negative but meh)
			isec.obj = this; //Pass me back
			isec.distance = tUse; // distance to compare closest object
			return isec;
		}

		return isec;
	}
};

#endif