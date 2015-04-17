#ifndef __RPASSDESC_H__
#define __RPASSDESC_H__
#include <base/Base.h>
class BufferStoreType
{
public:
	static const int BUFFER_BYTE = 0 , BUFFER_FLOAT = 1 , BUFFER_INT = 2;
};
struct Rect
{
	uint _x , _y , _w , _h;
};
struct PassDesc
{
	Size _size;
	int _store_type;
	uint _buffer_count;
	int _foreight_depth_buffer;
	bool _depth_buffer_access;
	bool _cubemap;
	int _component_number;
};
#endif /* __RPASSDESC_H__ */
