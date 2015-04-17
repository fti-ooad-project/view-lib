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
	uint getDepthBufferPtr() const;
	void release() override;
	void update(const Size &);
};
#endif // RPASSGL_H
