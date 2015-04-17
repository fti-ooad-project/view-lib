#ifndef SCENE_H
#define SCENE_H
#include <cudalinalg\vec.h>
#include <thrust\device_vector.h>
#include <thrust\memory.h>
#include <memory>
struct Collision
{
	bool _collided;
	f3 _pos , _n;
	float _dist2;
};
struct Ray
{
	f3 _pos , _v;
};
struct Material
{
	float _transperency;
	float _n;
	float _spec;
	float _metallness;
};
class Object
{
private:
public:
	//thrust::device_vector< Material > _material;
	f3 _pos;
	virtual DEVICE Collision getCollision( Ray const & ) const = 0;
};
class Sphere : public Object
{
public:
	float _radius;
	DEVICE Collision getCollision( Ray const & ) const override;
};
class Scene
{
public:
	Object **_obj_list;
	unsigned int _obj_count;
public:
	//DEVICE void addObj( Object* );
	DEVICE f4 traceRay( f3 const & , f3 const & );
};
#endif