#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "Ray.h"

#include <vector>
#include <glm/vec3.hpp>


class Viewport{
private:
	std::vector<glm::dvec3> pixels;

public:
	double d = .5;
	int width = 1000, height = 1000;
	int clipDistance = 10000000;

	glm::dvec3 backColor = glm::dvec3(0.0, 0.0, 1.0);

	Viewport(){
		pixels.resize(width * height);
	}

	void recalculate(){ pixels.resize(width*height); }

	glm::dvec3 getPixel(int x, int y){
		return pixels[x*width + y];
	}

	void setPixel(int x, int y, glm::dvec3 input){
		pixels[x*width + y] = input;
	}
};

#endif