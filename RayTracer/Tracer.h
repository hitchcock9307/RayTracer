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

#include <SDL.h>
#include <omp.h>


class Tracer{
private:
	SDL_Window* window;
	SDL_Surface* screenSurface;

	glm::dvec4 cameraLoc = glm::dvec4(0, 0, 1, 1);
	Scene scene;
public:

	Tracer(){

		scene.load(std::string("../SphereFlake1.scn"));
		loadSDL();
	
		double pixWidth = (2 * scene.view.d) / scene.view.width;
		double halfWidth = scene.view.d / scene.view.height;

		double yCur = halfWidth + scene.view.d/2.0;
		double xCur = halfWidth - scene.view.d/2.0;

		double dHalf = (scene.view.d / 2.0);


		
		int x, y;
		#pragma omp parallel for private(y)
		for (x = 0; x < scene.view.width; x++) {
			for (y = 0; y < scene.view.height; y++) {
				glm::dvec4 pointLoc = glm::dvec4(x*halfWidth - dHalf, -y*halfWidth + dHalf, 0.0, 1.0);
				glm::dvec3 color = scene.backColor;
				int depth = 0;


				Ray ray;
				ray.direction = pointLoc - cameraLoc;
				ray.point = pointLoc;

				traceRays(color, depth, ray);
				SDL_Rect r;
				r.w = 1;
				r.h = 1;
				r.x = x;
				r.y = y;
				SDL_FillRect(
					screenSurface, 
					&r, 
					SDL_MapRGB(
						screenSurface->format, 
						(unsigned char)(std::min(float(1), (float)color[0]) * 255), 
						(unsigned char)(std::min(float(1), (float)color[1]) * 255),
						(unsigned char)(std::min(float(1), (float)color[2]) * 255)));


				//scene.view.setPixel(x, y, color);
				if ((x*scene.view.width + y) % 2000 == 0) SDL_UpdateWindowSurface(window);
			}

		}


		SDL_UpdateWindowSurface(window);
		bool done = false;
		SDL_Event event;
		while (!done) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					done = true;
				}
			}
		}
	}


	//glm::dvec3 reflection(Intersection sIntersect) {

	//}

	void traceRays(glm::dvec3 &color, int &depth, Ray intersect) {
		if (depth > scene.maxDepth) return;

		Intersection sIntersect;
		Intersection cIntersect;
		Object* iObj = NULL;

		sIntersect.distance = scene.view.clipDistance;

		for (Object* o : scene.objects) {
			cIntersect = o->intersect(intersect);
			if (cIntersect.distance > 0) {
				if (cIntersect.distance < sIntersect.distance) {
					sIntersect = cIntersect;
				}
			}
		}
		if (sIntersect.obj == NULL)
			return;

		color = scene.ambColor * sIntersect.obj->md;


		glm::dvec3 V = glm::normalize(glm::dvec3(-sIntersect.v));
		glm::dvec3 N = glm::normalize(glm::dvec3(sIntersect.normal));

		for (Light* l : scene.lights) {

			glm::dvec3 L = glm::normalize(glm::dvec3(glm::dvec3(l->pos) - glm::dvec3(sIntersect.point)));

			Ray shadRay;
			shadRay.direction = glm::dvec4(L, 0);
			shadRay.point = sIntersect.point;

			Intersection shadIntersect;
			shadIntersect.distance = scene.view.clipDistance;
			for (Object* o : scene.objects) {
				cIntersect = o->intersect(shadRay);
				if (cIntersect.distance > .01 && cIntersect.distance < shadIntersect.distance) {
					shadIntersect = cIntersect;
					break;
				}
			}
			if (shadIntersect.distance != scene.view.clipDistance) continue;


			glm::dvec3 H = glm::normalize(L + V);
			glm::dvec3 Cl = l->color;
			double kd = glm::max(glm::dot(N, L), 0.0);
			double ks = glm::pow(glm::max(glm::dot(N, H), 0.0), sIntersect.obj->shininess * 8);

			color += kd*sIntersect.obj->md*Cl;
			color += ks*sIntersect.obj->ms*Cl;


		}

		glm::dvec3 R = V - (N*2.0)*glm::dot(V, N);



		Ray reflected;
		reflected.point = sIntersect.point + glm::dvec4(N*scene.delta, 0.0);
		reflected.direction = -glm::dvec4(glm::normalize(R), 0.0);

		glm::dvec3 rColor = scene.backColor;

		depth++;
		traceRays(rColor, depth, reflected);

		color += rColor * sIntersect.obj->ms;

		
	}

	void loadSDL() {
		window = NULL;

		screenSurface = NULL;

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("Could not start SDL. SDL_GetError: %s\n", SDL_GetError());
		}
		else
		{
			window = SDL_CreateWindow(
				"Graphics Lab 6 (Ray Tracer)",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				scene.view.width,
				scene.view.height,
				SDL_WINDOW_SHOWN);
			if (window == NULL)
			{
				printf("Could not create window. SDL_GetError: %s\n", SDL_GetError());
			}
			else
			{
				screenSurface = SDL_GetWindowSurface(window); 
				

			}
		}

	}


	//glm::dvec3 lightRays(Intersection sIntersect){
	//	
	//}

	void printdvec3(glm::dvec3 vec){
		std::cout << "(" << vec[0] << " ," << vec[1] << " ," << vec[2] << ")\n";
	}


	void toPPM() {
		std::ofstream ofs("../out.ppm", std::ios::out | std::ios::binary);
		ofs << "P6\n" << scene.view.width << " " << scene.view.height << "\n255\n";
		for (int y = 0; y < scene.view.height; y++) {
			for (int x = 0; x < scene.view.width; x++) {
				glm::dvec3 pix = scene.view.getPixel(x, y);
				//printdvec3(pix);
				ofs << (unsigned char)(std::min(float(1), (float)pix[0]) * 255) <<
					(unsigned char)(std::min(float(1), (float)pix[1]) * 255) <<
					(unsigned char)(std::min(float(1), (float)pix[2]) * 255);
			}
		}
		ofs.close();
	}

};