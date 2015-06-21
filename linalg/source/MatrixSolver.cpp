#include <linalg\MatrixSolver.h>
#include <iostream>
#include <thread>
typedef double FType;
#define allocFTypeMat( name , size ) \
		double *name = ( FType* )malloc( size );\
		{\
			memset( name , 0 , size );\
		}
#define getElem( mat , i , j ) mat[ i * N + j ]
#define getElemU( mat , i , j ) mat[ ( N - j ) * N - 1 - i ]
#define getInElem( i , j ) inmat[ P[ i ] * N + j ]
/*returns determinant of input matrix:*/
FType *mat;
int N;
size_t mat_size;
FType LUDecomposite( FType const *inmat , const int N , FType *outQ )
{
	FType det = 1.0f;
	//FType *transposed = outQ + ::N * ::N;
	int *P = ( int* )( outQ + ::N * ::N );
	//memset( outQ , 0 , N * N * sizeof( double ) );
	memset( P , 0 , N * sizeof( int ) );
	//for( int j = 0; j < N; j++ )
	//	P[ j ] = j;
	for( int j = 0; j < N; j++ )
	{
		if( !P[ j ] && j )
			P[ j ] = j;
		FType qjj = getInElem( j , j );
		for( int n = 0; n < j; n++ )
		{
			qjj -= getElem( outQ , j , n ) * getElemU( outQ , n , j );
		}
		getElem( outQ , j , j ) = qjj;
		det *= qjj;
		if( fabs( qjj ) < 1e-100 )
		{
			std::cout << "zero element q" << j << "'" << j << "\n";
			int t = P[ j ];
			P[ j ] = P[ j + 1 ];
			P[ j + 1 ] = t;
			j--;
			continue;
		}
		for( int i = j + 1; i < N; i++ )
		{
			FType lij = getInElem( i , j );
			FType uji = getInElem( j , i );
			for( int n = 0; n < j; n++ )
			{
				lij -= getElem( outQ , i , n ) * getElemU( outQ , n , j );
				uji -= getElemU( outQ , n , i ) * getElem( outQ , j , n );
			}
			getElem( outQ , i , j ) = lij;
			getElemU( outQ , j , i ) = uji / qjj;
		}
		//testNative();
	}
	for( int i = 0; i < N; i++ )
	{
		for( int j = i + 1; j < N; j++ )
		{
			double temp = getElem( outQ , i , j );
			getElem( outQ , i , j ) = getElemU( outQ , i , j );
			getElemU( outQ , i , j ) = temp;
		}
	}
	return det;
}
double calcDet( FType const *inmat , int N )
{
	if( N > ::N )
	{
		release();
		init( N );
	}
	FType det = LUDecomposite( inmat , N , ::mat );
	//testNative();
	return det;
}
void init( int N )
{
	::N = N;
	::mat_size = N * N * sizeof( FType );
	::mat = ( double* )malloc( ( N * N + N ) * sizeof( FType ) );
	//memset( ::mat , 0 , ::mat_size * 2 );
}
void release()
{
	free( ::mat );
}
void testNative()
{
	for( int i = 0; i < N; i++ )
	{
		for( int j = 0; j < N; j++ )
		{
			std::cout << ::mat[ i * N + j ] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}
/*for( int j = 0; j < N; j++ )
{
FType qjj;
for( int n = 0; n < j; n++ )
{
buffer[ n ] = getElem( outQ , n , j );
buffer1[ n ] = getElem( outQ , j , n );
}
for( int i = j; i < N; i++ )
{
FType lij = getElem( inmat , i , j );
for( int n = 0; n < j; n++ )
{
lij -= getElem( outQ , i , n ) * buffer[ n ];
}
getElem( outQ , i , j ) = lij;
if( i == j )
{
qjj = lij;
}
}
if( fabs( qjj ) < 0.000001f )
{
std::cout << "zero element!!!\n";
}
for( int i = j + 1; i < N; i++ )
{
FType uji = getElem( inmat , j , i );
for( int n = 0; n < j; n++ )
{
uji -= buffer1[ n ] * getElem( outQ , n , i );
}
getElem( outQ , j , i ) = uji / qjj;
}
det *= qjj;
}*/
/*for( int n = 0; n < N; n++ )
{
for( int i = n; i < N; i++ )
{
FType ann = U[ n * N + n ];
FType k = U[ i * N + n ];
for( int j = 0; j < N; j++ )
{
if( i == n )
{
if( j > n - 1 )
U[ n * N + j ] /= ann;
if( j < n + 1 )
L[ n * N + j ] /= ann;
} else
if( j < n )
{
U[ i * N + j ] = 0.0f;
} else
{
U[ i * N + j ] -= U[ n * N + j ] * k;
L[ i * N + j ] -= L[ n * N + j ] * k;
}
}
}
det *= L[ n * N + n ];
}*/
/*for( int n = N - 1; n >= 0; n-- )
{
for( int i = n - 1; i >= 0; i-- )
{
FType ann = L[ n * N + n ];
FType k = L[ i * N + n ] / ann;
for( int j = 0; j < N; j++ )
{
U[ i * N + j ] -= U[ n * N + j ] * k;
if( j > n )
{
L[ i * N + j ] = 0.0f;
} else
{
L[ i * N + j ] -= L[ n * N + j ] * k;
}
}
}
det *= L[ n * N + n ];
}*/