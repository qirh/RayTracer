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

	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity
	// of the light based on the distance between the source and the
	// point P.  For now, we assume no attenuation and just return 1.0


<<<<<<< HEAD
	double d =sqrt(pow((position[0]-P[0]), 2.0) + 
		      	pow((position[1]-P[1]), 2.0) + 
		      	pow((position[2]-P[2]), 2.0)
		      	);
	double d2 = pow(d, 2.0);

	double att = 1/(constantTerm +
			(linearTerm * d) +
			(quadraticTerm * d2));

	return min(att, 1.0);

	//return 1.0;
=======
	return 1.0;
>>>>>>> b407fc87b4345c3db82c871f3156ac34cb6f6a50
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
	return glm::dvec3(1,1,1);
}

#define VERBOSE 0
