#ifndef RPASSGL_H
#define RPASSGL_H
#include <view\PassDesc.h>
class DrawPassGL : public Initable
{
private:
	PassDesc _desc;
	uint _depth_buffer_pointer = 0;
	uint _framebuffer_id = 0;
	uint N;
	std::unique_ptr< uint[] > __texture_pointer_array;
public:
	DrawPassGL() = default;
	void init(const PassDesc &);
	uint getBufferPtr( int ) const;
	uint getBufferCount();
	void clear(bool cd = true);
	void bind() const;
	void bind( uint , uint , uint ) const;
	void bindDepth( uint , uint ) const;
	uint getDepthBufferPtr() const;
	void release() override;
	void update(const Size &);
};
class UnpackBufferGL :public Initable
{
private:
	uint _buffer_id;
	uint _texture_id;
	//std::unique_ptr< char[] > _buffer_data;
	int _gl_type;
	int _gl_store;
	int _gl_format;
	Size _size;
public:
	void init( const Size & , uint , int );
	uint getBufferPtr() const;
	void unpack();
	void bind( uint , uint ) const;
	void release() override;
	void update( const Size & );
};
#endif // RPASSGL_H
