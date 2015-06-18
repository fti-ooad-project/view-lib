#pragma once
extern "C"
{
	void calcLennardForceVector( void const *pos , void *out_force , int count );
	void LennardForceRelease();
	void LennardForceInit( int n );
}