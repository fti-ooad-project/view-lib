#ifndef MAT_H
#define MAT_H
#include "vec.h"
#include <iostream>
template< typename T , int N , int M >
struct Matrix
{
	union
	{
		T _data[N*M];
		T __data[N][M];
	};
public:
	API T& getValue( int i , int j )
	{
		return this->__data[i][j];
	}
	API T const &getValue( int i , int j ) const
	{
		return this->__data[i][j];
	}
	API T const *getPtr() const
	{
		return _data;
	}
	API Matrix()
	{
	}
	API Matrix( T d )
	{
		memset( _data , 0 , sizeof( T ) * N * M );
		ito( N )
			__data[i][i] = d;
	}
	API template< typename ...D >
	Matrix( D ...arg )
	{
		unpacker< N * M >::unpack( this->_data , N * M , arg... );
	}
	API T& operator()( int i , int j )
	{
		return getValue( i , j );
	}
	API T const &operator()( int i , int j ) const
	{
		return __data[i][j];
	}
	API Matrix( Matrix< T , N , M > const &v )
	{
		/*ito( N )
			xfor( j , M )
				v.getValue( i , j ) = v.getValue( i , j );*/
		memcpy( this->_data , v.getPtr() , sizeof( T ) * N * M );
	}
	API vec< M , T > &row( int i )
	{
		return *reinterpret_cast< vec< M , T >* >( __data[i] );
	}
	API vec< M , T > const & row( int i ) const
	{
		return *reinterpret_cast< vec< M , T > const * >( __data[i] );
	}
	API vec< N , T > collumn( int j ) const
	{
		vec< N , T > out;
		ito( N )
			out.getValue( i ) = this->__data[i][j];
		return out;
	}
	API void scale( vec< N - 1 , T > const &scale )
	{
		for( int i = 0; i < M - 1; i ++ )
			for( int j = 0; j < N - 1; j ++ )
				__data[j][i] *= scale.getValue( j );
	}
	API void print() const
	{
		for( int i = 0; i < N; i ++ )
		{
			for( int j = 0; j < M ; j ++ )
				std::cout << this->__data[i][j] << " | ";
			std::cout << "\n";
		}
		std::cout << "___________\n";
	}
};
typedef Matrix< float , 4 , 4 > f4x4;
typedef Matrix< float , 3 , 3 > f3x3;
API f3x3 rotation( const f3 &nax , float ang )
{
	float sa = sin( ang ) , ca = cos( ang );
	float oa = 1.0 - ca;
	return f3x3(
		ca + oa * nax.x() * nax.x() ,
		oa * nax.y() * nax.x() + sa * nax.z() ,
		oa * nax.z() * nax.x() - sa * nax.y() ,

		oa * nax.x() * nax.y() - sa * nax.z() ,
		ca + oa * nax.y() * nax.y() ,
		oa * nax.z() * nax.y() + sa * nax.x() ,

		oa * nax.x() * nax.z() + sa * nax.y() ,
		oa * nax.y() * nax.z() - sa * nax.x() ,
		ca + oa * nax.z() * nax.z()
		);
}
API template< typename T , int N , int M , int R >
Matrix< T , N , M > operator*( const Matrix< T , N , R > &a , const Matrix< T , R , M > &b )
{
	Matrix< T , N , M > out;
	for( int i = 0; i < N; i ++ )
	{
		for( int j = 0; j < M; j ++ )
		{
			out( i , j ) = a.row( i ) * b.collumn( j );
		}
	}
	return out;
}
API template< typename T , int N , int M >
Matrix< T , N , M > operator+( const Matrix< T , N , M > &a , const Matrix< T , N , M > &b )
{
	Matrix< T , N , M > out;
	ito( N )
			xfor( j , M )
				out( i , j ) = a( i , j ) + b( i , j );
	return out;
}
API template< typename T , int N , int M >
Matrix< T , N , M > operator*( const Matrix< T , N , M > &a , float b )
{
	Matrix< T , N , M > out;
	for( int i = 0; i < N; i ++ )
		for( int j = 0; j < M; j ++ )
			out( i , j ) = a( i , j ) * b;
	return out;
}
#endif
