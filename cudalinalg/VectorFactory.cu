#include <cudalinalg/VectorFactory.h>
#include<curand_kernel.h>
DEVICE float PI;
//#define RAND_COUNT 0x1000
#define RANDSTATE_COUNT 0x100
//__device__ float randf[ RAND_COUNT ];
DEVICE curandState randstate[ RANDSTATE_COUNT ];
//__device__ unsigned int cur_rand;
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
DEVICE df3 VectorFactory::getRandomHalfSphere( unsigned int seed )
{
	float phi = getRandom( seed ) * PI * 2.0f;
	float cp = cosf( phi );
	float sp = sinf( phi );
	float ct = getRandom( seed );
	float st = sqrtf( 1.0f - ct * ct );
	return df3( st * cp , st * sp , ct );
}
DEVICE df3 VectorFactory::getRandomCosHalfSphere( unsigned int seed )
{
	float phi = getRandom( seed ) * PI * 2.0f;
	float cp = cosf( phi );
	float sp = sinf( phi );
	float ct = cosf( getRandom( seed ) * PI * 0.5f );
	float st = sqrtf( 1.0f - ct * ct );
	return df3( st * cp , st * sp , ct );
}
DEVICE df3 VectorFactory::getRandomSphere( unsigned int seed )
{
	float phi = getRandom( seed ) * PI * 2.0f;
	float cp = cosf( phi );
	float sp = sinf( phi );
	float ct = -1.0f + 2.0f * getRandom( seed );
	float st = sqrtf( 1.0f - ct * ct );
	return df3( st * cp , st * sp , ct );
}
DEVICE df2 VectorFactory::getRandomCircle( unsigned int seed )
{
	float phi = getRandom( seed ) * PI * 2.0f;
	float r = powf( getRandom( seed ) , 0.5f );
	return df2( cos( phi ) , sin( phi ) ) * r;
}
DEVICE df3 VectorFactory::getReflected( df3 const &v , df3 const &n )
{
	return v - 2.0f * n * ( n * v );
}
DEVICE df3 VectorFactory::getRefracted( df3 const &v , df3 const &n , float kn )
{
	float cosa = -v * n;
	float sina2 = 1.0f - cosa * cosa;
	float sinb2 = sina2 * kn * kn;
	if( sinb2 > 1.0f )
		return getReflected( v , n );
	float cosb = sqrtf( 1.0f - sinb2 );
	return n * ( -cosb + cosa * kn ) + v * kn;
}
DEVICE df3 VectorFactory::getDiffuseReflected( df3 const &v , df3 const &n , float spec , unsigned int seed )
{
	df3 locx = vecx( v , n ).g_norm();
	df3 locy = vecx( locx , n );
	df3 kvec = getRandomCosHalfSphere( seed );
	df3 rand_vec = n * kvec.z() + locx * kvec.x() + locy * kvec.y();
	df3 refl = getReflected( v , n );
	df3 out = ( refl * spec + rand_vec * ( 1.0 - spec ) ).g_norm();
	return out;
}
DEVICE df3 VectorFactory::getDiffuseRefracted( df3 const &v , df3 const &n , float spec , float kn , unsigned int seed )
{
	df3 locx = vecx( v , n ).g_norm();
	df3 locy = vecx( locx , n );
	df3 kvec = getRandomCosHalfSphere( seed );
	df3 rand_vec = n * kvec.z() + locx * kvec.x() + locy * kvec.y();
	df3 refl = getRefracted( v , n , kn );
	df3 out = ( refl * spec + rand_vec * ( 1.0 - spec ) ).g_norm();
	return out;
}