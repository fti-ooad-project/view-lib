#pragma once
#include <cudalinalg\vec.h>
class VectorFactory
{
public:
	LOCAL static void init();
	DEVICE static float getRandom( unsigned int );
	DEVICE static df3 getRandomHalfSphere( unsigned int );
	DEVICE static df3 getRandomSphere( unsigned int );
	DEVICE static df3 getRandomCosHalfSphere( unsigned int );
	DEVICE static df2 getRandomCircle( unsigned int );
	DEVICE static df3 getReflected( df3 const & , df3 const & );
	DEVICE static df3 getRefracted( df3 const & , df3 const & , float );
	DEVICE static df3 getDiffuseReflected( df3 const & , df3 const & , float , unsigned int );
	DEVICE static df3 getDiffuseRefracted( df3 const & , df3 const & , float , float , unsigned int );
	DEVICE static float getPI();
};