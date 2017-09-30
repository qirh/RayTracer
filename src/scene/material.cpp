#include "material.h"
#include "ray.h"
#include "light.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

#include "../fileio/images.h"
#include <glm/gtx/io.hpp>
#include <iostream>
#include <algorithm>

using namespace std;
extern bool debugMode;

Material::~Material()
{
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene *scene, const ray& r, const isect& i) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.
	//	if( debugMode )
	//		std::cout << "Debugging Phong code..." << std::endl;

	// When you're iterating through the lights,
	// you'll want to use code that looks something
	// like this:
	//

	glm::dvec3 p  = r.at(i.t);
	glm::dvec3 N = i.N;
	glm::normalize(N);

	//init shades
	glm::dvec3 emission = ke(i);
	glm::dvec3 ambient_Term = ka(i) *( scene->ambient()); //ka * ki
	glm::dvec3 diffuse = kd(i);			//I * kd * max(L dot N , 0)
	glm::dvec3 specular = ks(i);

 	glm::dvec3 view = scene->getCamera().getEye() - p;
 	view = glm::normalize(view);
 	double shine = shininess(i);

	glm::dvec3 reflection = glm::dvec3(0, 0, 0);
	glm::dvec3 shadow_attenuation = glm::dvec3(0, 0, 0);

	glm::vec3 phong = emission + ambient_Term;



	for ( vector<Light*>::const_iterator litr = scene->beginLights();
		litr != scene->endLights(); ++litr )

	{
		Light* pLight = *litr;
		glm::dvec3 lightDirection = pLight->getDirection(p);
		glm::normalize(lightDirection);

		double distance_atten = pLight -> distanceAttenuation(p);

		reflection = (glm::dot(lightDirection, N) * N * 2.0) - lightDirection;
		glm::normalize(reflection);
		shadow_attenuation = pLight->shadowAttenuation(r, p);
		glm::dvec3 intensity = distance_atten * shadow_attenuation;
		
			phong += diffuse * max((glm::dot(lightDirection, N)), 0.0) * intensity;
			phong += specular * pow(max(glm::dot(view, 	reflection), 0.0), shine) * intensity;
	}
	return (phong);
}

TextureMap::TextureMap( string filename )
{
	data = readImage(filename.c_str(), width, height);
	if (data == NULL) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("'.");
		throw TextureMapException(error);
	}
}

glm::dvec3 TextureMap::getMappedValue( const glm::dvec2& coord ) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.

	return glm::dvec3(1,1,1);
}


glm::dvec3 TextureMap::getPixelAt( int x, int y ) const
{
    // This keeps it from crashing if it can't load
    // the texture, but the person tries to render anyway.
    if (0 == data)
      return glm::dvec3(1.0, 1.0, 1.0);

    if( x >= width )
       x = width - 1;
    if( y >= height )
       y = height - 1;

    // Find the position in the big data array...
    int pos = (y * width + x) * 3;
    return glm::dvec3(double(data[pos]) / 255.0,
       double(data[pos+1]) / 255.0,
       double(data[pos+2]) / 255.0);
}

glm::dvec3 MaterialParameter::value( const isect& is ) const
{
    if( 0 != _textureMap )
        return _textureMap->getMappedValue( is.uvCoordinates );
    else
        return _value;
}

double MaterialParameter::intensityValue( const isect& is ) const
{
    if( 0 != _textureMap )
    {
        glm::dvec3 value( _textureMap->getMappedValue( is.uvCoordinates ) );
        return (0.299 * value[0]) + (0.587 * value[1]) + (0.114 * value[2]);
    }
    else
        return (0.299 * _value[0]) + (0.587 * _value[1]) + (0.114 * _value[2]);
}
