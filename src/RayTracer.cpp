// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <string.h> // for memset

#include <iostream>
#include <fstream>

using namespace std;
extern TraceUI* traceUI;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

// Trace a top-level ray through pixel(i,j), i.e. normalized window coordinates (x,y),
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.

glm::dvec3 RayTracer::trace(double x, double y, unsigned char *pixel)
{
	// Clear out the ray cache in the scene for debugging purposes,
	if (TraceUI::m_debug)
		scene->intersectCache.clear();

	ray r(glm::dvec3(0,0,0), glm::dvec3(0,0,0), pixel, glm::dvec3(1,1,1), ray::VISIBILITY);

	scene->getCamera().rayThrough(x,y,r);
	double dummy;

	glm::dvec3 ret = traceRay(r, glm::dvec3(1.0,1.0,1.0), traceUI->getDepth(), dummy);
	ret = glm::clamp(ret, 0.0, 1.0);

	return ret;
}

glm::dvec3 RayTracer::tracePixel(int i, int j)
{

	glm::dvec3 col(0,0,0);

	if( ! sceneLoaded() ) return col;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;
	col = trace(x, y, pixel);

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);


	return col;
}

#define VERBOSE 0

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
glm::dvec3 RayTracer::traceRay(ray& r, const glm::dvec3& thresh, int depth, double& t )
{
	isect i;
	glm::dvec3 colorC;
#if VERBOSE
	std::cerr << "== current depth: " << depth << std::endl;
#endif

	if(scene->intersect(r, i)) {
		// YOUR CODE HERE

		// An intersection occurred!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.



		const Material& m = i.getMaterial();
		colorC = m.shade(scene.get(), r, i);
		//std::cout << "Depth: " << depth << std::endl;
		if (depth <=0) {
			//std::cout << "getDepth: " << traceUI->getDepth() << std::endl;
			return colorC;
		}

		//START REFLECTION
			//std::cout << "Depth: " << depth << std::endl;
			/*std::cout << "i.t: " << i.t << std::endl;
			std::cout << "r.at(i.t): " << r.at(i.t)[0] << ", " << r.at(i.t)[1] << ", " << r.at(i.t)[2] << std::endl;*/
		glm::dvec3 q = r.at(i.t);
		glm::dvec3 N = i.N;
		glm::dvec3 d = -1.0 * r.getDirection();
		glm::normalize(d);

		//glm::dvec3 reflection = (glm::dot(d, N) * N * 2.0) - d;
		glm::dvec3 reflection = glm::reflect(-d, N);
		glm::normalize(reflection);

		ray reflect_ray = ray(q, reflection, nullptr, glm::dvec3(1,1,1), ray::REFLECTION);

		if(glm::length(m.kr(i))!=0){

			colorC += m.kr(i)*traceRay(reflect_ray, thresh, depth-1, t);
		}


		//START REFRACTION
		if(glm::length(m.kt(i))!=0){
			std::cout << "Needs refrction!" << std::endl;
			double cos = acos(glm::dot(N, r.getDirection())) * 180/M_PI;
		}

		return colorC;


	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
		//
		// FIXME: Add CubeMap support here.

		colorC = glm::dvec3(0.0, 0.0, 0.0);
		return colorC;
	}
#if VERBOSE
	std::cerr << "== depth: " << depth+1 << " done, returning: " << colorC << std::endl;
#endif
	return colorC;
}

RayTracer::RayTracer()
	: scene(nullptr), buffer(0), thresh(0), buffer_width(256), buffer_height(256), m_bBufferReady(false), cubemap(nullptr)
{
}

RayTracer::~RayTracer()
{
	delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn ) {
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}

	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos ) path = ".";
	else path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
	Parser parser( tokenizer, path );
	try {
		scene.reset(parser.parseScene());
	}
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	if (!sceneLoaded())
		return false;

	return true;
}

void RayTracer::traceSetup(int w, int h)
{
	if (buffer_width != w || buffer_height != h)
	{
		buffer_width = w;
		buffer_height = h;
		bufferSize = buffer_width * buffer_height * 3;
		delete[] buffer;
		buffer = new unsigned char[bufferSize];
	}
	memset(buffer, 0, w*h*3);
	m_bBufferReady = true;
}

void RayTracer::traceImage(int w, int h, int bs, double thresh)
{
	// YOUR CODE HERE
	// FIXME: Start one or more threads for ray tracing

	traceSetup(w, h);

	for(int i = 0; i <= w; ++i){
		for(int j = 0; j <= h; ++j){
			tracePixel(i, j);
		}
	}
}

int RayTracer::aaImage(int samples, double aaThresh)
{
	int aa_number = traceUI->getSuperSamples();

	cout << "aa_number" << aa_number << endl;
	return 0;
}

bool RayTracer::checkRender()
{
	// YOUR CODE HERE
	// FIXME: Return true if tracing is done.
	return true;
}

glm::dvec3 RayTracer::getPixel(int i, int j)
{
	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;
	return glm::dvec3((double)pixel[0]/255.0, (double)pixel[1]/255.0, (double)pixel[2]/255.0);
}

void RayTracer::setPixel(int i, int j, glm::dvec3 color)
{
	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * color[0]);
	pixel[1] = (int)( 255.0 * color[1]);
	pixel[2] = (int)( 255.0 * color[2]);
}
