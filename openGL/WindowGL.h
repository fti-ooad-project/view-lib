#ifndef WINDOWGL_HPP
#define WINDOWGL_HPP
#include <thread>
#include <chrono>
#include <base/Parallel.h>
#include <base/Base.h>
#include <base/Eventer.h>
#include <functional>
#include <SDL2/SDL.h>
class WindowGL : public Timer , public Parallel , public Initable
{
public:
	typedef std::function< void( int , int ) > FrameFunc;
	typedef std::function< void() > ReleaseFunc;
protected:
	keystate	__key_state[MAX_KEY];
	keystate	__mouse_state[3];
	f2		__mouse_pos;
	float _mwheel;
	int _screen_width , _screen_height;
	SDL_Window *mainwindow;
	FrameFunc _func;
	ReleaseFunc _relf;
	Eventer *_eventer;
public:
	void init( FrameFunc  , ReleaseFunc  , Eventer * , bool , std::string );
	void release() override;
	~WindowGL() override;
public:
	void run() override;
};
#endif // WINDOWGL_HPP
