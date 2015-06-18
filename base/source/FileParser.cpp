#include <base/FileParser.h>
uint FileParser::stringLength( const char *a )
{
	for( int i = 0;; ++i )
	{
		if( a[ i ] == '\0' ) return i;
	}
	return 0;
}
std::string FileParser::genParsed( std::string fullname )
{
	int path_pos = getFileNamePosition( fullname );
	const std::string catalog = fullname.substr( 0 , path_pos );
	Word str = FileLoader::loadFile( fullname );
	//std::cout << str << "\n";
	size_t pos = 0;
	for( const char *t = str.c_str(); *t != '\0'; ++t , ++pos )
	{
		if( *t == '$' )
		{
			size_t size = 0;
			size = toSpace( t );
			Word command = str.substr( pos , size );
			str.erase( pos , size + 1 );
			int pi = getPreproc( command );
			size = toSpace( t );
			//std::cout << size << "\n";
			Word wr = str.substr( pos , size );
			str.erase( pos , size );
			switch( pi )
			{
			case 0:
			{
				std::string fullname_( catalog );
				fullname_.append( wr );
				std::string file = FileLoader::loadFile( fullname_.c_str() );
				str.insert( pos , file );
				t = str.c_str();
				pos = 0;
			}
			break;
			}
		}
	}
	return str;
}
uint FileParser::toSpace( const char *r )
{
	uint size = 0;
	for( ; *r != '\n' && *r != ' ' && *r != '\0'; ++r , ++size )
	{
	}
	return size;
}
uint FileParser::getPreproc( Word const c )
{
	for( uint i = 0; i < RWORDS_COUNT; ++i )
		if( !bool( res_word[ i ].compare( c ) ) ) return i;
	return 0;
}
bool FileParser::compare( Word const a , Word const b )
{
	/*while( ( *a++ == *b++ ) && ( *a != '\0' && *b != '\0' ) ){}
	printf( "%c %c\n" , *a , *b );
	if( a[-1] == b[-1] && a[0] == b[0] )
	return true;*/
	return !bool( a.compare( b ) );
}
uint FileParser::getFileNamePosition( std::string &fullname )
{
	int l = fullname.size();
	char const *str_ptr = fullname.c_str();
	for( int i = l - 1; i >= 0; --i )
	{
		if( str_ptr[ i ] == '/' )
		{
			return i + 1;
		}
	}
	return 0;
}