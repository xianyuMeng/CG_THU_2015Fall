#include "Sphere.h"
#include "glm\gtc\matrix_inverse.hpp"

Sphere::Sphere(const vec3 &center,
	const float radius,
	const mat4 &transformobj,
	const object &obj) :center(center), radius(radius), transformobj(transformobj), obj(obj) {
		inversetransform = inverse(transformobj);
}

inline vec3 applyMatrix(const mat4 &trans, const vec3 &v) {
	vec4 temp = vec4(v, 1.0f);
	temp = trans * temp;
	temp = temp / temp[3];
	return vec3(temp);
}

intersectP Sphere::intersection(const Ray &ray)const{
	/*Ray - Scene Intersection
	1. Apply inverse transform M^(-1) to ray
	ray directions have homogeneous crood set to 0
	2. Ray surface intersection
	3. transform intersection back to actual coords
	intersection point p transform as Mp
	Normals n transform as M^(-1)n*/
	/*
	ray-sphere intersection Geometry
	see http://www.vis.uky.edu/~ryang/teaching/cs535-2012spr/Lectures/13-RayTracing-II.pdf
	for futher information
	*/
	
	vec3 InvPos = applyMatrix(inversetransform, ray.pos);
	//vec3 InvPos = vec4_to_vec3(inversetransform * vec4(ray.pos, 1));
	vec3 InvDirection = normalize(applyMatrix(inversetransform, ray.pos + ray.direction) - InvPos);
	
	intersectP ret;
	ret.obj = obj;
	ret.tHit = FarFarAway;
	if (ret.obj.transparency > 1.0f) {
		ret.materialType = reflect_and_refract;
	}
	vec3 OC = center - InvPos;
	float radius_square = radius * radius;
	bool inside = false;
	float OC_square = Transform::dot(OC, OC);
	
	if (OC_square < radius_square)
		//the InvPos is in the sphere
		inside = true;
	//distance from ray_origin to thepoint closet to sphere_center
	float t_ca = Transform::dot(InvDirection, OC);
	if (t_ca < NEAR && inside == false){
		//the ray does not intersect the sphere
		return ret;
	}
	else {
		//the diatance from the closet point to the sphere's point
		float t_hc_square = radius_square - OC_square + t_ca * t_ca;
		float t_hc = sqrtf(t_hc_square);
		if (t_hc < NEAR)
			//no intersection
			return ret;
		else {
			if (inside == false){
				//ray_origin is outside the sphere
				//float t = abs(t_ca - t_hc) < NEAR ? t_ca - t_hc : t_ca + t_hc;
				//float t = t_ca - t_hc;
				float t_temp = t_ca - t_hc;
				if (t_temp > NEAR) {
					vec3 HitPoint = InvPos + t_temp * InvDirection;
					vec3 normal = HitPoint - center;
					ret.intersectPoint = applyMatrix(transformobj, HitPoint);
					ret.tHit = length(ret.intersectPoint - ray.pos);
					ret.normal = inverseTranspose(mat3(transformobj)) * normal;
					ret.normal = normalize(ret.normal);
					ret.intersectPos = Transform::dot(ret.normal, ray.direction) > 0.0f ? intersect_inside : intersect_outside;
				}
				else {
					t_temp = t_ca + t_hc;
					if (t_temp > NEAR) {
						vec3 HitPoint = InvPos + t_temp * InvDirection;
						vec3 normal = HitPoint - center;
						ret.intersectPoint = applyMatrix(transformobj, HitPoint);
						ret.tHit = length(ret.intersectPoint - ray.pos);
						ret.normal = inverseTranspose(mat3(transformobj)) * normal;
						ret.normal = normalize(ret.normal);
						ret.intersectPos = Transform::dot(ret.normal, ray.direction) > 0.0f ? intersect_inside : intersect_outside;
					}
				}
				return ret;
				/*
				vec3 HitPoint = InvPos + t * InvDirection;
				vec3 normal = HitPoint - center;
				ret.intersectPoint = applyMatrix(transformobj, HitPoint);
				ret.tHit = length(ret.intersectPoint - ray.pos);
				ret.normal = inverseTranspose(mat3(transformobj)) * normal;
				ret.normal = normalize(ret.normal);
				return ret;*/
			}
			else{
				//ray_origin is inside the sphere
				//inside == true
				float t = t_ca + t_hc;
				vec3 HitPoint = InvPos + t * InvDirection;
				vec3 normal = HitPoint - center;
				ret.intersectPoint = applyMatrix(transformobj, HitPoint);
				ret.tHit = length(ret.intersectPoint - ray.pos);
				ret.normal = inverseTranspose(mat3(transformobj)) * normal;
				ret.normal = normalize(ret.normal);
				ret.intersectPos = Transform::dot(ret.normal, ray.direction) > 0.0f ? intersect_inside : intersect_outside;
				return ret;
			}
		}
	}

	
	
/*
	mat3 normal_mat = inverseTranspose(mat3(transformobj));

	vec3 o = applyMatrix(inversetransform, ray.pos);
	vec3 d = normalize(applyMatrix(inversetransform, ray.pos + ray.direction) - o);

	intersectP ret;
	ret.tHit = FarFarAway;
	ret.obj = obj;

	float r2 = radius * radius;
	vec3 toSphere = center - o;
	float l2 = dot(toSphere, toSphere);

	if (l2 > r2) {
		float d2 = dot(toSphere, d);
		if (d2 <= 0.0f) {
			return ret;
		}

		float thc = r2 - l2 + d2 * d2;
		if (thc <= 0.0f) {
			return ret;
		}

		float thc_sqrt = sqrtf(thc);
		float t_temp = d2 - thc_sqrt;
		if (t_temp > FarFarAway) {
			vec3 hitpoint = o + t_temp * d;
			vec3 normal = normalize(hitpoint - center);
			ret.intersectPoint = applyMatrix(transformobj, hitpoint);
			ret.tHit = length(ret.intersectPoint - ray.pos);
			ret.normal = normalize(normal_mat * normal);
		}
		else {
			t_temp = d2 + thc_sqrt;
			if (t_temp > FarFarAway) {
				vec3 hitpoint = o + t_temp * d;
				vec3 normal = normalize(hitpoint - center);
				ret.intersectPoint = applyMatrix(transformobj, hitpoint);
				ret.tHit = length(ret.intersectPoint - ray.pos);
				ret.normal = normalize(normal_mat * normal);
			}
		}
		return ret;
	}
	else {
		float d2 = dot(toSphere, d);
		float thc = r2 - l2 + d2 * d2;
		float t_temp = sqrtf(thc) + d2;
		if (t_temp > FarFarAway) {
			vec3 hitpoint = o + t_temp * d;
			vec3 normal = normalize(hitpoint - center);
			ret.intersectPoint = applyMatrix(transformobj, hitpoint);
			ret.tHit = length(ret.intersectPoint - ray.pos);
			ret.normal = normalize(normal_mat * normal);
		}
		return ret;
	}
	*/
}