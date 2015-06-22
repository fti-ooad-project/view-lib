#include <view/GUIProcessor.h>
#include <SDL2/SDL.h>
void GUIProcessor::process( GUILayout const *layout , MouseStates const &mousestates , KeyStates const &keystates )
{
	static auto isin = []( f2 const &p , f2 const &c , f2 const &s )
	{
		if(
			fabsf( -p.x() - c.x() ) < s.x()
			&& fabsf( -p.y() - c.y() ) < s.y()
			)
			return true;
		return false;
	};
	auto &ev = layout->getElemVec();
	bool elem_clicked = false;
	bool click = !mousestates.__cur_states[ 0 ] && mousestates.__last_states[ 0 ];
	ito( ev.size() )
	{
		if( isin( mousestates.__cur_pos , ev[ i ]->_calculated_pos , ev[ i ]->_calculated_size ) )
		{
			if( mousestates.__cur_states[ 0 ] )
				ev[ i ]->_status = 2;
			else
				ev[ i ]->_status = 1;
			if( click )
			{
				_focused_elem = i;
				elem_clicked = true;
				if( ev[ i ]->_onClick )
				{
					ev[ i ]->_onClick();
				}
			}
		} else
		{
			ev[ i ]->_status = 0;
		}
	}
	if( click && !elem_clicked )
	{
		_focused_elem = -1;
	}
	if( _focused_elem > 0 )
	{
		ev[ _focused_elem ]->_status = 2;
		ito( MAX_KEY )
		{
			if( keystates.__cur_states[ i ] && !keystates.__last_states[ i ] )
			{
				if( ev[ _focused_elem ]->_onKeyPressWhenFocused )
				{
					ev[ _focused_elem ]->_onKeyPressWhenFocused( SDL_GetKeyFromScancode( ( SDL_Scancode )i ) );
				}
			}
		}
	}
}
