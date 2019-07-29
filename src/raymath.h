#pragma once

#include <random>
#include "vec3.h"

namespace rayMath
{
	float getRandom()
	{
		static std::default_random_engine e;
		static std::uniform_real_distribution<float> dis( 0.0f, 1.0f );
		return dis( e );
	}

	vec3 getRandomPointInSphere()
	{
		vec3 p;
		do
		{
			p = 2.0f * vec3( getRandom(), getRandom(), getRandom() ) - vec3::one;
		} while( p.lengthSqr() >= 1.0f );
		return p;
	}

	float schlick( float cosine, float ior )
	{
		float r0 = (1.0f - ior) / (1.0f + ior);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * pow( (1.0f - cosine), 5.0f );
	}

	vec3 getRandomInUnitDisk()
	{
		vec3 p;
		do 
		{
			p = 2.0f * vec3( getRandom(), getRandom(), 0.0f ) - vec3( 1.0f, 1.0f, 0.0f );
		} while ( dot( p, p ) >= 1.0f );
		return p;
	}
}
