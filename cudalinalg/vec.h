#ifndef VEC_H
#define VEC_H
#include "cuda_runtime.h"
#define DEVICE __device__
#define GLOBAL __global__
#define LOCAL __host__
#ifndef ito
#define ito( x ) for( int i = 0; i < x; i++ )
#endif
#ifndef xfor
#define xfor( x , n ) for( int x = 0; x < n; x++ )
#endif
//#define PRINT( x ) printf( "%f\n" , static_cast< float >( x ) );
//#define PRINT( x ) printf( "%i\n" , static_cast< int >( x ) );
#define X( v ) v.getValue( 0 )
#define Y( v ) v.getValue( 1 )
#define DELTA 0.00000001f
#define BETHA 99999999999.0f
DEVICE float c_wrap( float , float , float );
DEVICE float c_sqr( float );
template< int N , typename T >
struct array
{
protected:
	T __data[ N ];
public:
	DEVICE T &getValue( int i )
	{
		return __data[ i ];
	}
	DEVICE T const &getValue( int i ) const
	{
		return __data[ i ];
	}
	DEVICE T &x()
	{
		return __data[ 0 ];
	}
	DEVICE T const &x() const
	{
		return __data[ 0 ];
	}
	DEVICE T &y()
	{
		return __data[ 1 ];
	}
	DEVICE T const &y() const
	{
		return __data[ 1 ];
	}
	DEVICE T &z()
	{
		return __data[ 2 ];
	}
	DEVICE T const &z() const
	{
		return __data[ 2 ];
	}
	DEVICE T &w()
	{
		return __data[ 3 ];
	}
	DEVICE T const &w() const
	{
		return __data[ 3 ];
	}
	DEVICE T const *getArray() const
	{
		return __data;
	}
};
template< int N , typename T >
struct accessor : public array < N , T >
{
};
template< int N >
struct unpacker
{
	template< typename T , typename P , typename ...S >
	DEVICE static void unpack( T *data , int C , P x , S ...arg )
	{
		data[ C - N ] = static_cast< T >( x );
		unpacker< N - 1 >::unpack( data , C , arg... );
	}
};
template<>
struct unpacker < 1 >
{
	template< typename T , typename P >
	DEVICE static void unpack( T *data , int C , P x )
	{//PRINT( x ) PRINT( C - 1 )
		data[ C - 1 ] = static_cast< T >( x );
	}
};
template< int N , typename T >
struct vec : public accessor < N , T >
{
	DEVICE T const &getX() const
	{
		return this->getValue( 0 );
	}
	DEVICE T &getX()
	{
		return this->getValue( 0 );
	}
	DEVICE T const &getY() const
	{
		return this->getValue( 1 );
	}
	DEVICE T &getY()
	{
		return this->getValue( 1 );
	}
	/*DEVICE void print() const
	{
	ito( N )
	std::cout << this->getValue( i ) << " ";
	std::cout << "\n";
	}*/
	DEVICE vec()
	{
		//memset( this->__data , 0 , N * sizeof( T ) );
	}
	DEVICE vec( T d )
	{
		ito( N )
			this->__data[ i ] = d;
	}
	DEVICE vec( vec< N , T > const &v )
	{
		ito( N )
			this->getValue( i ) = v.getValue( i );
	}
	template< typename P >
	DEVICE void operator=( vec< N , P > const &v )
	{
		ito( N )
			this->getValue( i ) = static_cast< T >( v.getValue( i ) );
	}
	template< typename ...D >
	DEVICE  vec( D ...arg )
	{
		unpacker< N >::unpack( this->__data , N , arg... );
	}
	template< int M >
	DEVICE vec< M , T >
		getVec( int a , int b ) const
	{
		vec< M , T > out;
		for( int i = a; i < b; i++ )
			out.getValue( i ) = this->getValue( i );
		return out;
	}
	DEVICE void mul( float k )
	{
		for( int i = 0; i < N; i++ )
			this->getValue( i ) *= k;
	}
	DEVICE vec< N , T > g_mul( float k ) const
	{
		vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * k;
		return out;
	}
	DEVICE float g_dist2( vec< N , T > const &v ) const
	{
		float d = 0.0f;
		for( int i = 0; i < N; i++ )
			d += c_sqr( this->getValue( i ) - v.getValue( i ) );
		return d;
	}
	DEVICE vec< N , T > g_add( vec< N , T > const &v ) const
	{
		vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) + v.getValue( i );
		return out;
	}
	DEVICE void add( vec< N , T > const &v )
	{
		for( int i = 0; i < N; i++ )
			this->getValue( i ) = this->getValue( i ) + v.getValue( i );
	}
	DEVICE float g_dist( vec< N , T > const &v ) const
	{
		return sqrtf( this->g_dist2( v ) );
	}
	DEVICE float g_mod2() const
	{
		float d = 0.0f;
		for( int i = 0; i < N; i++ )
			d += c_sqr( this->getValue( i ) );
		return d;
	}
	DEVICE float g_mod() const
	{
		return sqrtf( g_mod2() );
	}
	DEVICE vec< N , T > g_norm() const
	{
		float m = this->g_mod();
		if( m < DELTA )
			return vec< N , T >( static_cast< T >( 0 ) );
		if( fabsf( m - 1.0f ) < DELTA )
			return *this;
		return this->g_mul( 1.0f / m );
	}
	DEVICE vec< N , T > operator&( vec< N , T > const &v ) const
	{
		vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * v.getValue( i );
		return out;
	}
	DEVICE vec< N , T > operator-( vec< N , T > const &v ) const
	{
		vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) - v.getValue( i );
		return out;
	}
	DEVICE vec< N , T > operator+( vec< N , T > const &v ) const
	{
		vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) + v.getValue( i );
		return out;
	}
	DEVICE vec< N , T > operator*( float k ) const
	{
		vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * k;
		return out;
	}
	DEVICE vec< N , T > operator/( float k ) const
	{
		vec< N , T > out;
		float t = 1.0f / k;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * t;
		return out;
	}
	DEVICE vec< N , T > operator/( vec< N , T > const &k ) const
	{
		vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) / k.getValue( i );
		return out;
	}
	DEVICE void operator/=( float k )
	{
		*this = *this / k;
	}
	DEVICE float operator*( vec< N , T > const &v ) const
	{
		float out = 0.0f;
		for( int i = 0; i < N; i++ )
			out += this->getValue( i ) * v.getValue( i );
		return out;
	}
	DEVICE void operator*=( float k )
	{
		for( int i = 0; i < N; i++ )
			this->getValue( i ) *= k;
	}
	DEVICE bool operator==( vec< N , T > const &v ) const
	{
		for( int i = 0; i < N; i++ )
			if( this->getValue( i ) != v.getValue( i ) ) return false;
		return true;
	}
	DEVICE bool operator!=( vec< N , T > const &v ) const
	{
		return !( *this == v );
	}
	DEVICE float operator^( vec< N , T > const &v ) const
	{
		if( this->g_dist2( v ) < DELTA ) return 0.0f;
		auto v1 = ( *this ).g_norm();
		auto v2 = v.g_norm();
		float scl = acos( c_wrap( v1 * v2 , -1.0f , 1.0f ) );
		float sgn = lperp( v1 ) * v2;
		if( sgn < 0.0f )
			scl = -scl;
		return scl;
	}
	DEVICE void operator+=( vec< N , T > const &v )
	{
		*this = *this + v;
	}
	DEVICE void operator-=( vec< N , T > const &v )
	{
		*this = *this - v;
	}
	DEVICE vec< N , T > operator-( ) const
	{
		return *this * -1.0f;
	}
	DEVICE vec< 2 , T > xy() const
	{
		return vec< 2 , T >( __data[ 0 ] , __data[ 1 ] );
	}
};
template< int N , typename T >
DEVICE vec< N , T > operator*( float k , vec< N , T > const &v )
{
	vec< N , T > out;
	for( int i = 0; i < N; i++ )
		out.getValue( i ) = v.getValue( i ) * k;
	return out;
}
template< typename T >
DEVICE vec< 2 , T > lperp( vec< 2 , T > const &v )
{
	return vec< 2 , T >( -v.getValue( 1 ) , v.getValue( 0 ) );
}
template< typename T >
DEVICE vec< 3 , T > perp_norm( vec< 3 , T > const &v , vec< 3 , T > const &p )
{
	return ( p - v * ( p * v ) ).g_norm();
}
template< typename T >
DEVICE vec< 3 , T > vecx( vec< 3 , T > const &a , vec< 3 , T > const &b )
{
	return vec< 3 , T >( a.getValue( 1 ) * b.getValue( 2 ) - b.getValue( 1 ) * a.getValue( 2 ) ,
		b.getValue( 0 ) * a.getValue( 2 ) - a.getValue( 0 ) * b.getValue( 2 ) ,
		a.getValue( 0 ) * b.getValue( 1 ) - b.getValue( 0 ) * a.getValue( 1 ) );
}
template< typename T >
DEVICE vec< 2 , T > rotate( vec< 2 , T > const &v , float a )
{
	float c = cosf( a );
	float s = sinf( a );
	return vec< 2 , T >( X( v ) * c - Y( v ) * s , Y( v ) * c + X( v ) * s );
}
template< int N , typename T >
DEVICE vec< N , T > c_wrap( vec< N , T > const &v , float min , float max )
{
	return v.g_norm() * c_wrap( v.g_mod() , min , max );
}
typedef vec< 2 , float > f2;
typedef vec< 2 , int > i2;
typedef vec< 2 , unsigned int > u2;
typedef vec< 3 , float > f3;
typedef vec< 4 , float > f4;
typedef vec< 3 , int > i3;
class VectorFactory
{
public:
	LOCAL static void init();
	DEVICE static float getRandom( unsigned int );
	DEVICE static f3 getRandomHalfSphere( unsigned int );
	DEVICE static f3 getRandomSphere( unsigned int );
	DEVICE static f3 getRandomCosHalfSphere( unsigned int );
	DEVICE static f2 getRandomCircle( unsigned int );
	DEVICE static f3 getReflected( f3 const & , f3 const & );
	DEVICE static f3 getRefracted( f3 const & , f3 const & , float );
	DEVICE static f3 getDiffuseReflected( f3 const & , f3 const & , float , unsigned int );
	DEVICE static f3 getDiffuseRefracted( f3 const & , f3 const & , float , float , unsigned int );
	DEVICE static float getPI();
};
#endif // VEC_H
