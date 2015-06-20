#include <openGL/DrawableGL.h>
#include <openGL/GLincludes.h>
bool PolyMeshGL::isInstanced() const
{
	return bool( _instanced_buf );
}
void PolyMeshGL::genInstancedBuffer( int start , std::vector< int > const &comp )
{
	if( _instanced_buf )
		return;
	glBindVertexArray( _vao );
	glGenBuffers( 1 , &_instanced_buf );
	glBindBuffer( GL_ARRAY_BUFFER , _instanced_buf );
	int datasize = 0;
	ito( comp.size() )
	{
		datasize += sizeof( float ) * comp[ i ];
	}
	int offset = 0;
	ito( comp.size() )
	{
		int pos = i + start;
		glVertexAttribPointer( pos , comp[ i ] , GL_FLOAT , GL_FALSE , datasize , reinterpret_cast< void* >( offset ) );
		glEnableVertexAttribArray( pos );
		glVertexAttribDivisor( pos , 1 );
		offset += comp[ i ] * sizeof( float );
	}
	glBindBuffer( GL_ARRAY_BUFFER , 0 );
	glBindVertexArray( 0 );
}
void PolyMeshGL::draw() const
{
	glBindVertexArray( _vao );
	glDrawElements( GL_TRIANGLES , _indx_count , GL_UNSIGNED_SHORT , 0 );
}
void PolyMeshGL::genVboFromMesh( std::unique_ptr< Polymesh > &&mesh )
{
	if( !mesh )
		return;
	if( isInited() )
		return;
	setInited( true );
	glGenVertexArrays( 1 , &_vao );
	glBindVertexArray( _vao );
	uint vbo , ibo;
	glGenBuffers( 1 , &vbo );
	glBindBuffer( GL_ARRAY_BUFFER_ARB , vbo );
	switch( mesh->_type )
	{
	case Polymesh::PolyMeshType::BONED_PMESH:
	{
		glBufferData( GL_ARRAY_BUFFER_ARB ,
			mesh->_vertex_count * RVertexOffsets::Base , mesh->__vertices.get() ,
			GL_STATIC_DRAW_ARB );
		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray( 1 );
		glEnableVertexAttribArray( 2 );
		glEnableVertexAttribArray( 3 );
		glEnableVertexAttribArray( 4 );
		glEnableVertexAttribArray( 5 );
		glEnableVertexAttribArray( 6 );
		glVertexAttribPointer( 0 , 3 , GL_FLOAT , GL_FALSE , RVertexOffsets::Base , reinterpret_cast< void* >( 0 ) );
		glVertexAttribPointer( 2 , 3 , GL_FLOAT , GL_FALSE , RVertexOffsets::Base , reinterpret_cast< void* >( RVertexOffsets::Normal ) );
		glVertexAttribPointer( 3 , 3 , GL_FLOAT , GL_FALSE , RVertexOffsets::Base , reinterpret_cast< void* >( RVertexOffsets::BiNormal ) );
		glVertexAttribPointer( 4 , 3 , GL_FLOAT , GL_FALSE , RVertexOffsets::Base , reinterpret_cast< void* >( RVertexOffsets::TangentNormal ) );
		glVertexAttribPointer( 1 , 2 , GL_FLOAT , GL_FALSE , RVertexOffsets::Base , reinterpret_cast< void* >( RVertexOffsets::TextureCoordinate ) );
		glVertexAttribPointer( 5 , 3 , GL_FLOAT , GL_FALSE , RVertexOffsets::Base , reinterpret_cast< void* >( RVertexOffsets::BoneWeights ) );
		glVertexAttribPointer( 6 , 4 , GL_FLOAT , GL_FALSE , RVertexOffsets::Base , reinterpret_cast< void* >( RVertexOffsets::BoneIndex ) );
	}
	break;
	case Polymesh::PolyMeshType::STATIC_PMESH:
	{
		glBufferData( GL_ARRAY_BUFFER_ARB ,
			mesh->_vertex_count * RStaticVertexOffsets::Base , mesh->__vertices.get() ,
			GL_STATIC_DRAW_ARB );
		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray( 1 );
		glEnableVertexAttribArray( 2 );
		glEnableVertexAttribArray( 3 );
		glEnableVertexAttribArray( 4 );
		glVertexAttribPointer( 0 , 3 , GL_FLOAT , GL_FALSE , RStaticVertexOffsets::Base , reinterpret_cast< void* >( 0 ) );
		glVertexAttribPointer( 2 , 3 , GL_FLOAT , GL_FALSE , RStaticVertexOffsets::Base , reinterpret_cast< void* >( RStaticVertexOffsets::Normal ) );
		glVertexAttribPointer( 3 , 3 , GL_FLOAT , GL_FALSE , RStaticVertexOffsets::Base , reinterpret_cast< void* >( RStaticVertexOffsets::BiNormal ) );
		glVertexAttribPointer( 4 , 3 , GL_FLOAT , GL_FALSE , RStaticVertexOffsets::Base , reinterpret_cast< void* >( RStaticVertexOffsets::TangentNormal ) );
		glVertexAttribPointer( 1 , 2 , GL_FLOAT , GL_FALSE , RStaticVertexOffsets::Base , reinterpret_cast< void* >( RStaticVertexOffsets::TextureCoordinate ) );
	}
	break;
	}
	glGenBuffers( 1 , &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB , ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB ,
		3 * mesh->_face_count * sizeof( unsigned short ) ,
		mesh->__indeces.get() , GL_STATIC_DRAW_ARB );
	_indx_count = 3 * mesh->_face_count;
	glBindVertexArray( 0 );
	glDeleteBuffers( 1 , &vbo );
	glDeleteBuffers( 1 , &ibo );
	_size = mesh->_v3size;
#ifdef RLOG
	LOG << "_________________________\n" << "polymesh generated:vao:" << _vao << "\n";
#endif
	mesh.reset();
}
void PolyMeshGL::release()
{
	if( !isInited() ) return;
	setInited( false );
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
	if( isInited() )
		return;
	setInited( true );
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
	if( isInited() )
		return;
	setInited( true );
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
void ScreenQuadGL::init()
{
	if( isInited() )
		return;
	setInited( true );
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
ScreenQuadGL * ScreenQuadGL::getSingleton()
{
	static ScreenQuadGL *sngl = new ScreenQuadGL();
	return sngl;
}
void PolySphereGL::init( int n )
{
	if( isInited() )
		return;
	setInited( true );
	struct Triangle
	{
		ushort a , b , c;
	};
	struct Vertex
	{
		f3 pos;
		f2 tx;
		f3 norm;
	};
	std::vector< Triangle > faces;
	std::vector< f3 > vertices;
	{
		float t = ( 1.0 + sqrtf( 5.0f ) ) / 2.0f;
		vertices.push_back( { -1.0f , t , 0.0f } );
		vertices.push_back( { 1.0f , t , 0.0f } );
		vertices.push_back( { -1.0f , -t , 0.0f } );
		vertices.push_back( { 1.0f , -t , 0.0f } );
		vertices.push_back( { 0.0f , -1.0f , t } );
		vertices.push_back( { 0.0f , 1.0f , t } );
		vertices.push_back( { 0.0f , -1.0f , -t } );
		vertices.push_back( { 0.0f , 1.0f , -t } );
		vertices.push_back( { t , 0.0f , -1.0f } );
		vertices.push_back( { t , 0.0f , 1.0f } );
		vertices.push_back( { -t , 0.0f , -1.0f } );
		vertices.push_back( { -t , 0.0f , 1.0f } );

		faces.push_back( { 0 , 11 , 5 } );
		faces.push_back( { 0 , 5 , 1 } );
		faces.push_back( { 0 , 1 , 7 } );
		faces.push_back( { 0 , 7 , 10 } );
		faces.push_back( { 0 , 10 , 11 } );
		faces.push_back( { 1 , 5 , 9 } );
		faces.push_back( { 5 , 11 , 4 } );
		faces.push_back( { 11 , 10 , 2 } );
		faces.push_back( { 10 , 7 , 6 } );
		faces.push_back( { 7 , 1 , 8 } );
		faces.push_back( { 3 , 9 , 4 } );
		faces.push_back( { 3 , 4 , 2 } );
		faces.push_back( { 3 , 2 , 6 } );
		faces.push_back( { 3 , 6 , 8 } );
		faces.push_back( { 3 , 8 , 9 } );
		faces.push_back( { 4 , 9 , 5 } );
		faces.push_back( { 2 , 4 , 11 } );
		faces.push_back( { 6 , 2 , 10 } );
		faces.push_back( { 8 , 6 , 7 } );
		faces.push_back( { 9 , 8 , 1 } );
	}
	auto addMiddlePoint = [ &vertices ]( int i1 , int i2 )
	{
		f3 v1 = vertices[ i1 ];
		f3 v2 = vertices[ i2 ];
		f3 mid = ( v1 + v2 ) * 0.5f;
		vertices.push_back( mid );
		return vertices.size() - 1;
	};
	for( int i = 0; i < n; i++ )
	{
		std::vector< Triangle > nfaces;
		for( auto j : faces )
		{
			ushort a = addMiddlePoint( j.a , j.b );
			ushort b = addMiddlePoint( j.b , j.c );
			ushort c = addMiddlePoint( j.c , j.a );
			nfaces.push_back( { j.a , a , c } );
			nfaces.push_back( { j.b , b , a } );
			nfaces.push_back( { j.c , c , b } );
			nfaces.push_back( { a , b , c } );
		}
		faces.clear();
		faces = std::move( nfaces );
	}
	std::vector< Vertex > verticesf;
	for( auto i : vertices )
	{
		f3 pos = i.g_norm();
		float phi = PI * 0.5f + atanf( pos.y() / pos.x() );
		if( pos.y() < 0.0f )
			phi *= -1.0f;
		float theta = atanf( pos.z() / sqrtf( pos.x() * pos.x() + pos.y() * pos.y() ) );
		verticesf.push_back( { pos , f2( phi * 0.5f / PI + 0.5f , theta / PI + 0.5f ) , pos } );
	}
	glGenVertexArrays( 1 , &_vao );
	glBindVertexArray( _vao );
	uint vbo , ibo;
	glGenBuffers( 1 , &vbo );
	glBindBuffer( GL_ARRAY_BUFFER_ARB , vbo );
	glBufferData( GL_ARRAY_BUFFER_ARB , verticesf.size() * sizeof( Vertex ) , &verticesf[ 0 ] , GL_STATIC_DRAW_ARB );
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 0 , 3 , GL_FLOAT , GL_FALSE , sizeof( Vertex ) , reinterpret_cast< void* >( 0 ) );
	glVertexAttribPointer( 1 , 2 , GL_FLOAT , GL_FALSE , sizeof( Vertex ) , reinterpret_cast< void* >( 12 ) );
	glVertexAttribPointer( 2 , 3 , GL_FLOAT , GL_FALSE , sizeof( Vertex ) , reinterpret_cast< void* >( 20 ) );
	glGenBuffers( 1 , &ibo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB , ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB , 3 * faces.size() * sizeof( ushort ) , &faces[ 0 ] , GL_STATIC_DRAW_ARB );
	glBindVertexArray( 0 );
	glDeleteBuffers( 1 , &vbo );
	glDeleteBuffers( 1 , &ibo );
	_indx_count = faces.size() * 3;
}
PolySphereGL::~PolySphereGL()
{
	release();
}