#include <cmath>
#include <float.h>
#include <algorithm>
#include <assert.h>
#include <string.h>
#include "trimesh.h"
#include "../ui/TraceUI.h"
#include <iostream>
extern TraceUI* traceUI;

using namespace std;

Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const glm::dvec3 &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const glm::dvec3 &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt ) return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    if (!newFace->degen) faces.push_back( newFace );


    // Don't add faces to the scene's object list so we can cull by bounding box
    // scene->add(newFace);
    return true;
}

const char* Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	typedef Faces::const_iterator iter;
	bool have_one = false;
	for( iter j = faces.begin(); j != faces.end(); ++j )
	{
		isect cur;
		if( (*j)->intersectLocal( r, cur ) )
		{
			if( !have_one || (cur.t < i.t) )
			{
				i = cur;
				have_one = true;
			}
		}
	}
	if( !have_one ) i.setT(1000.0);
	return have_one;
}

bool TrimeshFace::intersect(ray& r, isect& i) const {
  return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{
    // YOUR CODE HERE
    //
    // FIXME: Add ray-trimesh intersection

    glm::dvec3 a = parent->vertices[ids[0]];
    glm::dvec3 b = parent->vertices[ids[1]];
    glm::dvec3 c = parent->vertices[ids[2]];

    glm::dvec3 n = normal;

    double dist = -(glm::dot(n, a));


    /*  get from notes
    * t = - (n * P + d)/(n*D)
    */
    double t = -(glm::dot(n, r.getPosition())
                 + dist) / (glm::dot(n, r.getDirection()));
    
    if(t < RAY_EPSILON) return false; 

    glm::dvec3 p = r.at(t);
    glm::dvec3 v0 = b - a;
    glm::dvec3 v1 = c - a;
    glm::dvec3 v2 = p - a;

    //Christer Ericon's Real-Time Collision Detection: http://gamedev.stackexchange.com/a/23745
    double d00 = glm::dot(v0, v0);
    double d01 = glm::dot(v0, v1);
    double d11 = glm::dot(v1, v1);
    double d20 = glm::dot(v2, v0);
    double d21 = glm::dot(v2, v1);

    double denom = d00 * d11 - d01 * d01;
    const double v = (d11 * d20 - d01 * d21) / denom;
    const double w = (d00 * d21 - d01 * d20) / denom;
    const double u = 1.0 - v - w;

    if((v + w > 1.0) || v < 0 || w < 0) return false;

    i.setBary(u, v, w);
    i.setT(t);
    i.setN(n);
    i.setObject(this);

    if(parent->materials.empty()) i.setMaterial(getMaterial());
    else i.setMaterial(*parent->materials[ids[0]]);


    return true;
}

void Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );

    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
		glm::dvec3 faceNormal = (**fi).getNormal();

        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
    vertNorms = true;
}

