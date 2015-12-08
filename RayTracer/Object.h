#ifndef OBJECT_H
#define OBJECT_H


#include <iostream>
#include <glm/glm.hpp>

#if USESDL
	#include <SDL.h>
	#include <SDL_image.h>
#endif

#include "Intersection.h"

class Object{
private:
public:


	glm::dvec3 md = glm::dvec3(1, .1, .1);
	glm::dvec3 ms = glm::dvec3(1, .1, .1);
	double shininess = 60.0;

#if USESDL
	SDL_Surface* image;
	SDL_Surface* specMap;
#endif

	bool textured = false;

	glm::dmat4 xForm;
	glm::dmat4 ixForm;
	glm::dmat4 ixtForm;


	Object() {
#if USESDL
		image = NULL;
		specMap = NULL;
#endif
	}

#if USESDL
	/* borrowed function for reading in JPEG pixels from http://sdl.beuc.net/sdl.wiki/Pixel_Access */
	glm::dvec3 getpixel(SDL_Surface *surface, int x, int y)
	{
		int bpp = surface->format->BytesPerPixel;
		/* Here p is the address to the pixel we want to retrieve */
		Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		switch (bpp) {
		case 1:
			return glm::dvec3(0, 0, 0);
			break;

		case 2:
			return glm::dvec3(0, 0, 0);
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return glm::dvec3(p[2] / 255.0, p[1] / 255.0, p[0] / 255.0);
			else
				return glm::dvec3(p[0] / 255.0, p[1] / 255.0, p[2] / 255.0);
			break;

		case 4:
			return glm::dvec3(0, 0, 0);
			break;

		default:
			return glm::dvec3(0, 0, 0);
		}
	}
#endif


	glm::dvec3 getDiffColor(glm::vec3 point) {
		if (!textured) {
			return md;
		};

#if USESDL
		//Uv coordsssssss
		glm::dvec3 pix = glm::dvec3(1, 1, 1);
		int u = (.5 - std::atan2(point[2], point[0]) / (3.14159 * 2)) * image->w;
		int v = (.5 - std::asin(point[1]) / 3.14159) * image->h;
		pix = getpixel(image, u, v);
		glm::dvec3 tmd =
			pix;
		return tmd;

#else
		return md;
#endif
	}

	glm::dvec3 getSpecColor(glm::vec3 point) {
		if (!textured) {
			return ms;
		}
		glm::dvec3 pix = glm::dvec3(1, 1, 1);

#if USESDL

		//Calculate UV coordinates
		int u = (.5 - std::atan2(point[2], point[0]) / (3.14159 * 2)) * specMap->w;
		int v = (.5 - std::asin(point[1]) / 3.14159) *  specMap->h;
		pix = getpixel(specMap, u, v);
#endif
		glm::dvec3 tms = 
			pix * 
			ms;
		return tms;
	}

	//Overridden by sub-classes
	virtual Intersection intersect(Ray ray){ return Intersection(); };
};

#endif