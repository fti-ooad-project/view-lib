#pragma once
#define DEVICE __device__
#define GLOBAL __global__
#define LOCAL __host__
#define checkError( x , s ) { err = x; if( err != cudaSuccess ){ std::cout << "cuda error at " << s << " , " << cudaGetErrorString( err ) << "\n"; goto error; } }