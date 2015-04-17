#include <cudaRayTracerScene\scene.h>
/*DEVICE void Scene::addObj( Object *obj )
{
	this->_obj_list.push_back( obj );
}*/
DEVICE f4 Scene::traceRay( f3 const &pos , f3 const &v )
{

}
DEVICE Collision Sphere::getCollision( Ray const &ray ) const
{
	Collision out{ false };
	f3 dp = ray._pos - _pos;
	float b = 2.0f * dp * ray._v;
	if( b > 2.0f * _radius )
		return out;
	float c = dp * dp + _radius * _radius;
	float d = b * b - 4.0f * c;
	if( d < 0.0f )
		return out;
	d = sqrtf( d );
	float t = ( - b - d ) * 0.5f;
	if( t < 0.0f )
		t += d;
	out._dist2 = t * t;
	out._pos = ray._pos + ray._v * t;
	out._n = ( out._pos - _pos ).g_norm();
	out._collided = true;
	return out;
}