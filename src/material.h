#pragma once

#include "ray.h"
#include "vec3.h"
#include "hitable.h"
#include "raymath.h"

class material
{
public:
	virtual bool scatter( const ray& r, const hitRecord& rec, vec3& attenuation, ray& scattered ) const = 0;
};


/************************************************************************/
/* Specific material implementations
/************************************************************************/

class lambertian : public material
{
public:
	vec3 albedo;

	lambertian( const vec3& a ) : albedo( a ) {}

	virtual bool scatter( const ray& r, const hitRecord& rec, vec3& attenuation, ray& scattered ) const override
	{
		vec3 target = rec.p + rec.normal + rayMath::getRandomPointInSphere();
		scattered = ray( rec.p, target - rec.p );
		attenuation = albedo;
		return true;
	}
};

class metal : public material
{
public:
	vec3		albedo;
	float		fuzz;

	metal( const vec3& a, float f ) : albedo( a ), fuzz( f ) {}

	virtual bool scatter( const ray& r, const hitRecord& rec, vec3& attenuation, ray& scattered ) const override
	{
		vec3 reflected = reflect( normalize( r.direction() ), rec.normal );
		scattered = ray( rec.p, reflected + fuzz * rayMath::getRandomPointInSphere() );
		attenuation = albedo;
		return (dot( scattered.direction(), rec.normal ) > 0.0f);
	}
};

class dielectric : public material
{
public:
	float		indexOfRefraction;

	dielectric( float ir ) : indexOfRefraction( ir ) {}

	virtual bool scatter( const ray& r, const hitRecord& rec, vec3& attenuation, ray& scattered ) const override
	{
		vec3 outwardNormal;
		vec3 reflected = reflect( r.direction(), rec.normal );
		float nint;
		attenuation = vec3( 1.0f, 1.0f, 1.0f );	// BUG! Fix blue channel to be 1.0f
		vec3 refracted;
		float reflectProb;
		float cosine;

		if( dot( r.direction(), rec.normal ) > 0.0f )
		{
			outwardNormal = -rec.normal;
			nint = indexOfRefraction;
			cosine = indexOfRefraction * dot( r.direction(), rec.normal ) / r.direction().length();
		}
		else
		{
			outwardNormal = rec.normal;
			nint = 1.0f / indexOfRefraction;
			cosine = -dot( r.direction(), rec.normal ) / r.direction().length();
		}
		
		if( refract( r.direction(), outwardNormal, nint, refracted ) )
		{
			reflectProb = rayMath::schlick( cosine, indexOfRefraction );
		}
		else
		{
			reflectProb = 1.0f;
		}

		// Randomly reflect or refract the ray (if it can be refracted)
		if( rayMath::getRandom() < reflectProb )
		{
			scattered = ray( rec.p, reflected );
		}
		else
		{
			scattered = ray( rec.p, refracted );
		}

		return true;
	}
};
