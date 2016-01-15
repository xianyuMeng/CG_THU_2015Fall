#include "RayTracer.h"
#include <fstream>
#include <iomanip>
#include<thread>


using namespace std;
default_random_engine RayTracer::generator;

bool RayTracer::readvals(stringstream &s, const int numvals, float* values)
{
	for (int i = 0; i < numvals; i++) {
		s >> values[i];
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}
void RayTracer::rightmultiply(const mat4 & M, stack<mat4> &transfstack)
{
		mat4 &T = transfstack.top();
		T = T * M;
}

pair<float, float> RayTracer::uniformSampleCircle() {
	uniform_real_distribution<float> distribution;
	float theta = 2.0f * pi * distribution(generator);
	float radius = sqrtf(distribution(generator));
	return make_pair(radius, theta);
}

vec3 RayTracer::cosineSampleHemisphere(const vec3 &normal, const vec3 &t, const vec3 &s) {
	// Sample in a circle.
	auto sample = uniformSampleCircle();

	// Transform it into the hemisphere.
	float cosTheta = sqrtf(1.0f - sample.first * sample.first);
	vec3 direction = (t * cos(sample.second) + s * sin(sample.second)) * sample.first + normal * cosTheta;
	return normalize(direction);
}

pair<vec3, vec3> RayTracer::getCoords(const vec3 &normal) {
	//vec3 normal = hit.pos == INTERSECTION_OUT ? hit.normal : -hit.normal;
	pair<vec3, vec3> ret;
	// Choose the local coordinate.
	ret.first = normal.x > 0.9f ? vec3(normal.y, -normal.x, 0.0f) : vec3(0.0f, normal.z, normal.y);
	ret.first = normalize(ret.first);
	ret.second = cross(normal, ret.second);
	return ret;
}

void RayTracer::trace(Ray &ray, int depth, vec3 *color) {

	// Clear the color.
	color->x = color->y = color->z = 0.0f;
	depth -= 1;
	if (depth == 0){
		//if depth exceeds some threshold, make the color black and return.
		return;
	}
	intersectP hitPoint;
	int whatever = -1;
	hitPoint.tHit = FarFarAway;
	
	for (size_t i = 0; i < Primitive.size(); ++i) {
		intersectP hit = Primitive[i]->intersection(ray);
		if (hit.tHit > NEAR && hit.tHit < hitPoint.tHit){
			whatever = i;
			hitPoint = hit;		
		}
	}

	if ((hitPoint.tHit >= FarFarAway - NEAR) || (hitPoint.tHit < NEAR)) {
		return;
	}
	
	vec3 I = hitPoint.obj.ambient + hitPoint.obj.emission;
	/*
	vec3 reflect = glm::reflect(ray.direction, hitPoint.normal);
	Ray reflectRay = Ray(reflect, hitPoint.intersectPoint, 0.0f, FarFarAway);
	vec3 mirrorColor;
	trace(reflectRay, depth, &mirrorColor);
	I += mirrorColor * hitPoint.obj.specular;
	*/

	if (hitPoint.materialType == _refract) {
		vec3 refract;
		vec3 transmitColor;
		if (hitPoint.intersectPos == intersect_inside) {
			for (int i = 0; i <= 1; ++i) {
				vec3 sColor;
				sColor.x = sColor.y = sColor.z = 0;
				pair<vec3, vec3> coords = RayTracer::getCoords(-hitPoint.normal);
				vec3 direction = RayTracer::cosineSampleHemisphere(hitPoint.normal, coords.first, coords.second);
				trace(Ray(direction, hitPoint.intersectPoint, 0.0f, FarFarAway), depth, &sColor);
				transmitColor += sColor;
			}
			/*refract = glm::refract(ray.direction, -hitPoint.normal, hitPoint.obj.transparency);
			if (length(refract) > 0.5f) {
				Ray refractRay = Ray(refract, hitPoint.intersectPoint, 0.0f, FarFarAway);
				trace(refractRay, depth, &transmitColor);
				I += transmitColor * hitPoint.obj.specular;*/
			transmitColor / vec3(2, 2, 2);
			I += transmitColor * hitPoint.obj.specular;
		}
		else {
			/*
			refract = glm::refract(ray.direction, hitPoint.normal, 1.0f / hitPoint.obj.transparency);
			Ray refractRay = Ray(refract, hitPoint.intersectPoint, 0.0f, FarFarAway);
			trace(refractRay, depth, &transmitColor);
			I += transmitColor * hitPoint.obj.specular;
			*/
			for (int i = 0; i <= 1; ++i) {
				vec3 sColor;
				sColor.x = sColor.y = sColor.z = 0;
				pair<vec3, vec3> coords = RayTracer::getCoords(hitPoint.normal);
				vec3 direction = RayTracer::cosineSampleHemisphere(hitPoint.normal, coords.first, coords.second);
				trace(Ray(direction, hitPoint.intersectPoint, 0.0f, FarFarAway), depth, &sColor);
				transmitColor += sColor;
			}
			transmitColor / vec3(2, 2, 2);
			I += transmitColor * hitPoint.obj.specular;
		}
	}
	else {
		for (size_t i = 0; i < Light.size(); ++i) {
			float t;
			Ray shadowRay = Light[i]->GenerateRay(hitPoint.intersectPoint, &t);
			intersectP hitPointShadow;
			bool is_block = false;
			for (size_t j = 0; j < Primitive.size(); ++j) {
				hitPointShadow = Primitive[j]->intersection(shadowRay);
				if (hitPointShadow.tHit < t - NEAR && hitPointShadow.tHit > NEAR) {
					is_block = true;
					break;
				}
			}
			if (!is_block) {
				vec3 L = shadowRay.direction;
				vec3 N = hitPoint.normal;
				vec3 eye = -ray.direction;
				vec3 H = normalize(eye + L);
				vec3 diffuse_color = Light[i]->color * hitPoint.obj.diffuse * max(dot(N, L), 0.0f);
				vec3 specualr_color = Light[i]->color * hitPoint.obj.specular * pow(max(dot(N, H), 0.0f), hitPoint.obj.shininess);
				float distance = t;
				float atten = Light[i]->attenuation.x + Light[i]->attenuation.y * distance + Light[i]->attenuation.z * pow(distance, 2);
				vec3 V = (diffuse_color + specualr_color) / atten;
				I += V;
			}

		}
		/*
		vec3 reflect = glm::reflect(ray.direction, hitPoint.normal);
		Ray reflectRay = Ray(reflect, hitPoint.intersectPoint, 0.0f, FarFarAway);
		vec3 mirrorColor;
		trace(reflectRay, depth, &mirrorColor);
		I += mirrorColor * hitPoint.obj.specular;
		*/
		vec3 reflectColor;
		for (int i = 0; i <= 1; ++i) {
			vec3 sColor;
			sColor.x = sColor.y = sColor.z = 0;
			pair<vec3, vec3> coords = RayTracer::getCoords(hitPoint.normal);
			vec3 direction = RayTracer::cosineSampleHemisphere(hitPoint.normal, coords.first, coords.second);
			trace(Ray(direction, hitPoint.intersectPoint, 0.0f, FarFarAway), depth, &sColor);
			reflectColor += sColor;
		}
		reflectColor /= vec3(2, 2, 2);
		I += reflectColor * hitPoint.obj.specular;
	}
	*color = I;
}

RayTracer::RayTracer(const char *filename) {
	string str, cmd;
	ifstream in;
	in.open(filename);
	stack<mat4> transformMat;
	transformMat.push(mat4(1.0));
	object obj;
	depth = 5;
	vec3 attenuation = vec3(1.0, 0.0, 0.0);
	if (in.is_open()) {
		getline(in, str);
		while (in) {
			if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
				stringstream s(str);
				s >> cmd;
				bool validinput;
				float values[10];
				if (cmd == "size") {
					validinput = readvals(s, 2, values);
					if (validinput) {
						width = int(values[0]);
						height = int(values[1]);
					}
				}
				else if (cmd == "maxdepth") {
					validinput = readvals(s, 1, values);
					if (validinput) {
						depth = int(values[0]);
					}
				}
				else if (cmd == "output") {
					s >> this->filename;
				}
				else if (cmd == "camera"){
					validinput = readvals(s, 10, values);
					if (validinput){
						camera = new Camera(vec3(values[0], values[1], values[2]),
							vec3(values[3], values[4], values[5]),
							vec3(values[6], values[7], values[8]),
							values[9],
							height,
							width);
					}
				}
				else if (cmd == "sphere") {
					validinput = readvals(s, 4, values);
					if (validinput){
						Sphere *sph = new Sphere(vec3(values[0], values[1], values[2]), values[3], transformMat.top(), obj);
						Primitive.push_back(sph);
					}
				}
				else if (cmd == "maxverts"){
				}
				else if (cmd == "maxvertnorms"){
				}
				else if (cmd == "vertex"){
					validinput = readvals(s, 3, values);
					if (validinput){
						vertex.push_back(vec3(values[0], values[1], values[2]));
					}
				}
				else if (cmd == "vertexnormal"){

				}
				else if (cmd == "tri"){
					validinput = readvals(s, 3, values);
					if (validinput){
						vec4 A = vec4(vertex[(unsigned int)values[0]], 1);
						A = transformMat.top() * A;
						vec4 B = vec4(vertex[(unsigned int)values[1]], 1);
						B = transformMat.top() * B;
						vec4 C = vec4(vertex[(unsigned int )values[2]], 1);
						C = transformMat.top() * C;
						Triangle *tri = new Triangle(vec3(A.x/A.w, A.y/A.w, A.z/A.w),
							vec3(B.x / B.w, B.y / B.w, B.z / B.w), vec3(C.x / C.w, C.y / C.w, C.z / C.w) , obj);
						Primitive.push_back(tri);
					}
				}
				else if (cmd == "trinormal"){

				}
				else if (cmd == "translate"){
					validinput = readvals(s, 3, values);
					if (validinput){
						mat4 translate = Transform::translate(values[0], values[1], values[2]);
						rightmultiply(translate, transformMat);
					}
				}
				else if (cmd == "rotate"){
					validinput = readvals(s, 4, values);
					if (validinput){
						mat3 rotate = Transform::rotate(values[3], vec3(values[0], values[1], values[2]));
						mat4 Rotate(1.0);
						for (size_t i = 0; i != 3; ++i){
							for (size_t j = 0; j != 3; ++j){
								Rotate[i][j] = rotate[i][j];
							}
						}
						rightmultiply(Rotate, transformMat);
					}
				}
				else if (cmd == "scale"){
					validinput = readvals(s, 3, values);
					if (validinput){
						mat4 scale = Transform::scale(values[0], values[1], values[2]);
						rightmultiply(scale, transformMat);
					}
				}
				else if (cmd == "pushTransform"){
					mat4 pushMat = transformMat.top();
					transformMat.push(pushMat);
				}
				else if (cmd == "popTransform"){
					transformMat.pop();
				}
				else if (cmd == "directional"){
					validinput = readvals(s, 6, values);
					if (validinput){
						DirectionalLight *Dlight = new DirectionalLight(vec3(values[0], values[1], values[2]),
							vec3(values[3], values[4], values[5]), attenuation);
						Light.push_back(Dlight);
					}
				}
				else if (cmd == "point"){
					validinput = readvals(s, 6, values);
					if (validinput){
						vec3 pos = vec4_to_vec3(transformMat.top() * vec4(values[0], values[1], values[2], 1.0f));
						PointLight *Plight = new PointLight(vec3(values[3], values[4], values[5]),
							pos,
							attenuation);
						Light.push_back(Plight);
					}
				}
				else if (cmd == "attenuation"){
					validinput = readvals(s, 3, values);
					if (validinput) {
						attenuation = vec3(values[0], values[1], values[2]);
					}
					else
						attenuation = vec3(1.0, 0.0, 0.0);
				}
				else if (cmd == "ambient") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						obj.ambient = vec3(values[0], values[1], values[2]);
					}
					else
						obj.ambient = vec3(0.2f, 0.2f, 0.2f);
				}
				else if (cmd == "diffuse") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						obj.diffuse = vec3(values[0], values[1], values[2]);
					}
				}
				else if (cmd == "specular") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						obj.specular = vec3(values[0], values[1], values[2]);
					}
				}
				else if (cmd == "emission") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						obj.emission = vec3(values[0], values[1], values[2]);
					}
				}
				else if (cmd == "shininess") {
					validinput = readvals(s, 1, values);
					if (validinput) {
						obj.shininess = values[0];
					}
				}
				else if (cmd == "transparency") {
					validinput = readvals(s, 1, values);
					if (validinput) {
						obj.transparency = values[0];
					}
				}

				else {

				}	
			}
			getline(in, str);
		}
	}
}

