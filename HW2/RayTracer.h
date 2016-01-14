#ifndef _INCLUDERAYTRACERHEADER
#define _INCLUDERAYTRACERHEADER
//#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <stack>
#include "Camera.h"
#include "Film.h"
#include "Variable.h"
#include "Primitive.h"
#include "Light.h"
#include "Ray.h"
#include "Camera.h"
#include <vector>
#include "Triangle.h"
#include "Sphere.h"
#include "DirectionalLight.h"
#include "PointLight.h"
class RayTracer{
public:
	RayTracer(const char *filename);
	~RayTracer();
	void trace(Ray &ray, int depth, vec3 *color);
	void recursive();
	bool readvals(std::stringstream &s, const int numvals, float *values);
	void rightmultiply(const mat4 & M, std::stack<mat4> &transfstack);
	void call_from_thread(int tid, int *total);
	inline float fresnel(const vec3 &I, const vec3 &N, const float transparency);
private:
	int depth;
	int width, height;
	std::string  filename;
	Film *film;
	Camera *camera;
	std::vector<light*> Light;
	std::vector<vec3> vertex;
	std::vector<Primitive*> Primitive;
};

#endif