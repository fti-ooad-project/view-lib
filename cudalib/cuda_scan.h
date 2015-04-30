#pragma once
#include <base\Base.h>
class CudaScanHandler : public Initable
{
private:
	CudaScanHandler() = default;
public:
	static CudaScanHandler *getSingleton();
	void init( uint );
	int scanAdd( int * , int * , uint ) const;
	void release() override;
};