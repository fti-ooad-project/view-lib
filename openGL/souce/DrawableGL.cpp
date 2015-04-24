#include <openGL\DrawableGL.h>
#include <openGL\GLincludes.h>
bool PolyMeshGL::isInstanced() const
{
	return bool( _instanced_buf );
}
void PolyMeshGL::genInstancedBuffer( uint start , std::vector< uint > const &comp )
{
	if( _instanced_buf ) return;
	glBindVertexArray( _vao );
	glGenBuffers( 1 , &_instanced_buf );
	glBindBuffer( GL_ARRAY_BUFFER , _instanced_buf );
	uint datasize = 0;
	ito( comp.size() )
		datasize += comp[ i ];
	for( int c = start; c < start + comp.size(); ++c )
	{
		glEnableVertexAttribArray( c );
		glVertexAttribDivisor( c , 1 );
	}
	ito( comp.size() )
	{
		glVertexAttribPointer( i , comp[ i ] , GL_FLOAT , GL_FALSE , datasize , reinterpret_cast< void* >( 0 ) );
	}
	glBindBuffer( GL_ARRAY_BUFFER , 0 );
	glBindVertexArray( 0 );
}
void PolyMeshGL::draw() const
{
	glBindVertexArray( _vao );
	glDrawElements( GL_TRIANGLES , _indx_count , GL_UNSIGNED_SHORT , 0 );
}
void PolyMeshGL::release()
{
	glDeleteVertexArrays( 1 , &_vao );
	glDeleteBuffers( 1 , &_instanced_buf );
}
void PolyMeshGL::drawInstanced( void const *data , uint size , uint count ) const
{
	glBindVertexArray( _vao );
	glBindBuffer( GL_ARRAY_BUFFER , _instanced_buf );
	glBufferData( GL_ARRAY_BUFFER , size , data , GL_DYNAMIC_DRAW );
	glDrawElementsInstanced( GL_TRIANGLES , _indx_count , GL_UNSIGNED_SHORT , 0 , count );
	glBindBuffer( GL_ARRAY_BUFFER , 0 );
	glBindVertexArray( 0 );
}
void PolyMeshGL::drawInstancedPatches( void const *data , uint size , uint count ) const
{
	glBindVertexArray( _vao );
	glBindBuffer( GL_ARRAY_BUFFER , _instanced_buf );
	glBufferData( GL_ARRAY_BUFFER , size , data , GL_DYNAMIC_DRAW );
	glDrawElementsInstanced( GL_PATCHES , _indx_count , GL_UNSIGNED_SHORT , 0 , count );
	glBindBuffer( GL_ARRAY_BUFFER , 0 );
	glBindVertexArray( 0 );
}
void PolyQuadGL::init()
{
	static const float quad[] =
	{
		-1.0f , -1.0f , 0.0f , 0.0f , 1.0f , 0.0f , 0.0f , 1.0f , /*bn*/1.0f , 0.0f , 0.0f , /*tn*/0.0f , 1.0f , 0.0f ,
		1.0f , -1.0f , 0.0f , 1.0f , 1.0f , 0.0f , 0.0f , 1.0f , /*bn*/1.0f , 0.0f , 0.0f , /*tn*/0.0f , 1.0f , 0.0f ,
		1.0f , 1.0f , 0.0f , 1.0f , 0.0f , 0.0f , 0.0f , 1.0f , /*bn*/1.0f , 0.0f , 0.0f , /*tn*/0.0f , 1.0f , 0.0f ,
		-1.0f , 1.0f , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 1.0f , /*bn*/1.0f , 0.0f , 0.0f , /*tn*/0.0f , 1.0f , 0.0f
	};
	static const ushort indx[] =
	{
		0 , 1 , 2 , 0 , 2 , 3
	};
	glGenVertexArrays( 1 , &_vao );
	glBindVertexArray( _vao );
	uint vbo , ibo;
	glGenBuffers( 1 , &vbo );
	glBindBuffer( GL_ARRAY_BUFFER_ARB , vbo );
	glBufferData( GL_ARRAY_BUFFER_ARB , 224 , quad , GL_STATIC_DRAW_ARB );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	glEnableVertexAttribArray( 3 );
	glEnableVertexAttribArray( 4 );
	glVertexAttribPointer( 0 , 3 , GL_FLOAT , GL_FALSE , 56 , reinterpret_cast< void* >( 0 ) );
	glVertexAttribPointer( 1 , 2 , GL_FLOAT , GL_FALSE , 56 , reinterpret_cast< void* >( 12 ) );
	glVertexAttribPointer( 2 , 3 , GL_FLOAT , GL_FALSE , 56 , reinterpret_cast< void* >( 20 ) );
	glVertexAttribPointer( 3 , 3 , GL_FLOAT , GL_FALSE , 56 , reinterpret_cast< void* >( 32 ) );
	glVertexAttribPointer( 4 , 3 , GL_FLOAT , GL_FALSE , 56 , reinterpret_cast< void* >( 44 ) );
	glGenBuffers( 1 , &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB , ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB , 12 , indx , GL_STATIC_DRAW_ARB );
	glBindVertexArray( 0 );
	glDeleteBuffers( 1 , &vbo );
	glDeleteBuffers( 1 , &ibo );
	_indx_count = 6;
}
PolyQuadGL::~PolyQuadGL()
{
	release();
}
void PolyBoxGL::init()
{
	static const float quad[] =
	{
		-1.0f , -1.0f , 1.0f , 0.0f , 0.0f , -0.6f , -0.6f , 0.6f ,
		1.0f , -1.0f , 1.0f , 1.0f , 0.0f , 0.6f , -0.6f , 0.6f ,
		1.0f , 1.0f , 1.0f , 1.0f , 1.0f , 0.6f , 0.6f , 0.6f ,
		-1.0f , 1.0f , 1.0f , 0.0f , 1.0f , -0.6f , 0.6f , 0.6f ,///
		-1.0f , -1.0f , -1.0f , 0.0f , 0.0f , -0.6f , -0.6f , -0.6f ,
		1.0f , -1.0f , -1.0f , 1.0f , 0.0f , 0.6f , -0.6f , -0.6f ,
		1.0f , 1.0f , -1.0f , 1.0f , 1.0f , 0.6f , 0.6f , -0.6f ,
		-1.0f , 1.0f , -1.0f , 0.0f , 1.0f , -0.6f , 0.6f , -0.6f ///
	};
	static const ushort indx[] =
	{
		0 , 2 , 3 , 0 , 1 , 2 ,
		4 , 7 , 6 , 4 , 6 , 5 ,
		1 , 6 , 2 , 1 , 5 , 6 ,
		0 , 3 , 7 , 0 , 7 , 4 ,
		2 , 7 , 3 , 2 , 6 , 7 ,
		1 , 0 , 4 , 1 , 4 , 5
	};
	glGenVertexArrays( 1 , &_vao );
	glBindVertexArray( _vao );
	uint vbo , ibo;
	glGenBuffers( 1 , &vbo );
	glBindBuffer( GL_ARRAY_BUFFER_ARB , vbo );
	glBufferData( GL_ARRAY_BUFFER_ARB , 256 , quad , GL_STATIC_DRAW_ARB );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 0 , 3 , GL_FLOAT , GL_FALSE , 32 , reinterpret_cast< void* >( 0 ) );
	glVertexAttribPointer( 1 , 2 , GL_FLOAT , GL_FALSE , 32 , reinterpret_cast< void* >( 12 ) );
	glVertexAttribPointer( 2 , 3 , GL_FLOAT , GL_FALSE , 32 , reinterpret_cast< void* >( 20 ) );
	glGenBuffers( 1 , &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB , ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB , 72 , indx , GL_STATIC_DRAW_ARB );
	glBindVertexArray( 0 );
	glDeleteBuffers( 1 , &vbo );
	glDeleteBuffers( 1 , &ibo );
	_indx_count = 36;
}
PolyBoxGL::~PolyBoxGL()
{
	release();
}