#ifndef REVENTER_H
#define REVENTER_H
#include <base/Base.h>
#include <functional>
#include "Parallel.h"
#include <view/GUIProcessor.h>
#include <view/GUILayot.h>
typedef std::function< void( const KeyStates& , const float ) > KeyFunc;
typedef std::function< void( const MouseStates& , const float ) > MouseFunc;
typedef std::function< void( const float ) > TimeFunc;
class Eventer : public Initable , public Timer , public Parallel
{
private:
	KeyFunc _key_func = nullptr;
	MouseFunc _mouse_func = nullptr;
	TimeFunc _time_func = nullptr;
	keystate    __key_state[ 2 ][ MAX_KEY ];
	keystate	__mouse_state[ 2 ][ 3 ];
	f2			__mouse_pos[ 2 ];
	float _mwheel[ 2 ];
	int _cur[ 3 ] , _last[ 3 ];
	bool _udated;
	GUILayout const *_guilayout = nullptr;
public:
	Eventer();
	void setMouseFunc( MouseFunc );
	void setKeyFunc( KeyFunc );
	void setTimeFunc( TimeFunc );
	void setGuiLayout( GUILayout const * );
	void update( const keystate * , const keystate * , const f2 * , float );
	void release();
	void run() override;
	~Eventer();
};
#endif // REVENTER_H
