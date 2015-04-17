#ifndef __RTEXTURES_H__
#define __RTEXTURES_H__
#include <base/Base.h>
struct Image : public Initable
{
	Size _size;
	uint _bytes_per_pixel = 0;
	std::unique_ptr< char[] > __data;
	Image( const Image & ) = delete;
	Image& operator=( const Image & ) = delete;
	Image( std::unique_ptr< char[] > && , Size , uint );
	void getVal( uint * , f2 const & ) const;
	Image() = default;
	Image( Image && );
	void operator=( Image && );
	void release();
	~Image();
};
#endif /* __RTEXTURES_H__ */
