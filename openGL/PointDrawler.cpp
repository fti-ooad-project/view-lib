#include <openGL/PointDrawler.h>
#include <openGL/GLincludes.h>
static const char VERTEX_SHADER[] =
"#version 440\n"
"layout(location = 0 ) in vec2 pos;\n"
"void main() {\n"
"	gl_Position = vec4( pos , 0.0 , 1.0 );\n"
"}\n";
static const char FRAGMENT_SHADER[] =
"#version 440\n"
"layout(location=0) uniform vec3 color;\n"
"out vec4 outColor;\n"
"void main() {\n"
"	outColor = vec4( color , 1.0 );\n"
"}\n";
void GraphDrawler::init( uint N )
{
	if( isInited() )
		return;
	setInited( true );
	_N = N;
	glGenVertexArrays( 1 , &_vao );
	glBindVertexArray( _vao );
	glGenBuffers( 1 , &_vbo );
	glBindBuffer( GL_ARRAY_BUFFER , _vbo );
	_data = std::move( std::unique_ptr< f2[] >( new f2[ N ] ) );
	glBufferData( GL_ARRAY_BUFFER , N * sizeof( f2 ) , NULL , GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0 , 2 , GL_FLOAT , GL_FALSE , sizeof( f2 ) , reinterpret_cast< void* >( 0 ) );
	glBindBuffer( GL_ARRAY_BUFFER , 0 );
	glBindVertexArray( 0 );
	_prog.initDirect( FRAGMENT_SHADER , VERTEX_SHADER );
}
void GraphDrawler::release()
{
	if( !isInited() )
		return;
	setInited( false );
}
void GraphDrawler::draw( float const *data , f3 const &color , float k , uint count )
{
	int NN = count > _N ? _N : count;
	float pk = 1.0f / ( NN - 1 );
	ito( NN )
	{
		_data[ i ] = f2( i * pk * 2.0f - 1.0f , data[ i ] * k );
		//std::cout << _data[ i ].x() << " " << _data[ i ].y() << "\n";
	}
	_prog.bind();
	glUniform3fv( 0 , 1 , color.getArray() );
	glBindVertexArray( _vao );
	glBindBuffer( GL_ARRAY_BUFFER , _vbo );
	glBufferSubData( GL_ARRAY_BUFFER , 0 , NN * sizeof( f2 ) , _data.get() );
	glDrawArrays( GL_LINE_STRIP , 0 , NN );
	glBindBuffer( GL_ARRAY_BUFFER , 0 );
	glBindVertexArray( 0 );
}
GraphDrawler::~GraphDrawler()
{
	release();
}
