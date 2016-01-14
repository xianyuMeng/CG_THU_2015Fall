#ifndef	_INCLUDEVARIABLEHEADER
#define _INCLUDEVARIABLEHEADER
//#define _CRT_SECURE_NO_WARNINGS
#include "glm\glm.hpp"
//#include "glm/gtx/string_cast.hpp"
using namespace glm;
#define FarFarAway 10000.0f
#define NEAR 0.001f

struct object{
	vec3 diffuse;
	vec3 emission;
	vec3 specular;
	vec3 ambient;
	float transparency;
	float shininess;
	mat4 transform;
};
enum intersectPos {
	intersect_inside,
	intersect_outside
};

enum materialType {
	reflect,
	reflect_and_refract,
};
struct intersectP{
	vec3 normal;
	float tHit;
	//to calculate color for each pixel, we need to return the material.
	object obj;
	vec3 intersectPoint;
	intersectPos intersectPos;
	materialType materialType;
};

inline vec3 vec4_to_vec3(vec4 &in) {
	float w = abs(in.w) < NEAR ? 1.0f : in.w;
	return vec3(in.x / w, in.y / w, in.z / w);
}



#endif