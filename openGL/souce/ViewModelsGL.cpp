#include <openGL\ViewModelsGL.h>
#include <openGL\GLincludes.h>
void GraphicProgrammGL::init( std::string frag_file , std::string vert_file , std::string geom_file , std::string tess_file , std::string tessest_file )
{
	std::string fsrc = FileParser::genParsed( frag_file );
	std::string vsrc = FileParser::genParsed( vert_file );
	std::string gsrc = "";
	std::string tsrc = "";
	std::string tesrc = "";
	if( geom_file.size() > 1 ) gsrc = FileParser::genParsed( geom_file );
	if( tess_file.size() > 1 ) tsrc = FileParser::genParsed( tess_file );
	if( tessest_file.size() > 1 ) tesrc = FileParser::genParsed( tessest_file );
	initDirect( fsrc , vsrc , gsrc , tsrc , tesrc );
}
/*void RGraphicProgrammGL::bind(RShaderInput &in)
{
	glUseProgram(_m_program);
	uint tc = 0;///five becouse polymeshes bind textures from 0 to 5( diffuse, spec , normals... )
	for (uint i = 0; i < RShaderInput::MAX_INPUT; ++i)
	{
		if (in._loaded[i]) continue;
		in._loaded[i] = true;
		setUniform(in.__tuple[i], in.__id[i], tc);
	}
}*/
void GraphicProgrammGL::initDirect( std::string frag_file , std::string vert_file , std::string geom_file , std::string tess_file , std::string tessest_file )
{
	try
	{
		auto proc = []( uint type , std::string const &raw )
		{
			int out = glCreateShader( type );
			GLint l = raw.length();
			char const *temp = raw.c_str();
			glShaderSource( out , 1 , &temp , &l );
			glCompileShader( out );
			GLint compiled;
			glGetShaderiv( out , GL_COMPILE_STATUS , &compiled );
			if( !compiled )
			{
				GLint length;
				glGetShaderiv( out , GL_INFO_LOG_LENGTH , &length );
				std::string log( length , ' ' );
				glGetShaderInfoLog( out , length , &length , &log[ 0 ] );
				std::stringstream temp( raw );
				std::string t;
				int i = 1;
				while( std::getline( temp , t ) )
				{
					LOG << i << ":" << t << "\n";
					i++;
				}
				printf( "Shader Log Data: %s\n" , log.data() );
				throw std::logic_error( "shader compilation error" );
			}
			return out;
		};
		_vertex_program = proc( GL_VERTEX_SHADER , vert_file );
		_fragment_program = proc( GL_FRAGMENT_SHADER , frag_file );
		if( geom_file.size() > 1 ) _geometric_program = proc( GL_GEOMETRY_SHADER , geom_file );
		if( tess_file.size() > 1 ) _tess_program = proc( GL_TESS_CONTROL_SHADER , tess_file );
		if( tessest_file.size() > 1 ) _tess_est_program = proc( GL_TESS_EVALUATION_SHADER , tessest_file );
		_m_program = glCreateProgram();
		glAttachShader( _m_program , _vertex_program );
		glAttachShader( _m_program , _fragment_program );
		if( _geometric_program ) glAttachShader( _m_program , _geometric_program );
		if( _tess_program ) glAttachShader( _m_program , _tess_program );
		if( _tess_est_program ) glAttachShader( _m_program , _tess_est_program );
		glLinkProgram( _m_program );
		GLint compiled;
		glGetProgramiv( _m_program , GL_LINK_STATUS , &compiled );
		if( !compiled )
		{
			GLint length;
			glGetProgramiv( _m_program , GL_INFO_LOG_LENGTH , &length );
			std::string log( length , ' ' );
			glGetProgramInfoLog( _m_program , length , &length , &log[ 0 ] );
			//#ifdef RLOG
			LOG << "shader link error: " << log << "\n";
			//#endif
			throw std::logic_error( "shader linkage error" );
			return;
		}
	}
	catch( std::logic_error &err )
	{
		LOG << "shader compile exception: " << err.what() << "\n";
	}
}
void GraphicProgrammGL::bind()
{
	glUseProgram( _m_program );
}
void GraphicProgrammGL::release()
{
	if( !_m_program ) glDeleteProgram( _m_program );
	if( !_vertex_program ) glDeleteShader( _vertex_program );
	if( !_fragment_program ) glDeleteShader( _fragment_program );
	if( !_geometric_program ) glDeleteShader( _geometric_program );
	if( !_tess_program ) glDeleteShader( _tess_program );
	if( !_tess_est_program ) glDeleteShader( _tess_est_program );
}
void GraphicProgrammGL::setUniform( RTuple &tuple , uint i , uint &tc )
{
	switch( tuple._type )
	{
		case RShaderInTypes::vec1:
		glUniform1fv( i , tuple._count , static_cast< const float* >( tuple.__data ) );
		break;
		case RShaderInTypes::vec2:
		glUniform2fv( i , tuple._count , static_cast< const float* >( tuple.__data ) );
		break;
		case RShaderInTypes::vec3:
		glUniform3fv( i , tuple._count , static_cast< const float* >( tuple.__data ) );
		break;
		case RShaderInTypes::vec4:
		glUniform4fv( i , tuple._count , static_cast< const float* >( tuple.__data ) );
		break;
		case RShaderInTypes::ivec1:
		glUniform1iv( i , tuple._count , static_cast< const int* >( tuple.__data ) );
		break;
		case RShaderInTypes::mat4:
		glUniformMatrix4fv( i , tuple._count , GL_FALSE , static_cast< const float* >( tuple.__data ) );
		break;
		case RShaderInTypes::tex:
		glActiveTexture( GL_TEXTURE0 + tc );
		glBindTexture( GL_TEXTURE_2D , *static_cast< const uint* >( tuple.__data ) );
		glUniform1i( i , tc );
		++tc;
		break;
		case RShaderInTypes::cubemap:
		glActiveTexture( GL_TEXTURE0 + tc );
		glBindTexture( GL_TEXTURE_CUBE_MAP , *static_cast< const uint* >( tuple.__data ) );
		glUniform1i( i , tc );
		++tc;
		break;
	}
}
GraphicProgrammGL::~GraphicProgrammGL()
{
	release();
}