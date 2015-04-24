#ifndef SCENE_H
#define SCENE_H
#include <cudalinalg\vec.h>
#include <thrust\device_vector.h>
#include <thrust\memory.h>
#include <memory>
struct Collision
{
	bool _inside;
	f3 _pos , _n;
	float _dist2;
};
struct Ray
{
	f3 _pos , _v;
};
struct QueueRay
{
	f3 _pos , _v , _color_k;
	unsigned int _dir_pixel;
};
struct Photon
{
	f3 _pos;
	f3 _v;
	f3 _color;
};
struct Material
{
	f3 _color;
	float _transperency;
	float _n;
	float _spec;
	bool _metallness;
	bool _emit;
};
class Object
{
private:
public:
	Material _material;
	f3 _pos;
	virtual DEVICE bool getCollision( Ray const & , Collision & ) const = 0;
};
class Sphere : public Object
{
public:
	float _radius;
	DEVICE bool getCollision( Ray const & , Collision & ) const override;
};
class InfPlane : public Object
{
public:
	f3 _n;
	DEVICE bool getCollision( Ray const & , Collision & ) const override;
};
class PhotonMap
{
public:
	DEVICE void setSize( f3 const & , float , unsigned int );
	DEVICE void add( Photon const & );
	DEVICE int getClosest( Photon const * ) const;
};
class RayQueue
{
private:
	int _pos;
public:
	int _array_size;
	QueueRay *_array;
	f4 *_screen;
	i2 _size;
	DEVICE void add( QueueRay const & );
	DEVICE void writePixel( f3 const & , unsigned int );
	DEVICE unsigned int getSize() const;
	DEVICE void empty();
};
class Scene
{
public:
	Object **_obj_list;
	unsigned int _obj_count;
public:
	//DEVICE void addObj( Object* );
	//DEVICE f3 traceRay( Ray const & , int , unsigned int , unsigned int ) const;
	DEVICE void traceRayQueued( QueueRay const & , RayQueue * , unsigned int , unsigned int ) const;
};
#endif