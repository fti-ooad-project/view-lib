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
	/**/
	void calcKernelVector( float const * in_matrix , float in_kernel_value , int matrix_size , float * out_kernel_vector );
	/*return matrix - k * I determinant*/
	float calcMatrixDet2M( float const *in_matrix , int matrix_size , float kernel_value );
	/*free memory on host and device*/
	void release() override;
};