#include <cudalinalg\cudaMatrixSolver.h>
#include "device_launch_parameters.h"
#include <cudalinalg\vec.h>
#define getValue( i , j ) mat[ i * mat_size + j ]
__global__ void dev_triangulate( float *mat , int mat_size , int n )
{
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	int i = blockDim.y * blockIdx.y + threadIdx.y;
	if( i < mat_size && j < mat_size && i > n )
	{
		float k = getValue( n , n );
		if( fabsf( k ) > 0.0f )
		{
			getValue( i , j ) -= getValue( n , j ) * getValue( i , n ) / k;
		} else
		{
			//getValue( i , j ) = 0.0f;
		}
	}
}
__global__ void dev_calcNullRow( float *mat , int mat_size , int *outv )
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;
	if( i < mat_size )
	{
		float sum = 0.0f;
		for( int k = 0; k < mat_size; k++ )
		{
			float v = getValue( i , k );
			sum += v * v;
		}
		if( sum < 0.000001f )
			*outv = i;
	}
}
__global__ void dev_shiftY( float *out_mat , float *mat , int mat_size , int zerorow )
{
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	int i = blockDim.y * blockIdx.y + threadIdx.y;
	if( i < mat_size && j < mat_size )
	{
		int shift = 0;
		if( i >= zerorow )
			shift = 1;
		if( i == mat_size - 1 )
		{
			out_mat[ i * mat_size + j ] = 0.0f;
		} else
		{
			out_mat[ i * mat_size + j ] = mat[ ( i + shift ) * mat_size + j ];
		}
	}
}
CudaMatrixSolver *CudaMatrixSolver::getSingleton()
{
	static cudaMatrixSolver *sngl = new cudaMatrixSolver();
	return sngl;
}
void CudaMatrixSolver::makeSpace( int size )
{
	free( _host_buffer );
	cudaFree( _dev_buffer );
	_buffer_size = size;
	_host_buffer = malloc( _buffer_size );
	cudaMalloc( &_dev_buffer , _buffer_size );
}
/*take some memory on host and device*/
void CudaMatrixSolver::init()
{
	if( isInited() ) return;
	setInited( true );
	_buffer_size = 0x1000;
	_host_buffer = malloc( _buffer_size );
	cudaMalloc( &_dev_buffer , _buffer_size );
}
/*return true if kernel value is match and false if not*/
bool CudaMatrixSolver::calcKernelVector( float const * in_matrix , float in_kernel_value , int matrix_size , float * out_kernel_vector )
{
	int byte_size = matrix_size * matrix_size * sizeof( float );
	if( 2 * byte_size > _buffer_size )
	{
		makeSpace( byte_size * 2 );
	}
	float *dev_mat = ( float* )_dev_buffer;
	cudaMemcpy( dev_mat , in_matrix , byte_size , cudaMemcpyHostToDevice );
	dim3 block_size = dim3( matrix_size / 32 + 1 , matrix_size / 32 + 1 );
	for( int n = 0; n < matrix_size; n++ )
	{
		dev_triangulate<<< block_size , dim3( 32 , 32 ) >>>( dev_mat , matrix_size , n );
		cudaThreadSynchronize();
	}
	int zerorow = -1;
	{
		int *dev_zerorow;
		cudaMalloc( &dev_zerorow , sizeof( int ) );
		cudaMemcpy( dev_zerorow , &zerorow , sizeof( int ) , cudaMemcpyHostToDevice );
		dev_calcNullRow << < 1 , 32 >> >( dev_mat , matrix_size , dev_zerorow );
		cudaThreadSynchronize();
		cudaMemcpy( &zerorow , dev_zerorow , sizeof( int ) , cudaMemcpyDeviceToHost );
		cudaFree( dev_zerorow );
	}
	if( zerorow < 0 )
	{
		return false;
	}
	float *dev_temp_mat = ( float* )_dev_buffer + matrix_size * matrix_size;
	dev_shiftY << < block_size , dim3( 32 , 32 ) >> >( dev_temp_mat , dev_mat , matrix_size , zerorow );
	cudaMemcpy( _host_buffer , dev_temp_mat , byte_size , cudaMemcpyDeviceToHost );
	float *local_mat = ( float* )_host_buffer;
	out_kernel_vector[ matrix_size - 1 ] = 1.0f;
	for( int i = matrix_size - 2; i >= 0; i-- )
	{
		out_kernel_vector[ i ] = 0.0f;
		for( int j = i + 1; j < matrix_size; j++ )
		{
			out_kernel_vector[ i ] -= out_kernel_vector[ j ] * local_mat[ i * matrix_size + j ];
		}
		out_kernel_vector[ i ] /= local_mat[ i * matrix_size + i ];
	}
	return true;
}
/*free memory on host and device*/
void CudaMatrixSolver::release()
{
	if( !isInited() ) return;
	setInited( false );
}