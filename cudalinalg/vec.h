#ifndef CUDAVEC_H
#define CUDAVEC_H
#include "cuda_runtime.h"
#include <cudalib\cudaUtil.h>
#ifndef ito
#define ito( x ) for( int i = 0; i < x; i++ )
#endif
#ifndef xfor
#define xfor( x , n ) for( int x = 0; x < n; x++ )
#endif
#ifndef DELTA
#define DELTA 0.00000001f
#endif
#ifndef BETHA
#define BETHA 99999999999.0f
#endif
#ifndef dev_wrap
MULTYDEV inline float dev_wrap( float x , float min , float max )
{
	if( x > max ) return max;
	if( x < min ) return min;
	return x;
}
#endif
#ifndef dev_sqr
MULTYDEV inline float dev_sqr( float i )
{
	return i * i;
}
#endif
template< int N , typename T >
struct dev_array
{
protected:
	T __data[ N ];
public:
	MULTYDEV T &getValue( int i )
	{
		return __data[ i ];
	}
	MULTYDEV T const &getValue( int i ) const
	{
		return __data[ i ];
	}
	MULTYDEV T &x()
	{
		return __data[ 0 ];
	}
	MULTYDEV T const &x() const
	{
		return __data[ 0 ];
	}
	MULTYDEV T &y()
	{
		return __data[ 1 ];
	}
	MULTYDEV T const &y() const
	{
		return __data[ 1 ];
	}
	MULTYDEV T &z()
	{
		return __data[ 2 ];
	}
	MULTYDEV T const &z() const
	{
		return __data[ 2 ];
	}
	MULTYDEV T &w()
	{
		return __data[ 3 ];
	}
	MULTYDEV T const &w() const
	{
		return __data[ 3 ];
	}
	MULTYDEV T const *getdev_array() const
	{
		return __data;
	}
};
template< int N , typename T >
struct dev_accessor : public dev_array < N , T >
{
};
template< int N >
struct dev_unpacker
{
	template< typename T , typename P , typename ...S >
	MULTYDEV static void unpack( T *data , int C , P x , S ...arg )
	{
		data[ C - N ] = static_cast< T >( x );
		dev_unpacker< N - 1 >::unpack( data , C , arg... );
	}
};
template<>
struct dev_unpacker < 1 >
{
	template< typename T , typename P >
	MULTYDEV static void unpack( T *data , int C , P x )
	{//PRINT( x ) PRINT( C - 1 )
		data[ C - 1 ] = static_cast< T >( x );
	}
};
template< int N , typename T >
struct dev_vec : public dev_accessor < N , T >
{
	MULTYDEV T const &getX() const
	{
		return this->getValue( 0 );
	}
	MULTYDEV T &getX()
	{
		return this->getValue( 0 );
	}
	MULTYDEV T const &getY() const
	{
		return this->getValue( 1 );
	}
	MULTYDEV T &getY()
	{
		return this->getValue( 1 );
	}
	/*MULTYDEV void print() const
	{
	ito( N )
	std::cout << this->getValue( i ) << " ";
	std::cout << "\n";
	}*/
	MULTYDEV dev_vec()
	{
		//memset( this->__data , 0 , N * sizeof( T ) );
	}
	MULTYDEV dev_vec( T d )
	{
		ito( N )
			this->__data[ i ] = d;
	}
	MULTYDEV dev_vec( dev_vec< N , T > const &v )
	{
		ito( N )
			this->getValue( i ) = v.getValue( i );
	}
	template< typename P >
	MULTYDEV void operator=( dev_vec< N , P > const &v )
	{
		ito( N )
			this->getValue( i ) = static_cast< T >( v.getValue( i ) );
	}
	MULTYDEV dev_vec( dev_vec< N - 1 , T > const &v , T rest )
	{
		ito( N - 1 )
			this->__data[ i ] = v.getValue( i );
		this->__data[ N - 1 ] = rest;
	}
	template< int H >
	MULTYDEV dev_vec( dev_vec< H , T > const &v1 , dev_vec< N - H , T > const &v2 )
	{
		ito( H )
			this->__data[ i ] = v1.getValue( i );
		ito( N - H )
			this->__data[ i + H ] = v2.getValue( i );
	}
	template< typename ...D >
	MULTYDEV  dev_vec( D ...arg )
	{
		dev_unpacker< N >::unpack( this->__data , N , arg... );
	}
	template< int M >
	MULTYDEV dev_vec< M , T >
		getVec( int a , int b ) const
	{
		dev_vec< M , T > out;
		for( int i = a; i < b; i++ )
			out.getValue( i ) = this->getValue( i );
		return out;
	}
	MULTYDEV void mul( float k )
	{
		for( int i = 0; i < N; i++ )
			this->getValue( i ) *= k;
	}
	MULTYDEV dev_vec< N , T > g_mul( float k ) const
	{
		dev_vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * k;
		return out;
	}
	MULTYDEV float g_dist2( dev_vec< N , T > const &v ) const
	{
		float d = 0.0f;
		for( int i = 0; i < N; i++ )
			d += dev_sqr( this->getValue( i ) - v.getValue( i ) );
		return d;
	}
	MULTYDEV dev_vec< N , T > g_add( dev_vec< N , T > const &v ) const
	{
		dev_vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) + v.getValue( i );
		return out;
	}
	MULTYDEV void add( dev_vec< N , T > const &v )
	{
		for( int i = 0; i < N; i++ )
			this->getValue( i ) = this->getValue( i ) + v.getValue( i );
	}
	MULTYDEV float g_dist( dev_vec< N , T > const &v ) const
	{
		return sqrtf( this->g_dist2( v ) );
	}
	MULTYDEV float g_mod2() const
	{
		float d = 0.0f;
		for( int i = 0; i < N; i++ )
			d += dev_sqr( this->getValue( i ) );
		return d;
	}
	MULTYDEV float g_mod() const
	{
		return sqrtf( g_mod2() );
	}
	MULTYDEV dev_vec< N , T > g_norm() const
	{
		float m = this->g_mod();
		if( m < DELTA )
			return dev_vec< N , T >( static_cast< T >( 0 ) );
		if( fabsf( m - 1.0f ) < DELTA )
			return *this;
		return this->g_mul( 1.0f / m );
	}
	MULTYDEV dev_vec< N , T > operator&( dev_vec< N , T > const &v ) const
	{
		dev_vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * v.getValue( i );
		return out;
	}
	MULTYDEV dev_vec< N , T > operator-( dev_vec< N , T > const &v ) const
	{
		dev_vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) - v.getValue( i );
		return out;
	}
	MULTYDEV dev_vec< N , T > operator+( dev_vec< N , T > const &v ) const
	{
		dev_vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) + v.getValue( i );
		return out;
	}
	MULTYDEV dev_vec< N , T > operator*( float k ) const
	{
		dev_vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * k;
		return out;
	}
	MULTYDEV dev_vec< N , T > operator/( float k ) const
	{
		dev_vec< N , T > out;
		float t = 1.0f / k;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) * t;
		return out;
	}
	MULTYDEV dev_vec< N , T > operator/( dev_vec< N , T > const &k ) const
	{
		dev_vec< N , T > out;
		for( int i = 0; i < N; i++ )
			out.getValue( i ) = this->getValue( i ) / k.getValue( i );
		return out;
	}
	MULTYDEV void operator/=( float k )
	{
		*this = *this / k;
	}
	MULTYDEV float operator*( dev_vec< N , T > const &v ) const
	{
		float out = 0.0f;
		for( int i = 0; i < N; i++ )
			out += this->getValue( i ) * v.getValue( i );
		return out;
	}
	MULTYDEV void operator*=( float k )
	{
		for( int i = 0; i < N; i++ )
			this->getValue( i ) *= k;
	}
	MULTYDEV bool operator==( dev_vec< N , T > const &v ) const
	{
		for( int i = 0; i < N; i++ )
			if( this->getValue( i ) != v.getValue( i ) ) return false;
		return true;
	}
	MULTYDEV bool operator!=( dev_vec< N , T > const &v ) const
	{
		return !( *this == v );
	}
	MULTYDEV float operator^( dev_vec< N , T > const &v ) const
	{
		if( this->g_dist2( v ) < DELTA ) return 0.0f;
		auto v1 = ( *this ).g_norm();
		auto v2 = v.g_norm();
		float scl = acos( dev_wrap( v1 * v2 , -1.0f , 1.0f ) );
		float sgn = lperp( v1 ) * v2;
		if( sgn < 0.0f )
			scl = -scl;
		return scl;
	}
	MULTYDEV void operator+=( dev_vec< N , T > const &v )
	{
		*this = *this + v;
	}
	MULTYDEV void operator-=( dev_vec< N , T > const &v )
	{
		*this = *this - v;
	}
	MULTYDEV dev_vec< N , T > operator-( ) const
	{
		return *this * -1.0f;
	}
	MULTYDEV dev_vec< 2 , T > xy() const
	{
		return dev_vec< 2 , T >( __data[ 0 ] , __data[ 1 ] );
	}
};
template< int N , typename T >
MULTYDEV dev_vec< N , T > operator*( float k , dev_vec< N , T > const &v )
{
	dev_vec< N , T > out;
	for( int i = 0; i < N; i++ )
		out.getValue( i ) = v.getValue( i ) * k;
	return out;
}
template< typename T >
MULTYDEV dev_vec< 2 , T > lperp( dev_vec< 2 , T > const &v )
{
	return dev_vec< 2 , T >( -v.getValue( 1 ) , v.getValue( 0 ) );
}
template< typename T >
MULTYDEV dev_vec< 3 , T > perp_norm( dev_vec< 3 , T > const &v , dev_vec< 3 , T > const &p )
{
	return ( p - v * ( p * v ) ).g_norm();
}
template< typename T >
MULTYDEV dev_vec< 3 , T > vecx( dev_vec< 3 , T > const &a , dev_vec< 3 , T > const &b )
{
	return dev_vec< 3 , T >( a.getValue( 1 ) * b.getValue( 2 ) - b.getValue( 1 ) * a.getValue( 2 ) ,
		b.getValue( 0 ) * a.getValue( 2 ) - a.getValue( 0 ) * b.getValue( 2 ) ,
		a.getValue( 0 ) * b.getValue( 1 ) - b.getValue( 0 ) * a.getValue( 1 ) );
}
template< typename T >
MULTYDEV dev_vec< 2 , T > rotate( dev_vec< 2 , T > const &v , float a )
{
	float c = cosf( a );
	float s = sinf( a );
	return dev_vec< 2 , T >( X( v ) * c - Y( v ) * s , Y( v ) * c + X( v ) * s );
}
template< int N , typename T >
MULTYDEV dev_vec< N , T > dev_wrap( dev_vec< N , T > const &v , float min , float max )
{
	return v.g_norm() * dev_wrap( v.g_mod() , min , max );
}
typedef dev_vec< 2 , float > df2;
typedef dev_vec< 2 , int > di2;
typedef dev_vec< 2 , unsigned int > du2;
typedef dev_vec< 3 , float > df3;
typedef dev_vec< 4 , float > df4;
typedef dev_vec< 3 , int > di3;
#endif // VEC_H
