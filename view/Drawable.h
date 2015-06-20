#ifndef __RDRAWABLES_H__
#define __RDRAWABLES_H__
#include <base/Base.h>
#include <view/Textures.h>
#include <view/Anim.h>
#include <memory>
#include <list>
enum DrawableType
{
	DRAWBL_POLYMESH = 0 , DRAWBL_QUAD = 1 , DRAWBL_BOX = 2
};
class SpriteEffect
{
public:
	enum EffectType
	{
		REFFECT_NONE = -1 , REFFECT_GLOW = 0 , REFFECT_FIRE = 1
	};
	int _type;
	float _time = 0.0f;
	float _speed = 0.0;
	float _size = 1.0f;
	bool _dead = false;
	f3 _v3pos = f3( 0.0f , 0.0f , 0.0f );
	f3 _v3dir = f3( 0.0f , 0.0f , 0.0f );
	SpriteEffect( int type = REFFECT_NONE ) : _type( type ){}
	inline void update( const float dt )
	{
		_time += _speed * dt;
		if( _time > 1.0f ) _dead = true;
	}
};
class Polymesh
{
public:
	struct PolyMeshType{
		static const int BONED_PMESH = 0  , STATIC_PMESH = 1;
	};
	int				_type;
	f3				_v3size;
	uint        _vertex_count;
	uint        _face_count;
	uint        _bone_count;
	std::unique_ptr< char[] > __vertices;
	std::unique_ptr< unsigned short[] > __indeces;
	void release()
	{
		__vertices.reset();
		__indeces.reset();
	}
};
#endif
