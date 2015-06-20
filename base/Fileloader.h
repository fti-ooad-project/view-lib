#ifndef __RFileloader_H__
#define __RFileloader_H__
#include <cstring>
#include <view/Textures.h>
#include <view/Drawable.h>
#include <view/Anim.h>
#include <string>
#include <stdexcept>
#include <memory>
#ifdef _WIN32
#include <conio.h>
#endif
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <SDL2/SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif
//==========================
//	RData class 1.04.14 schreiner
//==========================
//#define ifor( x ) for( int i = 0; i < x; ++i )
class FileLoader
{
public:
	static std::unique_ptr< Image[] > loadImage( std::string*  , int );
	static std::unique_ptr< Image[] > loadImage( std::string );
	static std::unique_ptr< Image[] > loadImageBin( std::shared_ptr< std::ifstream > stream , int count );
	static std::unique_ptr< Polymesh > loadPolyMeshBin( std::shared_ptr< std::ifstream > stream , int type );
	static uint binarize( uint c );
	static std::unique_ptr< Animationset[] > loadAnimSetBin( std::shared_ptr< std::ifstream > stream , int count );
	static std::shared_ptr< std::ifstream > getStream( const char *filename );
	static std::string loadFile( std::string filename );
	static std::string append( std::string const &t1 , std::string const &t2 );
};
#endif //__RFileloader_H__
