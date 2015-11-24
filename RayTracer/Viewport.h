#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "Ray.h"

#include <vector>
#include <glm/vec3.hpp>


class Viewport{
private:
	std::vector<glm::vec3> pixels;

public:
	float d = .5;
	int width = 1000, height = 1000;
	int clipDistance = 10000000;

	glm::vec3 backColor = glm::vec3(0.0f, 0.0f, 1.0f);

	Viewport(){
		pixels.resize(width * height);
	}

	void recalculate(){ pixels.resize(width*height); }

	glm::vec3 getPixel(int x, int y){
		return pixels[x*width + y];
	}

	void setPixel(int x, int y, glm::vec3 input){
		pixels[x*width + y] = input;
	}
};

#endif