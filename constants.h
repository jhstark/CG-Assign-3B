#ifndef __constantsH__
#define __constantsH__


#define VALS_PER_VERT 3
#define VALS_PER_NORM 3
#define VALS_PER_TEX 2

// Required for c++11
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

int winX = 800;
int winY = 800;

std::map< std::string , int > programIdMap;

#endif