#ifndef RTEXTURESGL_H
#define RTEXTURESGL_H
#include <view\Textures.h>
#include <algorithm>
class TextureHolderGL : public Initable
{
private:
	std::unique_ptr< Image[] > _imgs;
	uint _count = 0;
	//std::unique_ptr< uint[] > __texture_pointer_array;
	uint _texture_array_pointer;
public:
	uint getCount() const;
	uint getTexture() const;
	TextureHolderGL( TextureHolderGL && );
	void operator=( TextureHolderGL && );
	TextureHolderGL() = default;
	TextureHolderGL( std::unique_ptr< Image[] > && , int );
	void init( std::unique_ptr< Image[] > && , int );
	void init( uint , uint , int , uint );
	void bind( uint , uint ) const;
	void init();
	void setRepeat( bool );
	~TextureHolderGL();
	void release();
};
#endif // RTEXTURESGL_H
