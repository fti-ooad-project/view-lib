#include "linalg/vec.h"
#include<curand_kernel.h>
DEVICE float PI;
//#define RAND_COUNT 0x1000
#define RANDSTATE_COUNT 0x100
//__device__ float randf[ RAND_COUNT ];
DEVICE curandState randstate[ RANDSTATE_COUNT ];
//__device__ unsigned int cur_rand;
DEVICE float wrap( float x , float min , float max )
{
	if( x > max ) return max;
	if( x < min ) return min;
	return x;
}
DEVICE float sqr( float i )
{
	return i * i;
}
GLOBAL void devInit()
{
	PI = acosf( -1.0f );
}
GLOBAL void randInit( unsigned int n )
{
	unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
	if( i < n )
	{
		curand_init( 93735373UL + 56733797UL * i , i , 0 , randstate + i );
	}
}
LOCAL void VectorFactory::init()
{
	devInit << < 1 , 1 >> >();
	unsigned int thread_count = min( RANDSTATE_COUNT , 1024 );
	unsigned int block_count = ( RANDSTATE_COUNT + thread_count - 1 ) / thread_count;
	randInit << < block_count , thread_count >> >( RANDSTATE_COUNT );
}
DEVICE float VectorFactory::getRandom( unsigned int seed )
{
	return curand_uniform( randstate + ( seed % RANDSTATE_COUNT ) );
}
DEVICE float VectorFactory::getPI()
{
	return PI;
}
DEVICE f3 VectorFactory::getRandomHalfSphere( unsigned int seed )
{
	float phi = getRandom( seed ) * PI * 2.0f;
	float cp = cosf( phi );
	float sp = sinf( phi );
	float ct = getRandom( seed );
	float st = sqrtf( 1.0f - ct * ct );
	return f3( st * cp , st * sp , ct );
}
DEVICE f3 VectorFactory::getRandomSphere( unsigned int seed )
{
	float phi = getRandom( seed ) * PI * 2.0f;
	//float theta = PI * 0.5f + asinf( 2.0f * getRandom() - 1.0f );
	float cp = cosf( phi );
	float sp = sinf( phi );
	float ct = -1.0f + 2.0f * getRandom( seed );
		//cosf( theta );
	float st = sqrtf( 1.0f - ct * ct );
		//sinf( theta );
	return f3( st * cp , st * sp , ct );
}
DEVICE f2 VectorFactory::getRandomCircle( unsigned int seed )
{
	float phi = getRandom( seed ) * PI * 2.0f;
	float r = powf( getRandom( seed ) , 0.5f );
	return f2( cos( phi ) , sin( phi ) ) * r;
}
DEVICE f3 VectorFactory::getReflected( f3 const &v , f3 const &n )
{
	return v - 2.0f * n * ( n * v );
}