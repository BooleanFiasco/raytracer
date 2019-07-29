// RayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "camera.h"
#include "raymath.h"
#include "material.h"

namespace colors
{
	static const vec3 white( 1.0f, 1.0f, 1.0f );
	static const vec3 black( 0.0f, 0.0f, 0.0f );
	static const vec3 red( 1.0f, 0.0f, 0.0f );
	static const vec3 green( 0.0f, 1.0f, 0.0f );
	static const vec3 blue( 0.0f, 0.0f, 1.0f );

	// Customized colors
	static const vec3 backgroundBlue( 0.5f, 0.7f, 1.0f );
}

// Returns the default background color for a given ray
// It's a simple lerp from blue to white going from top (+y) to bottom (-y)
vec3 color( const ray& r, hitable* world, int depth )
{
	hitRecord rec;
	if( world->hit( r, 0.001f, FLT_MAX, rec ) )
	{
		ray scattered;
		vec3 attenuation;
		if( depth < 50 && rec.mat->scatter( r, rec, attenuation, scattered ) )
		{
			return attenuation * color( scattered, world, depth + 1 );
		}
		else
		{
			return vec3::zero;
		}
	}
	else
	{
		vec3 unitDirection = normalize( r.direction() );
		float t = 0.5f * (unitDirection.y() + 1.0f);
		return (1.0f - t) * colors::white + t * colors::backgroundBlue;
	}
}

hitable* makeRandomScene()
{
	int n = 500;
	hitable** list = new hitable*[n + 1];
	list[0] = new sphere( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, new lambertian( vec3( 0.5f, 0.5f, 0.5f ) ) );

	int i = 1;
	for( int a = -11; a < 11; a++ )
	{
		for( int b = -11; b < 11; b++ )
		{
			float chooseMat = rayMath::getRandom();
			vec3 center( a + 0.9f * rayMath::getRandom(), 0.2f, b + 0.9f * rayMath::getRandom() );
			if( (center - vec3( 4.0f, 0.2f, 0.0f )).length() > 0.9f )
			{
				if( chooseMat < 0.8f )
				{
					// Selected diffuse
					list[i++] = new sphere(
						center,
						0.2f,
						new lambertian( vec3(
							rayMath::getRandom() * rayMath::getRandom(),
							rayMath::getRandom() * rayMath::getRandom(),
							rayMath::getRandom() * rayMath::getRandom() )
						)
					);
				}
				else if( chooseMat < 0.95f )
				{
					// Selected metallic
					list[i++] = new sphere(
						center,
						0.2f,
						new metal( vec3(
							0.5f * (1.0f + rayMath::getRandom()),
							0.5f * (1.0f + rayMath::getRandom()),
							0.5f * (1.0f + rayMath::getRandom()) ),
							0.5f * rayMath::getRandom() )
					);
				}
				else
				{
					// Selected glass
					list[i++] = new sphere( center, 0.2f, new dielectric( 1.5f ) );
				}
			}
		}
	}

	list[i++] = new sphere( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new dielectric( 1.5f ) );
	list[i++] = new sphere( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) );
	list[i++] = new sphere( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) );

	return new hitableList( list, i );
}

int main()
{
	// Define a camera view that's 200px wide and 100px tall
	int nx = 200;
	int ny = 100;
	int ns = 100;	// what is this? number of AA samples, right?
	vec3 lowerLeft( -2.0f, -1.0f, -1.0f );
	vec3 horizontal( 4.0f, 0.0f, 0.0f );
	vec3 vertical( 0.0f, 2.0f, 0.0f );
	vec3 origin( 0.0f, 0.0f, 0.0f );

	// Define list of hitable objects in the world
	/*
	float R = cos( float( M_PI ) / 4.0f );
	hitable* list[5];
	list[0] = new sphere( vec3( 0.0f, 0.0f, -1.0f ), 0.5f, new lambertian( vec3( 0.1f, 0.2f, 0.5f ) ) );
	list[1] = new sphere( vec3( 0.0f, -100.5f, -1.0f ), 100.0f, new lambertian( vec3( 0.8f, 0.8f, 0.0f ) ) );
	list[2] = new sphere( vec3( 1.0f, 0.0f, -1.0f ), 0.5f, new metal( vec3( 0.8f, 0.6f, 0.2f ), 0.3f ) );
	list[3] = new sphere( vec3( -1.0f, 0.0f, -1.0f ), 0.5f, new dielectric( 1.5f ) );
	list[4] = new sphere( vec3( -1.0f, 0.0f, -1.0f ), -0.45f, new dielectric( 1.5f ) );

	//list[0] = new sphere( vec3( -R, 0.0f, -1.0f ), R, new lambertian( vec3( colors::blue ) ) );
	//list[1] = new sphere( vec3( R, 0.0f, -1.0f ), R, new lambertian( vec3( colors::red ) ) );

	hitable* world = new hitableList( list, 5 );
	*/

	hitable* world = makeRandomScene();

	// Create our camera
	vec3 lookFrom( 3.0f, 3.0f, 2.0f );
	vec3 lookAt( 0.0f, 0.0f, -1.0f );
	float aperture = 2.0f;
	float distToFocus = (lookFrom - lookAt).length();
	camera cam( lookFrom, lookAt, vec3( 0.0f, 1.0f, 0.0f ), 20.0f, float( nx ) / float( ny ), aperture, distToFocus );

	// Write PCM image file
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	for( int j = ny - 1; j >= 0; j-- )
	{
		for( int i = 0; i < nx; i++ )
		{
			vec3 clr = colors::black;
			for( int s = 0; s < ns; s++ )
			{
				float u = float( i + rayMath::getRandom() ) / float( nx );
				float v = float( j + rayMath::getRandom() ) / float( ny );
				ray r = cam.getRay( u, v );
				vec3 p = r.pointAtParameter( 2.0f );
				clr += color( r, world, 0 );
			}
			
			// Average color for AA purposes
			clr /= float( ns );

			// Gamma correction
			clr = vec3( sqrt( clr[0] ), sqrt( clr[1] ), sqrt( clr[2] ) );

			int ir = int( 255.99 * clr[0] );
			int ig = int( 255.99 * clr[1] );
			int ib = int( 255.99 * clr[2] );

			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}
}
