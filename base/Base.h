#ifndef _RBase_H_
#define _RBase_H_
//#include <list>
//#include "vector"
#include <time.h>
#include <functional>
//#include <functional>
#include "../linalg/vec.h"
#include "../linalg/mat.h"
#include <memory>
//#include <SDL2/SDL.h>
#define MARK printf( "mark\n" );
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
#define MAX_KEY 0x100
#define MIN( x , y ) x < y ? x : y
#define ifor( i , x , y ) for( int i = x; i < y; ++i )
#define ito( y ) ifor( i , 0 , y )
struct Size
{
	uint _w , _h;
	Size(uint w = 0, uint h = 0);
	Size(const Size &size);
};
typedef bool keystate;
struct	KeyStates
{
	const keystate * __cur_states;
	const keystate * __last_states;
	KeyStates( const keystate * , const keystate * );
};
struct	MouseStates
{
	const keystate *__cur_states;
	const keystate *__last_states;
	const f2 __cur_pos;
	const f2 __last_pos;
	float _mwheel , _mwheel_last;
	MouseStates( const keystate * , const keystate * , const f2 & , const f2 & , float , float );
};
/*class Log
{
static std::fstream file;
public:
};
std::fstream Log::file;*/
#ifdef _WIN32
	#define TIMERK 1.0f
#else
	#define TIMERK 0.001f
#endif

#define LOG std::cout
class Timer
{
public:
	float _dt = 0.0f , _last_time = 0.0f , _cur_time = 0.0f;
	void updateTime();
};
#if RDEBUG
        #include "stdio.h"
#endif
class Initable
{
private:
	bool _inited = false;
public:
	Initable();
	void setInited(bool i);
	bool isInited() const;
	virtual void init();
	virtual void release() = 0;
	virtual ~Initable();
};
#endif