RayTracer::~RayTracer() {
	delete camera;
	for (size_t i = 0; i < Primitive.size(); ++i){
		delete Primitive[i];
	}
}
static const int thread_num = 4;
void RayTracer::call_from_thread(int tid, int *total){
	for (int i = width / thread_num * (tid); i < width / thread_num * (tid + 1); ++i){
		for (int j = 0; j < height; ++j){
			vec3 Color(0.0, 0.0, 0.0);
			for (float fragmentx = i; fragmentx < i + 1.0f; fragmentx += 0.5f) {
				for (float fragmenty = j; fragmenty < j + 1.0f; fragmenty += 0.5f) {
					Ray ray = camera->GenerateRay(i, j);
					//Ray ray(normalize(vec3(0, -1, 0) - vec3(0, 2, 2)), vec3(0, 2, 2), NEAR, FarFarAway);
					vec3 color(0.0, 0.0, 0.0);
					trace(ray, depth, &color);
					Color += color/vec3(4, 4, 4);
				}
			}
			/*
			if (i == 300 - 1 && j == height - 190){
				int x = 1;
				x++;
			}
			Ray ray = camera->GenerateRay(i, j);
			//Ray ray(normalize(vec3(0, -1, 0) - vec3(0, 2, 2)), vec3(0, 2, 2), NEAR, FarFarAway);
			vec3 color(0.0, 0.0, 0.0);
			trace(ray, depth, &color);*/
			film->commit(Color, i, j);
			(*total)++;
		}
	}
}

