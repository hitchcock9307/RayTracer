#include "Ray.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Viewport.h"
#include "Intersection.h"
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
		scene.load(std::string("../SphereFlake3.scn"));
	
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

				lightRays(sIntersect, iObj);

				scene.view.setPixel(x, y, color);
				pointLoc[1] -= halfWidth;
			}
			pointLoc[0] += halfWidth;
			pointLoc[1] = halfWidth + scene.view.d/2;
		}

		toPPM();
		system("PAUSE");
	}

	glm::vec3 lightRays(Intersection sIntersect, Object* iObj){
		Intersection cIntersect;
		glm::vec3 color = glm::vec3(0,0,0);
		for (Light* l : scene.lights){
			if (iObj == NULL) break;

			glm::vec3 L = glm::normalize(glm::vec3(glm::vec3(l->pos) - glm::vec3(sIntersect.point)));

			Ray shadRay;
			shadRay.direction = glm::vec4(L, 0);
			shadRay.point = sIntersect.point;

			Intersection shadIntersect;
			shadIntersect.distance = scene.view.clipDistance;
			for (Object* o : scene.objects) {
				cIntersect = o->intersect(shadRay);
				if (cIntersect.distance > .01 && cIntersect.distance < shadIntersect.distance){
					shadIntersect = cIntersect;
					break;
				}
			}
			if (shadIntersect.distance != scene.view.clipDistance) continue;


			glm::vec3 V = glm::normalize(glm::vec3(-sIntersect.v));
			glm::vec3 N = glm::normalize(glm::vec3(sIntersect.normal));
			glm::vec3 H = glm::normalize(L + V);
			glm::vec3 Cl = l->color;
			float kd = glm::max(glm::dot(L, N), 0.0f);
			float ks = glm::pow(glm::max(glm::dot(N, H), 0.0f), iObj->shininess * 4);

			color += kd*iObj->md*Cl;
			color += ks*iObj->ms*Cl;

			
		}
		return color;
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