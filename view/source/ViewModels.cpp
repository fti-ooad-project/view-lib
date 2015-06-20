#include <view/ViewModels.h>
void AnimationMixer::update( const float dt )
{
	if( _moment._mixing )
	{
		_moment._moment += dt * _mixspeed;
		if( _moment._moment > 1.0f )
		{
			_moment._moment = 0.0f;
			_moment._mixing = false;
		}
	} else
	{
		_moment._moment += dt * _speed;
		if( _moment._moment > 1.0f )
		{
			_moment._moment = _moment._moment - floorf( _moment._moment );
			_event_shot = false;
		}
		if( !_event_shot )
		{
			_event_shot = _event.check( _moment._moment );
		}
	}
}
void AnimationMixer::change( const int i , const float speed , TimeEvent const &event )
{
	_moment._mixing = true;
	_moment._last_moment = _moment._moment;
	_moment._moment = 0.0f;
	_moment._last_set = _moment._cur_set;
	_moment._cur_set = i;
	_speed = speed;
	_event = event;
}
bool AnimationMixer::TimeEvent::check( float time )
{
	if( _active && _func && time >= _switch_time )
	{
		_func();
		return true;
	}
	return false;
}

void ParticleEffect::update( float dt )
{
	_time += dt * _speed;
}