void RayTracer::recursive(){
	film = new Film(width, height);
	std::thread t[thread_num];
	int progress[thread_num];
	for (int i = 0; i < thread_num; ++i){
		progress[i] = 0;
		t[i] = std::thread(&RayTracer::call_from_thread, this, i, &progress[i]);
	}

	for (;;) {
		int total = 0;
		for (const auto i : progress){
			total += i;
		}
		float percent = float(total) / float(width * height);
		std::cout << "\r";
		std::cout << "Well: " << fixed << setprecision(2) << percent * 100.0f << "%";
		if (total == width * height)
			break;
	}

	for (int i = 0; i < thread_num; ++i){
		t[i].join();
	}
	//for (int i = 0; i < width; ++i) {
	//	for (int j = 0; j < height; ++j) {
	//		if (i == 60 && j == 60) {
	//			int x = 2;
	//		}
	//		/*if (i == width / 2 && j == height / 2) {
	//			int x = 1 + 1;
	//		}*/
	//		float percent = float(i * height + j) / float(width * height);
	//		std::cout << "\r";
	//		std::cout << "Well: " << fixed << setprecision(2) << percent * 100.0f << "%";
	//		Ray ray = camera->GenerateRay(i, j);
	//		vec3 color(0.0, 0.0, 0.0);
	//		trace(ray, depth, &color);
	//		film->commit(color, i, j);
	//	}
	//}
	film->WriteImage(filename);
	delete film;
}