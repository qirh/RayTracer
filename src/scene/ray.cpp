#include "ray.h"
#include "material.h"
#include "scene.h"
#include "../ui/TraceUI.h"

const Material &
isect::getMaterial() const
{
    return material ? *material : obj->getMaterial();
}

ray::ray(const glm::dvec3 &pp,
         const glm::dvec3 &dd,
         unsigned char *px,
         const glm::dvec3 &w,
         RayType tt)
	: p(pp), d(dd), pixel(px), atten(w), t(tt)
{
	TraceUI::addRay(ray_thread_id);
}

ray::ray(const ray& other)
	: p(other.p),
	  d(other.d),
	  pixel(other.pixel),
	  atten(other.atten)
{
	TraceUI::addRay(ray_thread_id);
}

ray& ray::operator=(const ray& other)
{
	p = other.p;
	d = other.d;
	pixel = other.pixel;
	atten = other.atten;
	t = other.t;
	return *this;
}

thread_local unsigned int ray_thread_id = 0;
