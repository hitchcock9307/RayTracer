#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>

class Object{
private:
public:
	virtual float intersect(Ray ray, glm::vec3 &color) {
		return 0;
	}
};

#endif