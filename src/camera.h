#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include "ray.h"
#include "vec3.h"
#include "raymath.h"


class camera
{
public:
	vec3	origin;
	vec3	lowerLeftCorner;
	vec3	horizontal;
	vec3	vertical;
	vec3	u, v, w;
	float	lensRadius;

	camera( vec3 lookFrom, vec3 lookAt, vec3 vup, float vfov, float aspect, float aperture, float focusDist )
	{
		lensRadius = 0.5f * aperture;
		float theta = vfov * float( M_PI ) / 180.0f;
		float halfHeight = tan( theta * 0.5f );
		float halfWidth = aspect * halfHeight;
		origin = lookFrom;
		w = normalize( lookFrom - lookAt );
		u = normalize( cross( vup, w ) );
		v = cross( w, u );
		lowerLeftCorner = origin - halfWidth * focusDist * u - halfHeight * focusDist * v - focusDist * w;
		horizontal = 2.0f * halfWidth * focusDist * u;
		vertical = 2.0f * halfHeight * focusDist * v;
	}

	ray getRay( float s, float t )
	{
		vec3 rd = lensRadius * rayMath::getRandomInUnitDisk();
		vec3 offset = u * rd.x() + v * rd.y();
		return ray( origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset );
	}
};
