#include "../base/RFileloader.h"
#include "../view/3dgl/RVertex.h"
std::unique_ptr< RImage[] > RFileLoader::loadImage( std::string filename )
{
	return std::move( loadImage( &filename , 1 ) );
}
std::unique_ptr< RImage[] > RFileLoader::loadImage( std::string *filename , int count )
{
	if( !IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG ) )
	{
		printf( "SDL_image could not initialize! SDL_image Error: %s\n" , IMG_GetError() );
	}
	std::unique_ptr< RImage[] > out( new RImage[ count ] );
	ito( count )
	{
		SDL_Surface *loadedSurface = IMG_Load( filename[ i ].c_str() );
		//IMG_LoadPNG_RW
		if( loadedSurface == NULL )
		{
			printf( "Unable to load image %s! SDL_image Error: %s\n" , filename[ i ].c_str() , IMG_GetError() );
		}
		uint bpp = loadedSurface->pitch / loadedSurface->w;
		std::unique_ptr< char[] > data( new char[ loadedSurface->h * loadedSurface->w * bpp ] );
		memcpy( data.get() , loadedSurface->pixels , loadedSurface->h * loadedSurface->w * bpp );
		out[ i ] = std::move( RImage( std::move( data ) , RSize{ uint( loadedSurface->w ) , uint( loadedSurface->h ) } , bpp ) );
		SDL_FreeSurface( loadedSurface );
	}
	IMG_Quit();
	return std::move( out );
}
std::unique_ptr< RImage[] > RFileLoader::loadImageBin( std::shared_ptr< std::ifstream > stream , int count )
{
	std::unique_ptr< RImage[] > out( new RImage[ count ] );
	ito( count )
	{
		uint h , w;
		stream->read( ( char * )&w , sizeof( int ) );
		stream->read( ( char * )&h , sizeof( int ) );
		std::unique_ptr< char[] > data( new char[ h * w * 4 ] );
		stream->read( ( char * )data.get() , 4 * w * h );
		out[ i ] = std::move( RImage( std::move( data ) , RSize{ w , h } , 4 ) );
	}
	return std::move( out );
}
std::unique_ptr< RPolymesh > RFileLoader::loadPolyMeshBin( std::shared_ptr< std::ifstream > stream , int type )
{
	std::unique_ptr< RPolymesh > out( new RPolymesh() );
	out->_flags = 0;
	out->_type = type;
	switch( type )
	{
		case RPolymesh::RPolyMeshType::RBONED_PMESH:
		{
			out->_flags |= ShaderMask::MASK_ANIMATED;
			stream->read( ( char * )&out->_v3size , sizeof( float ) * 3 );
			stream->read( ( char * )&out->_bone_count , sizeof( unsigned int ) );
			stream->read( ( char * )&out->_face_count , sizeof( unsigned int ) );
			stream->read( ( char * )&out->_vertex_count , sizeof( unsigned int ) );
			out->__vertices = std::move( std::unique_ptr< char[] >( reinterpret_cast< char* >( new RVertex[ out->_vertex_count ] ) ) );
			out->__indeces = std::move( std::unique_ptr< unsigned short[] >( new unsigned short[ out->_face_count * 3 ] ) );
			stream->read( ( char * )out->__vertices.get() , sizeof( RVertex )*out->_vertex_count );
			//ito( out->_vertex_count )
			//	PRINT( reinterpret_cast< RVertex* >( out->__vertices.get() )[i].BoneIndex[0] );
			stream->read( ( char * )out->__indeces.get() , sizeof( unsigned short )*out->_face_count * 3 );
		}
		break;
		case RPolymesh::RPolyMeshType::RSTATIC_PMESH:
		{
			stream->read( ( char * )&out->_v3size , sizeof( float ) * 3 );
			stream->read( ( char * )&out->_face_count , sizeof( unsigned int ) );
			stream->read( ( char * )&out->_vertex_count , sizeof( unsigned int ) );
			out->__vertices = std::move( std::unique_ptr< char[] >( reinterpret_cast< char* >( new RStaticVertex[ out->_vertex_count ] ) ) );
			out->__indeces = std::move( std::unique_ptr< unsigned short[] >( new unsigned short[ out->_face_count * 3 ] ) );
			stream->read( ( char * )out->__vertices.get() , sizeof( RStaticVertex )*out->_vertex_count );
			stream->read( ( char * )out->__indeces.get() , sizeof( unsigned short )*out->_face_count * 3 );
		}
		break;
	}
	int img_count;
	stream->read( ( char* )&img_count , sizeof( int ) );
	out->_texture_count = img_count;
	out->_anim_count = 0;
	if( img_count > 0 )
	{
		out->_flags |=
			ShaderMask::MASK_TEXTURED
			| ShaderMask::MASK_TEXTURED_DIF
			| ShaderMask::MASK_TEXTURED_NOR
			| ShaderMask::MASK_TEXTURED_SPE
			;
		std::unique_ptr< std::string[] > fnames( new std::string[ img_count ] );
		ito( img_count )
		{
			int l;
			stream->read( ( char* )&l , sizeof( int ) );
			std::unique_ptr< char[] > fname( new char[ l + 1 ] );
			stream->read( fname.get() , sizeof( char ) * l );
			fname.get()[ l ] = '\0';
			std::string respath( "res/view/images/" );
			respath.append( std::string( fname.get() ) );
			fnames[ i ] = std::move( std::string( respath ) );
		}
		out->_textures = std::move( loadImage( fnames.get() , img_count ) );
	}
	if( type != RPolymesh::RPolyMeshType::RBONED_PMESH )
	{
		stream->close();
		return std::move( out );
	}
	int anim_count;
	stream->read( ( char * )&anim_count , sizeof( uint ) );
	out->_anim_count = anim_count;
	if( anim_count )
	{
		out->_flags |= ShaderMask::MASK_OWN_ANIMATED;
		out->__mat4anim = std::move( loadAnimSetBin( stream , anim_count ) );

	}
	//LOG<<out->__mat4anim[0]._bone_count;
	stream->close();
	return std::move( out );
}
uint RFileLoader::binarize( uint c )
{
	uint t;
	ito( 32 )
	{
		t = 1 << i;
		if( t > c ) return t;
	}
	return 0;
}
std::unique_ptr< RAnimationset[] > RFileLoader::loadAnimSetBin( std::shared_ptr< std::ifstream > stream , int count )
{
	std::unique_ptr< RAnimationset[] > out( new RAnimationset[ count ] );
	/*int anim_count = 0;
	stream->read( ( char* )&anim_count , sizeof( int ) );
	int bcount = 0;
	stream->read( ( char* )&bcount , sizeof( int ) );
	int *skelet = new int[bcount*2];
	stream->read( ( char* )skelet , bcount * 2 * sizeof( int ) );/////*/
	ito( count )
	{
		int bc , fc;
		stream->read( ( char * )&bc , sizeof( uint ) );
		stream->read( ( char * )&fc , sizeof( uint ) );
		///ATWECHAU PEREPISATI BLYAAAATY
		//out[i].__data = std::move( std::unique_ptr< f4x4[] >( new f4x4[fc*bc] ) );
		//stream->read( ( char * )out[i].__data.get() , sizeof( f4x4 )*fc*bc );
		std::unique_ptr< f4x4[] > data( new f4x4[ fc*bc ] );
		stream->read( ( char * )data.get() , sizeof( f4x4 ) * fc * bc );
		out[ i ] = std::move( RAnimationset( std::move( data ) , fc , bc ) );
	}
	return std::move( out );
}
/*static void loadScene( const char *filename , std::shared_ptr< RSceneDsc > out_scene )
{
try
{
RDrawableState &box = out_scene->getInstanceState( out_scene->addInstance(
out_scene->genDrawable( RDrawableType::RDRAWBL_BOX ) ) );
box.getModelMat().scale( 10.0f );
box.getModelMat()( 3 , 2 ) = 30.0f;
RDrawableState &quad = out_scene->getInstanceState( out_scene->addInstance(
out_scene->genDrawable( RDrawableType::RDRAWBL_QUAD ) ) );
quad.getModelMat().scale( 100.0f );
//MARK

std::shared_ptr< std::ifstream > orc_s = getStream( "res/view/polymodels/monkey.bin" );
RDrawablePTR orc_ptr = out_scene->genPolyMesh( loadPolyMeshBin( orc_s , RPolymesh::RPolyMeshType::RBONED_PMESH ) );
std::unique_ptr< RAnimationset[] > orc_sets{ std::move( loadAnimSetBin( orc_s ) ) };
out_scene->genBoneAnimInTex( std::move( orc_sets ) , 1 );
int l = 10;
ifor( i , 0 , l )
ifor( j , 0 , l )
{
RDrawableState &state = out_scene->getInstanceState( out_scene->addInstance( orc_ptr ) );
state.getModelMat()( 3 , 0 ) = 3.0f * ( i - l / 2 );
state.getModelMat()( 3 , 1 ) = 3.0f * ( j - l / 2 );
}
out_scene->addLightSource( RLightState( true , RLightSourceType::RLIGHT_OMNI ,
RPassDesc( 512 , 512 , 1 , -1 , RBufferType::RBUFFER_FLOAT ) , f4( 1.0f , 1.0f , 1.0f , 1.0f ) ) );
out_scene->addLightSource( RLightState( true , RLightSourceType::RLIGHT_DIRECT ,
RPassDesc( 2048 , 2048 , 1 , -1 , RBufferType::RBUFFER_FLOAT ) , f4( 1.0f , 1.0f , 1.1f , 1.0f ) ) );
}catch( std::logic_error const &err )
{
printf( err.what() );
}
}*/
std::shared_ptr< std::ifstream > RFileLoader::getStream( const char *filename )
{
	std::shared_ptr< std::ifstream > out( new std::ifstream( filename , std::ios::in | std::ios::binary ) );
	if( !out->is_open() )
		throw std::logic_error( "cannot open file\n" );
	return out;
}
std::string RFileLoader::loadFile( std::string filename )
{
	std::ifstream file( filename );
	if( file.is_open() )
	{
		std::stringstream  out;
		std::string temp( "" );
		while( std::getline( file , temp ) )
		{
			out << temp << "\n";
		}
		out << "\0";
		file.close();
#ifdef RLOG
		std::cout << filename << " is loaded with " << out.str().size() << " char\n";
#endif
		return out.str();
		} else
	{
#ifdef RLOGERROR
		std::cout << filename << " is not loaded\n";
#endif
		throw std::logic_error( filename );
	}
	}
std::string RFileLoader::append( std::string const &t1 , std::string const &t2 )
{
	char *temp = new char( t1.length() + t2.length() );
	memcpy( temp , t1.c_str() , t1.length() );
	memcpy( temp + t1.length() , t2.c_str() , t2.length() );
	return std::string( temp );
}