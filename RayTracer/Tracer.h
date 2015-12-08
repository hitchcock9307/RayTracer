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
#include <atlimage.h>
#include <gdiplusimaging.h>
#undef min
#undef max



#if USESDL
	#include <SDL.h>
	#include <SDL_image.h>
#endif

#include <glm/glm.hpp>

#if USEMP
#include <omp.h>
#endif

class Tracer{
private:
#if USESDL
	SDL_Window* window;
	SDL_Surface* screenSurface;
#endif
	glm::dvec4 cameraLoc = glm::dvec4(0, 0, 1, 1);
	Scene scene;

public:





	Tracer(){
		scene.load(std::string(SCENEFILE)); // load in file
#if USESDL
		loadSDL(); //Load window (if applicable)
#endif
	
		trace(); // DO EVERYTHING
#if USESDL
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
#endif

		toPPM();
	}

	void trace() {

		//Calculate view properties
		double pixWidth = (2.0 * scene.view.d) / (double)scene.view.width;
		double halfWidth = (double)scene.view.d / (double)scene.view.height;

		//half of d
		double dHalf = (scene.view.d / 2.0);



		int x, y;


		while(true){
		//Parallelize the loop
#if USEMP
		#pragma omp parallel for private(y)		
#endif
			for (x = 0; x < scene.view.width; x++) {
				for (y = 0; y < scene.view.height; y++) {
					//Calculate point location
					glm::dvec4 pointLoc = glm::dvec4(x*halfWidth - dHalf, -y*halfWidth + dHalf, 0.0, 1.0);
					//Set the intial color to the back color, since an immediate return means no intersection
					glm::dvec3 color = scene.backColor;
					int depth = 0;

					//Create ray for intersect
					Ray ray;
					ray.direction = pointLoc - cameraLoc;
					ray.point = pointLoc;

					//Try to determine color of pixel
					traceRays(color, depth, ray);
#if USESDL
					SDL_Rect r; //Sdl rect of a single pixel for screen display
					r.w = 1;
					r.h = 1;
					r.x = x;
					r.y = y;

					SDL_FillRect( //Write to pixel buffer
						screenSurface,
						&r,
						SDL_MapRGB(
							screenSurface->format,
							(unsigned char)(glm::min(float(1), (float)color[0]) * 255),
							(unsigned char)(glm::min(float(1), (float)color[1]) * 255),
							(unsigned char)(glm::min(float(1), (float)color[2]) * 255)));
#endif
					//Set the pixel for the PPM output
					scene.view.setPixel(x, y, color);
				}
				//Swap buffer
				globalTransform = glm::translate(globalTransform, glm::dvec3(0, 0, .0001));
#if USESDL
				SDL_UpdateWindowSurface(window);
			}
#endif
		}
	}

	//glm::dvec3 reflection(Intersection sIntersect) {

	//}

	void traceRays(glm::dvec3 &color, int &depth, Ray intersect) {
		//If our recursion has gone too far from reflection
		if (depth > scene.maxDepth) return;

		//S intersect is the value which stores the 
		//closest object intersected, c is current
		Intersection sIntersect;
		Intersection cIntersect;
		Object* iObj = NULL;

		//Set the distance to infinity (effectively)
		sIntersect.distance = scene.view.clipDistance;

		//For every object
		for (Object* o : scene.objects) {
			cIntersect = o->intersect(intersect); //Check for intersect and return values if intersected
			if (cIntersect.distance > 0) { //Greater than zero
				if (cIntersect.distance < sIntersect.distance) { //Smallest
					sIntersect = cIntersect; //Set for rest of calculations
				}
			}
		}
		if (sIntersect.obj == NULL)
			return;

		//Set ambient color
		color = scene.ambColor * sIntersect.obj->md;

		//Calculate values before loop for speed
		glm::dvec3 V = glm::normalize(glm::dvec3(-sIntersect.v));
		glm::dvec3 N = glm::normalize(glm::dvec3(sIntersect.normal));

		glm::dvec3 intersectDiffColor = sIntersect.obj->getDiffColor(N);
		glm::dvec3 intersectSpecColor = sIntersect.obj->getSpecColor(N);

		//For each light
		for (Light* l : scene.lights) {
			//Calculate vector to the light
			glm::dvec4 lightPos = l->pos;
			glm::dvec3 L = glm::normalize(glm::dvec3(lightPos - sIntersect.point));

			//Calculate shadow ray
			Ray shadRay;
			shadRay.direction = glm::dvec4(L, 0);
			shadRay.point = sIntersect.point + glm::dvec4(N*scene.delta, 0.0);

			//See if any of the objects block the light vector
			Intersection shadIntersect;
			shadIntersect.distance = scene.view.clipDistance;
			for (Object* o : scene.objects) {
				cIntersect = o->intersect(shadRay);
				//If blocked break out and set a closer intersect
				if (cIntersect.distance < shadIntersect.distance && cIntersect.distance != -1) {
					shadIntersect = cIntersect;
					break;
				}
			}
			//Ignore this light and go to the next
			if (shadIntersect.distance != scene.view.clipDistance) continue;

			//Calculate half vector
			glm::dvec3 H = glm::normalize(L + V);
			glm::dvec3 Cl = l->color; //For cleaner code
			
			//Calculate diffuse and specular contribution
			double kd = glm::max(glm::dot(N, L), 0.0);
			double ks = glm::pow(glm::max(glm::dot(N, H), 0.0), sIntersect.obj->shininess * 4);
			
			color += kd * intersectDiffColor * Cl;
			color += ks * intersectSpecColor * Cl;
		}

		//Start relection, R vector calculation
		glm::dvec3 R = V - (N*2.0)*glm::dot(N, V);
		
		//Create the reflected ray to recursively call this function
		Ray reflected;
		reflected.point = sIntersect.point + glm::dvec4(N*scene.delta, 0.0);
		reflected.direction = -glm::dvec4(glm::normalize(R), 0.0);

		//Start with the back 
		glm::dvec3 rColor = glm::dvec3(0, 0, 0);

		//Increase depth and recurr!!!
		depth++;
		traceRays(rColor, depth, reflected);

		//Add reflected color
		color += rColor * sIntersect.obj->getSpecColor(N);

		
	}
#if USESDL
	//Window setup
	void loadSDL() {
		window = NULL; //Safety set window obj to null

		screenSurface = NULL; //Ditto

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			//Print sdl error just in case
			printf("Could not start SDL. SDL_GetError: %s\n", SDL_GetError());
		}
		else
		{
			//Create window
			window = SDL_CreateWindow(
				"Graphics Lab 6 (Ray Tracer)",
				SDL_WINDOWPOS_UNDEFINED, //X and Y Vlaues
				SDL_WINDOWPOS_UNDEFINED,
				scene.view.width, //Pull from scene
				scene.view.height,
				SDL_WINDOW_SHOWN); //Show window immediately
			if (window == NULL)
			{
				printf("Could not create window. SDL_GetError: %s\n", SDL_GetError());
			}
			else
			{
				//Get surface to be able to draw onto the window (realtime?)
				screenSurface = SDL_GetWindowSurface(window); 
			}
		}

	}
#endif

	//Debug
	void printdvec3(glm::dvec3 vec){
		std::cout << "(" << vec[0] << " ," << vec[1] << " ," << vec[2] << ")\n";
	}

	//Outputs scene.pixels' to ppm file
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