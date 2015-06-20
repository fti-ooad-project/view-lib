#ifndef __RViewModels_H__
#define __RViewModels_H__
//#include "../../hpp/linalg/RLinAlg.hpp"
#include <linalg/vec.h>
#include <linalg/mat.h>
#include <base/Base.h>
#include <functional>
/*started 29.03.14
*@schreiner
*/
class ViewTypes
{
public:
	typedef uint DrawBufferPTR;
	typedef uint DrawablePTR;
	typedef uint LightSourcePTR;
	typedef uint DrawInstancePTR;
	typedef uint BoneAnimInTexPTR;
};
class LightSourceType
{
public:
	static const int LIGHT_OMNI = 0 , LIGHT_DIRECT = 1;
};
class LightSourceShape
{
public:
	static const int LIGHTSHAPE_CIRCLE = 0 , LIGHTSHAPE_CIRCLE_SMOOTH = 1;
};
struct AnimationMixer
{
	struct Moment
	{
		float _moment = 0.0f;
		int _cur_set = 0;
		int _last_set = 0;
		float _last_moment = 0.0f;
		bool _mixing = false;
	} _moment;
	struct TimeEvent
	{
		bool _active = false;
		std::function< void() > _func;
		float _switch_time;
		bool check( float );
	} _event;
	float _speed = 1.0f;
	float _mixspeed = 1.0f;
	bool _event_shot = false;
	void update( const float );
	void change( const int , const float , TimeEvent const &event );
};
class UnitInstanceState
{
public:
	std::vector< ViewTypes::DrawablePTR > _view;
	f3 _pos;
	f3 _look;
	f3 _up;
	f3 _left;
	bool _auto_height;
	mutable int selectid = 0;
	int animset_id = -1;
	mutable f3 _size;
	mutable AnimationMixer _animstat;
};
struct LightState
{
	bool _cast_shadow;
	int _type;
	f3	_pos;
	f3	_dir;
	f4	_colori;
	float _size;
};
struct ParticleEffect
{
	f3 _pos;
	f3 _dir;
	float _radius;
	float _time;
	float _speed;
	float _depth;
	uint _particle_count;
	int _view_id;
	void update( float );
};
#endif /* __RViewModels_H__ */
