#include <cudalinalg\cudaMatrixSolver.h>
#include "device_launch_parameters.h"
#include <cudalinalg\vec.h>
#include <iomanip>
#define getValue( i , j ) mat[ i * mat_size + j ]
__global__ void dev_triangulate( float *mat , int mat_size , int n )
{
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	int i = blockDim.y * blockIdx.y + threadIdx.y;
	if( i < mat_size && j < mat_size && i > n )
	{
		float k = getValue( n , n );
		if( fabsf( k ) > 0.0001f )
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
		if( sum < 0.01f )
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
__global__ void dev_subKernelValue( float *mat , int mat_size , float value )
{
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	if( j < mat_size )
	{
		mat[ j * mat_size + j ] -= value;
	}
}
CudaMatrixSolver *CudaMatrixSolver::getSingleton()
{
	static CudaMatrixSolver *sngl = new CudaMatrixSolver();
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
#define printMatrix( mat , N )\
{\
	cudaMemcpy( _host_buffer , mat , N * N * sizeof( float ) , cudaMemcpyDeviceToHost );\
	float *local_mat = ( float* )_host_buffer;\
	for( int ti = 0; ti < N; ti ++ )\
		{\
		for( int tj = 0; tj < N; tj++ )\
				{\
					std::cout << std::setprecision( 1 ) << std::setw( 4 ) << local_mat[ ti * N + tj ] << " ";\
				}\
		std::cout << "\n";\
	}\
}
/*return matrix determinant*/
void CudaMatrixSolver::calcKernelVector( float const * in_matrix , float kernel_value , int matrix_size , float * out_kernel_vector )
{
	int byte_size = matrix_size * matrix_size * sizeof( float );
	if( 2 * byte_size > _buffer_size )
	{
		makeSpace( byte_size * 2 );
	}
	float *dev_mat = ( float* )_dev_buffer;
	cudaMemcpy( dev_mat , in_matrix , byte_size , cudaMemcpyHostToDevice );
	dim3 block_size = dim3( matrix_size / 32 + 1 , matrix_size / 32 + 1 );
	dev_subKernelValue<<< block_size.x , 32 >>>( dev_mat , matrix_size , kernel_value );
	cudaThreadSynchronize();
	float *local_mat = ( float* )_host_buffer;
	/*cudaMemcpy( local_mat , dev_mat , byte_size , cudaMemcpyDeviceToHost );
	float det = calcMatrixDet2M( local_mat , matrix_size , 0.0f );
	if( fabsf( det ) > 0.0001f )
	{
		std::cout << "calcKernelVector: matrix determinant is not 0\n";
		return;
	}*/
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
		//std::cout << "calcKernelVector: triangulation error\n";
		return;
	}
	//printMatrix( dev_mat , matrix_size );
	std::cout << kernel_value << "\n";
	float *dev_temp_mat = ( float* )_dev_buffer + matrix_size * matrix_size;
	dev_shiftY << < block_size , dim3( 32 , 32 ) >> >( dev_temp_mat , dev_mat , matrix_size , zerorow );
	cudaMemcpy( _host_buffer , dev_temp_mat , byte_size , cudaMemcpyDeviceToHost );
	out_kernel_vector[ matrix_size - 1 ] = 1.0f;
	for( int i = matrix_size - 2; i >= 0; i-- )
	{
		out_kernel_vector[ i ] = 0.0f;
		if( fabsf( local_mat[ i * matrix_size + i ] ) > 0.001f )
		{
			for( int j = i + 1; j < matrix_size; j++ )
			{
				out_kernel_vector[ i ] -= out_kernel_vector[ j ] * local_mat[ i * matrix_size + j ];
			}
			out_kernel_vector[ i ] /= local_mat[ i * matrix_size + i ];
		} else
		{
			out_kernel_vector[ i ] = 1.0f;
		}
	}
	return;
}
__global__ void dev_calcCellDeterm2M( float const *in_matrix , float *out_matrix , int cell_count )
{
	int j = blockDim.x * blockIdx.x + threadIdx.x;
	int i = blockDim.y * blockIdx.y + threadIdx.y;
	if( i < cell_count && j < cell_count )
	{
		int indx = i * cell_count * 4 + j * 2;
		out_matrix[ i * cell_count + j ] = in_matrix[ indx ] * in_matrix[ indx + cell_count * 2 + 1 ] - in_matrix[ indx + cell_count * 2 ] * in_matrix[ indx + 1 ];
	}
}
float CudaMatrixSolver::calcMatrixDet2M( float const *in_matrix , int matrix_size , float kernel_value )
{
	for( int i = 1 << 30; i > 1; i >>= 1 )
	{
		if( i & matrix_size )
		{
			if( i != matrix_size )
			{
				std::cout << "calcMatrixDet2M: matrix size is not power of two\n";
				return 0.0f;
			} else
			{
				break;
			}
		}
	}
	int byte_size = matrix_size * matrix_size * sizeof( float );
	if( 2 * byte_size > _buffer_size )
	{
		makeSpace( byte_size * 2 );
	}
	float *dev_mat[ 2 ] = { ( float* )_dev_buffer , ( float* )_dev_buffer + matrix_size * matrix_size };
	cudaMemcpy( dev_mat[ 0 ] , in_matrix , byte_size , cudaMemcpyHostToDevice );
	if( kernel_value != 0.0f )
	{
		dim3 block_size = dim3( matrix_size / 32 + 1 , matrix_size / 32 + 1 );
		dev_subKernelValue << < block_size.x , 32 >> >( dev_mat[ 0 ] , matrix_size , kernel_value );
		cudaThreadSynchronize();
	}
	int cur = 1 , last = 0;
	//printMatrix( dev_mat[ 0 ] , 4 );
	for( int i = matrix_size / 2; i > 0; i >>= 1 )
	{
		dim3 block_size = dim3( i / 32 + 1 , i / 32 + 1 );
		dev_calcCellDeterm2M <<< block_size , dim3( 32 , 32 ) >> > ( dev_mat[ last ] , dev_mat[ cur ] , i );
		cudaThreadSynchronize();
		//printMatrix( dev_mat[ cur ] , i );
		last = cur;
		cur = 1 - cur;
	}
	float det;
	cudaMemcpy( &det , dev_mat[ last ] , sizeof( float ) , cudaMemcpyDeviceToHost );
	return det;
}
/*free memory on host and device*/
void CudaMatrixSolver::release()
{
	if( !isInited() ) return;
	setInited( false );
	free( _host_buffer );
	cudaFree( _dev_buffer );
}