#include <cudalib\cuda_scan.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cudalib\cudaUtil.h>
//int *c_scan_buffer;

CudaScanHandler *CudaScanHandler::getSingleton()
{
	static CudaScanHandler *sngl = new CudaScanHandler();
	return sngl;
}
void CudaScanHandler::init( uint n )
{
	if( isInited() )
		return;
	setInited( true );
	cudaError_t err = cudaSuccess;
	//checkError( cudaMalloc( &c_scan_buffer , n * sizeof( int ) ) , "cuda scan handler malloc buffer" );
error:
	return;
}
int CudaScanHandler::scanAdd( int *obuf , int *ibuf , uint n ) const
{
	dim3 thread_count( min( n , 32 ) , 1 );
	dim3 block_count( ( n + thread_count.x - 1 ) / thread_count.x , 1 );
	c_scan << < block_count , thread_count >> >( obuf , ibuf , n );
	cudaError_t err = cudaSuccess;
	checkError( cudaGetLastError() , "cuda scan handler scanAdd()" );
error:
	return;
}
void CudaScanHandler::release()
{
	if( !isInited() )
		return;
	setInited( false );
	cudaError_t err = cudaSuccess;
	//checkError( cudaFree( c_scan_buffer ) , "cuda scan handler free buffer" );
error:
	return;
}