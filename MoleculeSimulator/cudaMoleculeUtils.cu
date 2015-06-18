#include <MoleculeSimulator\cudaMoleculeUtils.h>
#include <cudalinalg\vec.h>
#include "device_launch_parameters.h"
#include <iostream>
struct SortCube
{
	static const int N = 32;
	static const int DIV = 6;
	static const int SIZE = 60;
	int _array[ DIV ][ DIV ][ DIV ][ N + 1 ];
};
__device__ df3 calcForce( df3 const &p1 , df3 const &p2 )
{
	df3 dr = p2 - p1;
	const float l0 = 2.0f;
	const float u0 = 15.0f;
	float l = dr.g_mod();
	float ql;
	ql = powf( l0 / l , 2.0f );
	return dr * u0 * ( 6.0f * powf( ql , 4.0f ) - 12.0f * powf( ql , 7.0f ) );
}
__device__ __host__ void getIndex( int *outi3 , float const *inf3 )
{
	ito( 3 )
	{
		outi3[ i ] = ( inf3[ i ] + SortCube::SIZE * 0.5f ) / SortCube::SIZE * SortCube::DIV;
		outi3[ i ] = outi3[ i ] > 0 ? outi3[ i ] : 0;
		outi3[ i ] = outi3[ i ] < SortCube::DIV ? outi3[ i ] : SortCube::DIV - 1;
	}
}
__global__ void dev_calcLennardForceMatrix( void const *pos , void *force_mat , int count )
{
	int tidx = blockDim.x * blockIdx.x + threadIdx.x;
	int tidy = blockDim.y * blockIdx.y + threadIdx.y;
	/*const int SHN = 64;
	__shared__ df3 shared_pos[ SHN ];
	if( threadIdx.x == 0 )
	{
		shared_pos[ threadIdx.y ] = ( ( df3* )pos )[ tidx ];
	}
	if( threadIdx.y == 0 )
	{
		shared_pos[ 32 + threadIdx.x ] = ( ( df3* )pos )[ tidy ];
	}
	__syncthreads();*/
	if( tidx > tidy && tidy < count && tidx < count )
	{
		df3 curpos = ( ( df3* )pos )[ tidy ];// shared_pos[ 32 + threadIdx.x ];
		df3 iterpos = ( ( df3* )pos )[ tidx ];// shared_pos[ threadIdx.y ];
		df3 dr = iterpos - curpos;
		df3 force( 0.0f );
		if( fabsf( dr.x() ) < 20.0f && fabsf( dr.y() ) < 20.0f )
		{
			const float l0 = 4.0f;
			const float u0 = 15.0f;
			float l = dr.g_mod();
			float ql;
			if( l > l0 / 2.0f )
			{
				ql = powf( l0 / l , 2.0f );
			}
			else
			{
				ql = powf( 2.0f , 2.0f );
			}
			force = dr * u0 * ( 6.0f * powf( ql , 4.0f ) - 12.0f * powf( ql , 7.0f ) );
		}
		( ( df3* )force_mat )[ tidy * count + tidx ] = force;
		( ( df3* )force_mat )[ tidx * count + tidy ] = -force;
	}
}
__global__ void dev_calcLennardForceVector( void const *force_mat , void *force_vec , int count )
{
	int tid = blockDim.x * blockIdx.x + threadIdx.x;
	if( tid < count )
	{
		df3 temp_force( 0.0f );
		for( int j = 0; j < count; ++j )
		{
			if( j != tid )
			{
				temp_force += ( ( df3* )force_mat )[ j + count * tid ];
			}
		}
		( ( df3* )force_vec )[ tid ] = temp_force;
	}
}
__global__ void dev_calcLennardForceSorted( void *force_mat , void const *pos , SortCube const *sort_cube , int count )
{
	int bid = blockIdx.x;
	int tid = threadIdx.x;
	if( bid < SortCube::DIV * SortCube::DIV * SortCube::DIV )
	{
		int indx[ 4 ];
		int rc = bid % ( SortCube::DIV * SortCube::DIV );
		indx[ 0 ] = rc % SortCube::DIV;
		indx[ 1 ] = rc / SortCube::DIV;
		indx[ 2 ] = bid / ( SortCube::DIV * SortCube::DIV );
		int const *main_arr = sort_cube->_array[ indx[ 0 ] ][ indx[ 1 ] ][ indx[ 2 ] ];
		indx[ 3 ] = main_arr[ 0 ];
		if( tid >= indx[ 3 ] )
		{
			return;
		}
		int main_index = main_arr[ tid + 1 ];
		df3 curpos = ( ( df3* )pos )[ main_index ];
		df3 final_force( 0.0f );
		for( int dx = -1; dx < 2; dx++ )
		{
			for( int dy = -1; dy < 2; dy++ )
			{
				for( int dz = -1; dz < 2; dz++ )
				{
					if( !(
						indx[ 0 ] + dx < 0 || indx[ 0 ] + dx > SortCube::DIV
						|| indx[ 1 ] + dy < 0 || indx[ 1 ] + dy > SortCube::DIV
						|| indx[ 2 ] + dz < 0 || indx[ 2 ] + dz > SortCube::DIV
						)
						)
					{
						int const *arr = sort_cube->_array[ indx[ 0 ] + dx ][ indx[ 1 ] + dy ][ indx[ 2 ] + dz ];
						int n = arr[ 0 ];
						for( int i = 0; i < n; ++i )
						{
							int indx = arr[ i + 1 ];
							if( indx != main_index )
							{
								df3 local_pos = ( ( df3* )pos )[ indx ];
								final_force += calcForce( curpos , local_pos );
								//( ( df3* )force_mat )[ indx * count + main_index ] = -force;
								//( ( df3* )force_mat )[ main_index * count + indx ] = force;
							}
						}
					}
				}
			}
			( ( df3* )force_mat )[ main_index ] = final_force;
		}
		/*__shared__ int indx[ 4 ];
		__shared__ int const *main_arr;
		__shared__ df3 neib[ 32 ][ 32 ];
		__shared__ int neibindx[ 32 ][ 32 ];
		__shared__ int neibn[ 32 ];
		__shared__ int tempdr[ 32 ][ 3 ];
		df3 curpos;
		int main_index;
		if( tid == 0 )
		{
			int rc = bid % ( SortCube::DIV * SortCube::DIV );
			indx[ 0 ] = rc % SortCube::DIV;
			indx[ 1 ] = rc / SortCube::DIV;
			indx[ 2 ] = bid / ( SortCube::DIV * SortCube::DIV );
			main_arr = sort_cube->_array[ indx[ 0 ] ][ indx[ 1 ] ][ indx[ 2 ] ];
			indx[ 3 ] = main_arr[ 0 ];
			int t = 0;
			for( int dx = -1; dx < 2; dx++ )
			{
				for( int dy = -1; dy < 2; dy++ )
				{
					for( int dz = -1; dz < 2; dz++ )
					{
						tempdr[ t ][ 0 ] = dx;
						tempdr[ t ][ 1 ] = dy;
						tempdr[ t ][ 2 ] = dz;
						t++;
					}
				}
			}
		}
		//__syncthreads();
		neibn[ tid ] = 0;
		if( tid < 27 &&
			!(
			indx[ 0 ] + tempdr[ tid ][ 0 ] < 0 || indx[ 0 ] + tempdr[ tid ][ 0 ] > SortCube::DIV
			|| indx[ 1 ] + tempdr[ tid ][ 1 ] < 0 || indx[ 1 ] + tempdr[ tid ][ 1 ] > SortCube::DIV
			|| indx[ 2 ] + tempdr[ tid ][ 2 ] < 0 || indx[ 2 ] + tempdr[ tid ][ 2 ] > SortCube::DIV
			)
			)
		{
			int const *arr = sort_cube->_array[ indx[ 0 ] + tempdr[ tid ][ 0 ] ][ indx[ 1 ] + tempdr[ tid ][ 1 ] ][ indx[ 2 ] + tempdr[ tid ][ 2 ] ];
			int n = arr[ 0 ];
			neibn[ tid ] = n;
			for( int i = 0; i < n; ++i )
			{
				int indx = arr[ i + 1 ];
				neib[ tid ][ i ] = ( ( df3* )pos )[ indx ];
				neibindx[ tid ][ i ] = indx;
			}
		}
		__syncthreads();
		if( tid >= indx[ 3 ] )
		{
			return;
		}
		main_index = main_arr[ tid + 1 ];
		curpos = ( ( df3* )pos )[ main_index ];
		df3 final_force( 0.0f );

		for( int k = 0; k < 27; k++ )
		{
			int n = neibn[ k ];
			for( int i = 0; i < n; ++i )
			{
				if( neibindx[ k ][ i ] != main_index )
				{
					final_force += calcForce( curpos , neib[ k ][ i ] );
				}
				//df3 iterpos = ( ( df3* )pos )[ local_index ];
				//df3 force = calcForce( curpos , iterpos );
				//( ( df3* )force_mat )[ local_index * count + main_index ] = -force;
				//( ( df3* )force_mat )[ main_index * count + local_index ] = force;
			}
		}*/
		//( ( df3* )force_mat )[ main_index ] = final_force;
	}
}
int N = 0;
void *dev_buffer;
SortCube local_sort_cube;
void LennardForceInit( int n )
{
	::N = n;
	cudaMalloc( &dev_buffer , sizeof( SortCube ) + ( n + n * n ) * sizeof( df3 ) );
}
void LennardForceRelease()
{
	cudaFree( dev_buffer );
}
void calcLennardForceVector( void const *pos , void *out_force , int count )
{
	if( count > N )
	{
		LennardForceRelease();
		LennardForceInit( count );
	}
	cudaEvent_t start , stop;
	cudaEventCreate( &start );
	cudaEventCreate( &stop );
	cudaEventRecord( start );
	{
		memset( &local_sort_cube , 0 , sizeof( SortCube ) );
		for( int i = 0; i < count; ++i )
		{
			int indx[ 3 ];
			getIndex( indx , ( float const * )pos + i * 3 );
			if( local_sort_cube._array[ indx[ 0 ] ][ indx[ 1 ] ][ indx[ 2 ] ][ 0 ] < SortCube::N )
			{
				local_sort_cube._array[ indx[ 0 ] ][ indx[ 1 ] ][ indx[ 2 ] ][ local_sort_cube._array[ indx[ 0 ] ][ indx[ 1 ] ][ indx[ 2 ] ][ 0 ] + 1 ] = i;
				local_sort_cube._array[ indx[ 0 ] ][ indx[ 1 ] ][ indx[ 2 ] ][ 0 ]++;
			} else
			{
				std::cout << "SORTCUBE SPACE IS OVER!!\n";
			}
		}
		df3 *dev_pos = ( df3* )dev_buffer;
		df3 *dev_force_mat = ( df3* )dev_buffer + count;
		SortCube *dev_sort_cube = ( SortCube* )( ( df3* )dev_buffer + count + count * count );
		cudaMemcpy( dev_pos , pos , count * sizeof( df3 ) , cudaMemcpyHostToDevice );
		cudaMemset( dev_force_mat , 0 , count * count * sizeof( df3 ) );
		cudaMemcpy( dev_sort_cube , &local_sort_cube , sizeof( SortCube ) , cudaMemcpyHostToDevice );
		int block_count = SortCube::DIV * SortCube::DIV * SortCube::DIV;
		dev_calcLennardForceSorted << < block_count , 32 >> >( dev_force_mat , dev_pos , dev_sort_cube , count );
		cudaThreadSynchronize();
		//cudaMemcpy( out_force , ( df3* )dev_buffer + count , count * sizeof( df3 ) , cudaMemcpyDeviceToHost );
		//dim3 block_size = dim3( count / 32 + 1 , count / 32 + 1 );
		//dev_calcLennardForceVector << < block_size.x , 32 >> >( dev_force_mat , dev_pos , count );
		//cudaThreadSynchronize();
		cudaMemcpy( out_force , dev_force_mat , count * sizeof( df3 ) , cudaMemcpyDeviceToHost );
		/*float *force = ( float* )out_force;
		ito( 100 )
		{
			int k = i + 100;
			std::cout << force[ k * 3 ] << "," << force[ k * 3 + 1 ] << "," << force[ k * 3 + 2 ] << "\n";
		}*/

		/*cudaMemcpy( dev_buffer , pos , count * sizeof( df3 ) , cudaMemcpyHostToDevice );
		dim3 block_size = dim3( count / 32 + 1 , count / 32 + 1 );
		dev_calcLennardForceMatrix << < block_size , dim3( 32 , 32 ) >> >( dev_buffer , ( df3* )dev_buffer + count , count );
		cudaThreadSynchronize();
		dev_calcLennardForceVector << < block_size.x , 32 >> >( ( df3* )dev_buffer + count , dev_buffer , count );
		cudaThreadSynchronize();
		cudaMemcpy( out_force , dev_buffer , count * sizeof( df3 ) , cudaMemcpyDeviceToHost );*/
	}
	cudaEventRecord( stop );
	cudaEventSynchronize( stop );
	float milliseconds = 0;
	cudaEventElapsedTime( &milliseconds , start , stop );
	std::cout << milliseconds << "\n";
}