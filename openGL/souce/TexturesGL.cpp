#include <openGL/TexturesGL.h>
#include <openGL/GLincludes.h>
#include <view/PassDesc.h>
void TextureHolderGL::bind( uint tid , uint uid ) const
{
	glActiveTexture( GL_TEXTURE0 + tid );
	if( _count == 1 )
		glBindTexture( GL_TEXTURE_2D , _texture_array_pointer );
	else
		glBindTexture( GL_TEXTURE_2D_ARRAY , _texture_array_pointer );
	glUniform1i( uid , tid );
}
uint TextureHolderGL::getCount() const
{
	return _count;
}
uint TextureHolderGL::getTexture() const
{
	return _texture_array_pointer;// __texture_pointer_array[ i ];
}
TextureHolderGL::TextureHolderGL( TextureHolderGL &&a )
{
	*this = std::move( a );
}
void TextureHolderGL::operator=( TextureHolderGL &&a )
{
	release();
	_count = a._count;
	setInited( a.isInited() );
	if( a.isInited() )
	{
		_texture_array_pointer = a._texture_array_pointer;
		a._texture_array_pointer = 0;
		//__texture_pointer_array = std::move( a.__texture_pointer_array );
	} else
	{
		//__tex_size = std::move( std::unique_ptr< RSize[] >( new RSize[ _count ] ) );
		_imgs = std::move( a._imgs );
		//__texture_pointer_array = std::move( std::unique_ptr< uint[] >( new uint[ _count ] ) );
	}
	a.release();
}
TextureHolderGL::TextureHolderGL( std::unique_ptr< Image[] > &&imgs , int count )
{
	_count = count;
	_imgs = std::move( imgs );
	//__texture_pointer_array = std::move( std::unique_ptr< uint[] >( new uint[ _count ] ) );
}
void TextureHolderGL::init( std::unique_ptr< Image[] > &&imgs , int count )
{
	*this = std::move( TextureHolderGL( std::move( imgs ) , count ) );
	init();
}
void TextureHolderGL::init()
{
	if( !_imgs )
	{
		_texture_array_pointer = 0;
		return;
	}
	if( isInited() || !_count ) return;
	setInited( true );
	auto pow2 = []( int n )
	{
		int out = 1;
		ito( 32 )
		{
			if( n & ( 1 << i ) )
				out = i;
		}
		return out;
	};
	uint i_f , s;
	switch( _imgs[ 0 ]._bytes_per_pixel )
	{
		case 4:
			i_f = GL_RGBA8;
			s = GL_RGBA;
		break;
		case 3:
			i_f = GL_RGB8;
			s = GL_RGB;
		break;
		case 1:
			i_f = GL_LUMINANCE;
			s = GL_LUMINANCE;
		break;
	}
	const int mipmaplevels = std::min( 10 , pow2( std::min( _imgs[ 0 ]._size._w , _imgs[ 0 ]._size._h ) ) );
	glGenTextures( 1 , &_texture_array_pointer );
	if( _count == 1 )
	{
		glBindTexture( GL_TEXTURE_2D , _texture_array_pointer );
		glTexStorage2D( GL_TEXTURE_2D , mipmaplevels , i_f , _imgs[ 0 ]._size._w , _imgs[ 0 ]._size._h );
		glTexSubImage2D( GL_TEXTURE_2D , 0 , 0 , 0 , _imgs[ 0 ]._size._w , _imgs[ 0 ]._size._h , s , GL_UNSIGNED_BYTE , _imgs[ 0 ].__data.get() );
		glGenerateMipmap( GL_TEXTURE_2D );
		glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT );
	} else
	{
		glBindTexture( GL_TEXTURE_2D_ARRAY , _texture_array_pointer );
		glTexStorage3D( GL_TEXTURE_2D_ARRAY , mipmaplevels , i_f , _imgs[ 0 ]._size._w , _imgs[ 0 ]._size._h , _count );
		for( uint i = 0; i < _count; i++ )
		{
			glTexSubImage3D( GL_TEXTURE_2D_ARRAY , 0 , 0 , 0 , i , _imgs[ 0 ]._size._w , _imgs[ 0 ]._size._h , 1 , s , GL_UNSIGNED_BYTE , _imgs[ i ].__data.get() );
		}
		glGenerateMipmap( GL_TEXTURE_2D_ARRAY );
		glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_WRAP_S , GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_WRAP_T , GL_REPEAT );
	}
	_imgs.reset();
}
void TextureHolderGL::init( uint width , uint height , int type , uint bbp )
{
	if( isInited() ) return;
	setInited( true );
	_count = 1;
	auto pow2 = []( int n )
	{
		int out = 1;
		ito( 32 )
		{
			if( n & ( 1 << i ) )
				out = i;
		}
		return out;
	};
	int _gl_type;
	int _gl_store;
	int _gl_format;
	switch( type )
	{
	case BufferStoreType::BUFFER_BYTE:
	{
		switch( bbp )
		{
		case 1:
		{
			_gl_type = GL_R8;
			_gl_format = GL_R;
			_gl_store = GL_UNSIGNED_BYTE;
		}
		break;
		case 2:
		{
			_gl_type = GL_RG8;
			_gl_format = GL_RG;
			_gl_store = GL_UNSIGNED_BYTE;
		}
		break;
		case 3:
		{
			_gl_type = GL_RGB8;
			_gl_format = GL_RGB;
			_gl_store = GL_UNSIGNED_BYTE;
		}
		break;
		case 4:
		{
			_gl_type = GL_RGBA8;
			_gl_format = GL_RGBA;
			_gl_store = GL_UNSIGNED_BYTE;
		}
		break;
		}
	}
	break;
	case BufferStoreType::BUFFER_FLOAT:
	{
		switch( bbp )
		{
		case 1:
		{
			_gl_format = GL_R;
			_gl_type = GL_R32F;
			_gl_store = GL_FLOAT;
		}
		break;
		case 2:
		{
			_gl_format = GL_RG;
			_gl_type = GL_RG32F;
			_gl_store = GL_FLOAT;
		}
		break;
		case 3:
		{
			_gl_format = GL_RGB;
			_gl_type = GL_RGB32F;
			_gl_store = GL_FLOAT;
		}
		break;
		case 4:
		{
			_gl_format = GL_RGBA;
			_gl_type = GL_RGBA32F;
			_gl_store = GL_FLOAT;
		}
		break;
		}
	}
	break;
	case BufferStoreType::BUFFER_INT:
	{
		switch( bbp )
		{
		case 1:
		{
			_gl_type = GL_R32UI;
			_gl_store = GL_UNSIGNED_INT;
			_gl_format = GL_RED_INTEGER;
		}
		break;
		case 2:
		{
			_gl_type = GL_RG32UI;
			_gl_store = GL_UNSIGNED_INT;
			_gl_format = GL_RG_INTEGER;
		}
		break;
		case 3:
		{
			_gl_type = GL_RGB32UI;
			_gl_store = GL_UNSIGNED_INT;
			_gl_format = GL_RGB_INTEGER;
		}
		break;
		case 4:
		{
			_gl_type = GL_RGBA32UI;
			_gl_store = GL_UNSIGNED_INT;
			_gl_format = GL_RGBA_INTEGER;
		}
		break;
		}
	}
	break;
	}
	const int mipmaplevels = 1;// std::min( 10 , pow2( std::min( width , height ) ) );
	glGenTextures( 1 , &_texture_array_pointer );
	glBindTexture( GL_TEXTURE_2D , _texture_array_pointer );
	//glTexStorage2D( GL_TEXTURE_2D , mipmaplevels , _gl_type , width , height );
	//glTexSubImage2D( GL_TEXTURE_2D , 0 , 0 , 0 , width , height , _gl_format , _gl_store , NULL );
	glTexImage2D( GL_TEXTURE_2D , 0 , _gl_type , width , height , 0 , _gl_format , _gl_store , nullptr );
	//glTexImage2D( GL_TEXTURE_2D , 0 , GL_RGBA32F , width , height , 0 , GL_RGBA , GL_FLOAT , nullptr );
	//glGenerateMipmap( GL_TEXTURE_2D );
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT );
	glBindTexture( GL_TEXTURE_2D , 0 );
}
void TextureHolderGL::setRepeat( bool ref )
{
	if( ref )
	{
		if( _count == 1 )
		{
			glBindTexture( GL_TEXTURE_2D , _texture_array_pointer );
			glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT );
		} else
		{
			glBindTexture( GL_TEXTURE_2D_ARRAY , _texture_array_pointer );
			glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_WRAP_S , GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_WRAP_T , GL_REPEAT );
		}
	} else
	{
		if( _count == 1 )
		{
			glBindTexture( GL_TEXTURE_2D , _texture_array_pointer );
			glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE );
		} else
		{
			glBindTexture( GL_TEXTURE_2D_ARRAY , _texture_array_pointer );
			glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_WRAP_S , GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D_ARRAY , GL_TEXTURE_WRAP_T , GL_CLAMP_TO_EDGE );
		}
	}
}
TextureHolderGL::~TextureHolderGL()
{
	release();
}
void TextureHolderGL::release()
{
	if( !isInited() ) return;
	setInited( false );
	/*if( __texture_pointer_array )
	{
		glDeleteTextures( _count , __texture_pointer_array.get() );
		__texture_pointer_array.reset();
	}*/glDeleteTextures( 1 , &_texture_array_pointer );
	//__tex_size.reset();
}