#pragma once

#include "ray.h"
#include "vec3.h"

class material;


struct hitRecord
{
	float		t;
	vec3		p;
	vec3		normal;
	material*	mat;
};

class hitable
{
public:
	virtual bool hit( const ray& r, float mint, float maxt, hitRecord& rec ) const = 0;
};

class hitableList : public hitable
{
public:
	hitableList() {}
	hitableList( hitable** l, int s )
	{
		list = l;
		listSize = s;
	}

	virtual bool hit( const ray& r, float mint, float maxt, hitRecord& rec ) const override;

	hitable** list;
	int listSize;

};

bool hitableList::hit( const ray& r, float mint, float maxt, hitRecord& rec ) const
{
	hitRecord tempRec;
	bool hitAnything = false;
	float closestSoFar = maxt;

	// Iterate through all hitables and find closest one to camera
	for( int i = 0; i < listSize; i++ )
	{
		if( list[i]->hit( r, mint, closestSoFar, tempRec ) )
		{
			hitAnything = true;
			closestSoFar = tempRec.t;
			rec = tempRec;
		}
	}

	return hitAnything;
}

/************************************************************************/
/* Specific hitable object implementations
/************************************************************************/

class sphere : public hitable
{
public:
	vec3		center;
	float		radius;
	material*	mat;

	sphere() {}
	sphere( vec3 c, float r, material* m ) : center( c ), radius( r ), mat( m ) {}

	virtual bool hit( const ray& r, float mint, float maxt, hitRecord& rec ) const override;
};

bool sphere::hit( const ray& r, float mint, float maxt, hitRecord& rec ) const
{
	vec3 oc = r.origin() - center;
	float a = dot( r.direction(), r.direction() );
	float b = dot( oc, r.direction() );
	float c = dot( oc, oc ) - radius * radius;
	float discriminant = b * b - a * c;
	if( discriminant > 0 )
	{
		float temp = (-b - sqrt( b*b - a * c )) / a;
		if( temp < maxt && temp > mint )
		{
			rec.t = temp;
			rec.p = r.pointAtParameter( rec.t );
			rec.normal = (rec.p - center) / radius;
			rec.mat = mat;
			return true;
		}
		temp = (-b + sqrt( b*b - a * c )) / a;
		if( temp < maxt && temp > mint )
		{
			rec.t = temp;
			rec.p = r.pointAtParameter( rec.t );
			rec.normal = (rec.p - center) / radius;
			rec.mat = mat;
			return true;
		}
	}
	
	return false;
}
