#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	return glm::dvec3(1.0, 1.0, 1.0);
}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3& P) const
{

	double d = glm::distance(position, P);
	double d2 = pow(d, 2.0);

	double att = 1 /((constantTerm) +
			(linearTerm * d) +
			(quadraticTerm * d2));
	return min(att, 1.0);
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.


	isect I = isect();
	glm::dvec3 d = glm::normalize(position - p);
	ray shad = ray(p, d, r.getPixel(), r.getAtten(), ray::SHADOW);

	glm::dvec3 iPos = shad.at(I.t);

	double light_source = glm::distance(position, shad.getPosition());
	double Q = glm::distance(iPos, shad.getPosition());

	if(Q < light_source){
		return glm::dvec3(0,0,0);
	}

	return glm::dvec3(1,1,1);
}

#define VERBOSE 0
