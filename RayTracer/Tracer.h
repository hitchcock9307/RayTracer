#include "Ray.h"
#include "Object.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Viewport.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <algorithm>



class Tracer{
private:

public:
	std::vector<Light> lights;
	std::vector<Object*> objects;
	glm::vec4 cameraLoc = glm::vec4(0, 0, 1, 1);
	Viewport view;

	Tracer(){
		float pixWidth = (2 * view.d) / view.width;
		float halfWidth = view.d / view.height;

		float yCur = halfWidth - view.d/2;
		float xCur = halfWidth - view.d/2;

		//Sphere newSphere;
		//newSphere.xForm = glm::translate(newSphere.xForm, glm::vec3(0, 0, -7));
		//newSphere.sphereColor = glm::vec3(1, 1, .1);
		//objects.push_back(&newSphere);

		Sphere newSphere2;
		newSphere2.sphereColor = glm::vec3(1, 0, 0);
		//newSphere2.xForm = glm::scale(newSphere2.xForm, glm::vec3(.5, .5, .5));
		newSphere2.xForm = glm::translate(newSphere2.xForm, glm::vec3(0, 0, -1));
		objects.push_back(&newSphere2);


		for (int x = 0; x < view.width; x++){
			for (int y = 0; y < view.height; y++){
				glm::vec4 pointLoc = glm::vec4(xCur, yCur, 0.0f, 1.0f);
				glm::vec4 dir = pointLoc - cameraLoc;

				//printVec3(glm::vec3(pointLoc));
				
				Ray ray;
				ray.direction = dir;
				ray.point = pointLoc;

				float smallestIntersect = (float)view.clipDistance;
				glm::vec3 finColor = view.backColor;
				for (Object* o : objects){
					glm::vec3 color;
					float intersect = o->intersect(ray, color);
					if ( intersect > 0){
						if (intersect < smallestIntersect){
							//std::cout << "intersect " << x*view.width + y << std::endl;
							smallestIntersect = intersect;
							finColor = color;
						}
					}
				}

				view.setPixel(x, y, finColor);
				yCur += halfWidth;
			}
			xCur += halfWidth;
			yCur = halfWidth - view.d/2;
		}

		toPPM();
	}

	void toPPM() {
		std::ofstream ofs("C:\\Users\\Lucid\\Documents\\Visual Studio 2015\\Projects\\RayTracer\\out.ppm", std::ios::out | std::ios::binary);
		ofs << "P6\n" << view.width << " " << view.height << "\n255\n";
		for (int x = 0; x < view.width; ++x) {
			for (int y = 0; y < view.height; y++) {
				glm::vec3 pix = view.getPixel(x, y);
				//printVec3(pix);
				ofs << (unsigned char)(std::min(float(1), pix[0]) * 255) <<
					(unsigned char)(std::min(float(1), pix[1]) * 255) <<
					(unsigned char)(std::min(float(1), pix[2]) * 255);
			}
		}
		ofs.close();
	}

};