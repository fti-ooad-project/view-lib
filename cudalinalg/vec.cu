#include <cudalinalg/vec.h>
#include<curand_kernel.h>
DEVICE float PI;
//#define RAND_COUNT 0x1000
#define RANDSTATE_COUNT 0x100
//__device__ float randf[ RAND_COUNT ];
DEVICE curandState randstate[ RANDSTATE_COUNT ];
//__device__ unsigned int cur_rand;
DEVICE float c_wrap( float x , float min , float max )
{
	if( x > max ) return max;
	if( x < min ) return min;
	return x;
}
DEVICE float c_sqr( float i )
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
	float cp = cosf( phi );
	float sp = sinf( phi );
	float ct = -1.0f + 2.0f * getRandom( seed );
	float st = sqrtf( 1.0f - ct * ct );
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
DEVICE f3 VectorFactory::getRefracted( f3 const &v , f3 const &n , float kn )
{
	float cosa = -v * n;
	float sina2 = 1.0f - cosa * cosa;
	float sinb2 = sina2 * kn * kn;
	if( sinb2 > 1.0f )
		return getReflected( v , n );
	float cosb = sqrtf( 1.0f - sinb2 );
	return n * ( -cosb + cosa * kn ) + v * kn;
}
DEVICE f3 VectorFactory::getDiffuseReflected( f3 const &v , f3 const &n , float spec , unsigned int seed )
{
	f3 locx = vecx( v , n ).g_norm();
	f3 locy = vecx( locx , n );
	f3 kvec = getRandomHalfSphere( seed );
	f3 rand_vec = n * kvec.z() + locx * kvec.x() + locy * kvec.y();
	f3 refl = getReflected( v , n );
	f3 out = ( refl * spec + rand_vec * ( 1.0 - spec ) ).g_norm();
	return out;
}
DEVICE f3 VectorFactory::getDiffuseRefracted( f3 const &v , f3 const &n , float spec , float kn , unsigned int seed )
{
	f3 locx = vecx( v , n ).g_norm();
	f3 locy = vecx( locx , n );
	f3 kvec = getRandomHalfSphere( seed );
	f3 rand_vec = n * kvec.z() + locx * kvec.x() + locy * kvec.y();
	f3 refl = getRefracted( v , n , kn );
	f3 out = ( refl * spec + rand_vec * ( 1.0 - spec ) ).g_norm();
	return out;
}