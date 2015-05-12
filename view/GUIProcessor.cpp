#include <view\GUIProcessor.h>
void GUIProcessor::process( GUILayout const *layout , MouseStates const *states )
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
	for( auto &i : layout->getElemVec() )
	{
		if( isin( states->__cur_pos , i->_calculated_pos , i->_calculated_size ) )
		{
			if( states->__cur_states[ 0 ] )
				i->_status = 2;
			else
				i->_status = 1;
			if( !states->__cur_states[ 0 ] && states->__last_states[ 0 ] )
				if( i->_onClick )
					i->_onClick();
		} else
			i->_status = 0;
	}
}