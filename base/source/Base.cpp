#include <base/Base.h>
Size::Size(uint w , uint h) : _w(w), _h(h) {}
Size::Size(const Size &size) : _w(size._w), _h(size._h) {}
void Timer::updateTime()
{
	_last_time = _cur_time;
	//std::time_t temp = std::time( nullptr );
	clock_t time = clock();
	_cur_time = 0.001f * static_cast<float>(time * TIMERK);//QTime::currentTime().msecsSinceStartOfDay();
	_dt = fabs(_cur_time - _last_time);
}
Initable::Initable()
{
#if DEBUG
	//        printf( "__initable added in super %i\n" , ++_c );
#endif
}
void Initable::setInited(bool i)
{
	_inited = i;
}
bool Initable::isInited() const
{
	return _inited;
}
void Initable::init() {}
Initable::~Initable()
{
#if DEBUG
	//        printf( "__initable deleted in super %i\n" , --_c );
#endif
}