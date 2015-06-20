#include <base/Eventer.h>
KeyStates::KeyStates( const keystate *cs , const keystate *ls ) :
__cur_states( cs )
, __last_states( ls )
{
}
MouseStates::MouseStates( const keystate *cs , const keystate *ls , const f2 &cp , const f2 &lp , float mwheel , float mwheel_l ) :
__cur_states( cs )
, __last_states( ls )
, __cur_pos( cp )
, __last_pos( lp )
, _mwheel( mwheel )
, _mwheel_last( mwheel_l )
{
}
Eventer::Eventer()
{
	ito( 3 )
		_cur[ i ] = 0;
	ito( 3 )
		_last[ i ] = 1;
	ito( 2 )
		memset( __key_state[ i ] , 0 , MAX_KEY * sizeof( keystate ) );
	ito( 2 )
		memset( __mouse_state[ i ] , 0 , 3 * sizeof( keystate ) );
	ito( 2 )
		memset( __mouse_pos , 0 , 2 * sizeof( f2 ) );
	ito( 2 )
		_mwheel[ i ] = 0.0f;
}
void Eventer::setMouseFunc( MouseFunc func )
{
	_mouse_func = func;
}
void Eventer::setKeyFunc( KeyFunc func )
{
	_key_func = func;
}
void Eventer::setTimeFunc( TimeFunc func )
{
	_time_func = func;
}
void Eventer::update( const keystate *in_keys , const keystate *in_mous , const f2 *mp , float mwheel )
{
	if( in_keys != nullptr )
	{
		_last[ 0 ] = _cur[ 0 ];
		_cur[ 0 ] = ( _cur[ 0 ] + 1 ) & 1;
		memcpy( __key_state[ _cur[ 0 ] ] , in_keys , MAX_KEY * sizeof( keystate ) );
	}
	if( in_mous != nullptr )
	{
		_last[ 1 ] = _cur[ 1 ];
		_cur[ 1 ] = ( _cur[ 1 ] + 1 ) & 1;
		memcpy( __mouse_state[ _cur[ 1 ] ] , in_mous , 3 * sizeof( keystate ) );
	}
	if( mp != nullptr )
	{
		_last[ 2 ] = _cur[ 2 ];
		_cur[ 2 ] = ( _cur[ 2 ] + 1 ) & 1;
		_mwheel[ _cur[ 2 ] ] = mwheel;
		memcpy( &__mouse_pos[ _cur[ 2 ] ] , mp , sizeof( f2 ) );
	}
	_udated = true;
}
void Eventer::run()
{
	while( _working )
	{
		if( _udated )
		{
			updateTime();
			KeyStates ks{ __key_state[ _cur[ 0 ] ] , __key_state[ _last[ 0 ] ] };
			MouseStates ms{ __mouse_state[ _cur[ 1 ] ] , __mouse_state[ _last[ 1 ] ] , __mouse_pos[ _cur[ 2 ] ] , __mouse_pos[ _last[ 2 ] ] , _mwheel[ _cur[ 2 ] ] , _mwheel[ _last[ 2 ] ] };
			if( _mouse_func )
				_mouse_func( ms , _dt );
			if( _key_func )
				_key_func( ks , _dt );
			if( _time_func )
				_time_func( _dt );
			if( _guilayout )
			{
				GUIProcessor::process( _guilayout , ms , ks );
			}
			_udated = false;
		}
		sleep( 0x3 );
	}
}
void Eventer::setGuiLayout( GUILayout const *inguil )
{
	_guilayout = inguil;
}
void Eventer::release()
{
}
Eventer::~Eventer()
{
	release();
}