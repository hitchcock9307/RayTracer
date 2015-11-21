#include "Ray.h"
#include "Object.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Viewport.h"
#include "Intersection.h"
#include "Sphere.h"
#include "Scene.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <algorithm>
#include <string>



class Tracer{
private:

public:
	glm::vec4 cameraLoc = glm::vec4(0, 0, 1, 1);
	Scene scene;

	Tracer(){
		scene.load(std::string("../1n.scn"));
		//Sphere *s = new Sphere();
		//s->xForm = glm::mat4(1.0f);
		//s->xForm = glm::translate(s->xForm, glm::vec3(0, 3, 0));
		//s->xForm = glm::translate(s->xForm, glm::vec3(2, 1, -10));
		//s->xForm = glm::rotate(s->xForm, glm::radians(40.0f), glm::vec3(0, 0, 1));
		//s->xForm = glm::translate(s->xForm, glm::vec3(0, 0, -3));

		//s->ixForm = glm::inverse(s->xForm);
		//s->ixtForm = glm::transpose(s->ixForm);
		//scene.objects.push_back(s);

		//Light *l = new Light();
		//l->color = glm::vec3(1, 1, 1);
		//l->pos = glm::vec4(-50.0, 5.0, -30.0, 1);
		//scene.lights.push_back(l);

		float pixWidth = (2 * scene.view.d) / scene.view.width;
		float halfWidth = scene.view.d / scene.view.height;

		float yCur = halfWidth + scene.view.d/2;
		float xCur = halfWidth - scene.view.d/2;

		glm::vec4 pointLoc = glm::vec4(xCur, yCur, 0.0f, 1.0f);

		for (int x = 0; x < scene.view.width; x++){
			for (int y = 0; y < scene.view.height; y++){
				glm::vec4 dir = pointLoc - cameraLoc;

				Ray ray;
				ray.direction = dir;
				ray.point =  pointLoc;

				glm::vec3 color = scene.backColor;

				Intersection sIntersect;
				Intersection cIntersect;
				Object* iObj = NULL;

				sIntersect.distance = scene.view.clipDistance;

				for (Object* o : scene.objects){
					cIntersect = o->intersect(ray);
					if ( cIntersect.distance > 0){
						if (cIntersect.distance < sIntersect.distance){
							sIntersect = cIntersect;
							iObj = o;
							color = scene.ambColor;
						}
					}
				}

				for (Light* l : scene.lights){
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

				scene.view.setPixel(x, y, color);
				pointLoc[1] -= halfWidth;
			}
			pointLoc[0] += halfWidth;
			pointLoc[1] = halfWidth + scene.view.d/2;
		}

		toPPM();
	}

	void printVec3(glm::vec3 vec){
		std::cout << "(" << vec[0] << " ," << vec[1] << " ," << vec[2] << ")\n";
	}


	void toPPM() {
		std::ofstream ofs("../out.ppm", std::ios::out | std::ios::binary);
		ofs << "P6\n" << scene.view.width << " " << scene.view.height << "\n255\n";
		for (int y = 0; y < scene.view.height; y++) {
			for (int x = 0; x < scene.view.width; x++) {
				glm::vec3 pix = scene.view.getPixel(x, y);
				//printVec3(pix);
				ofs << (unsigned char)(std::min(float(1), pix[0]) * 255) <<
					(unsigned char)(std::min(float(1), pix[1]) * 255) <<
					(unsigned char)(std::min(float(1), pix[2]) * 255);
			}
		}
		ofs.close();
	}

};