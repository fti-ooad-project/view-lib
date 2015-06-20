#ifndef RANIM_H
#define RANIM_H
#include <base/Base.h>
#include <memory>
class Animationset
{
public:
	uint        _frame_count;
	uint        _bone_count;
	std::unique_ptr< f4x4[] > __data;
	Animationset() = default;
	Animationset( std::unique_ptr< f4x4[] > &&data , uint frame_count , uint bone_count ):
	_frame_count( frame_count )
	, _bone_count( bone_count )
	, __data( std::move( data ) )
	{
	}
	Animationset( Animationset &&a ):
	_frame_count( a._frame_count )
	, _bone_count( a._bone_count )
	, __data( std::move( a.__data ) )
	{
	}
	void operator=( Animationset &&a )
	{
		_frame_count = a._frame_count;
		_bone_count = a._bone_count;
		__data = std::move( a.__data );
	}
};
#endif // RANIM_H
