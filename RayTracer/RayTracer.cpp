
#define USESDL 1
#define USEMP 1
#define SCENEFILE "../1n.scn"
#include "Tracer.h"

glm::dmat4 globalTransform;

#undef main
int main(int argc, char* argv[]){


	new Tracer();
}