#ifndef RANIMGL_H
#define RANIMGL_H
#include <view\Anim.h>
#include <view\ViewModels.h>
#include <base\Base.h>
class RBoneAnimInTexHolderGL : public Initable
{
public:
	std::unique_ptr< RAnimationset[] > __sets;
	int _count = 0;
	int _bone_count;
	//std::unique_ptr< uint[] > __texture_pointer_array;
	uint _texture_array_pointer;
public:
	RBoneAnimInTexHolderGL() = default;
	uint getCount() const;
	void operator=( RBoneAnimInTexHolderGL && );
	RBoneAnimInTexHolderGL( RBoneAnimInTexHolderGL && );
	RBoneAnimInTexHolderGL( std::unique_ptr< RAnimationset[] > && , int );
	void init( std::unique_ptr< RAnimationset[] > && , int );
	void init();
	void release();
	uint getBufferPtr() const;
};
#endif // RANIMGL_H
