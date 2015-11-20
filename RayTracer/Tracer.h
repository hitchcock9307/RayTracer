#include "Ray.h"
#include "Object.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Viewport.h"
#include "Intersection.h"

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
	std::vector<Light*> lights;
	std::vector<Object*> objects;
	glm::vec4 cameraLoc = glm::vec4(0, 0, 1, 1);
	Viewport view;

	Tracer(){
		float pixWidth = (2 * view.d) / view.width;
		float halfWidth = view.d / view.height;

		float yCur = halfWidth + view.d/2;
		float xCur = halfWidth - view.d/2;


		Sphere newSphere2;
		newSphere2.sphereColor = glm::vec3(1, 0, 0);
		newSphere2.xForm = glm::translate(glm::mat4(1) , glm::vec3(0, 0, -5));
		newSphere2.ixForm = glm::inverse(newSphere2.xForm);
		newSphere2.ixtForm = glm::transpose(newSphere2.ixForm);
		objects.push_back(&newSphere2);

		Light testLight;
		testLight.color = glm::vec3(1, 1, 1); 
		testLight.pos = glm::vec4(0, 1, 1, 1);
		lights.push_back(&testLight);

		glm::vec4 pointLoc = glm::vec4(xCur, yCur, 0.0f, 1.0f);

		for (int x = 0; x < view.width; x++){
			for (int y = 0; y < view.height; y++){
				glm::vec4 dir = pointLoc - cameraLoc;

				Ray ray;
				ray.direction = dir;
				ray.point =  pointLoc;

				glm::vec3 color;

				Intersection sIntersect;
				Intersection cIntersect;
				Object* iObj = NULL;

				sIntersect.distance = view.clipDistance;

				for (Object* o : objects){
					cIntersect = o->intersect(ray);
					if ( cIntersect.distance > 0){
						if (cIntersect.distance < sIntersect.distance){
							sIntersect = cIntersect;
							iObj = o;
							color = glm::vec3(.2, .2, .2);
						}
					}
				}

				for (Light* l : lights){
					if (iObj == NULL) break;
					glm::vec3 L = glm::normalize(glm::vec3(l->pos - sIntersect.normal));
					glm::vec3 V = glm::normalize(glm::vec3(cameraLoc - sIntersect.normal));
					glm::vec3 N = glm::normalize(glm::vec3(sIntersect.normal));
					glm::vec3 H = glm::normalize(L + V);
					glm::vec3 Cl = l->color;
					float kd = glm::max(glm::dot(L, N), 0.0f);
					float ks = glm::pow(glm::max(glm::dot(N, H), 0.0f), iObj->shininess);
					
					color += kd*iObj->md*Cl;
					color += ks*iObj->ms*Cl;
				}

				view.setPixel(x, y, color);
				pointLoc[1] -= halfWidth;
			}
			pointLoc[0] += halfWidth;
			pointLoc[1] = halfWidth + view.d/2;
		}

		toPPM();
	}

	void printVec3(glm::vec3 vec){
		std::cout << "(" << vec[0] << " ," << vec[1] << " ," << vec[2] << ")\n";
	}


	void toPPM() {
		std::ofstream ofs("C:\\Users\\Lucid\\source\\repos\\raytracer\\out.ppm", std::ios::out | std::ios::binary);
		ofs << "P6\n" << view.width << " " << view.height << "\n255\n";
		for (int y = 0; y < view.height; y++) {
			for (int x = 0; x < view.width; x++) {
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