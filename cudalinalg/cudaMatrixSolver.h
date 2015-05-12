#pragma once
#include <base\Base.h>
class CudaMatrixSolver : public Initable
{
private:
	int _buffer_size;
	void *_host_buffer , *_dev_buffer;
	CudaMatrixSolver() = default;
	void makeSpace( int size );
public:
	static CudaMatrixSolver *getSingleton();
	/*take some memory on host and device*/
	void init();
	/*return true if kernel value is match and false if not*/
	bool calcKernelVector( float const * $in_matrix , float $in_kernel_value , int $matrix_size , float * $out_kernel_vector );
	/*free memory on host and device*/
	void release() override;
};