#include <cudaRayTracerScene\scene.h>
/*DEVICE void Scene::addObj( Object *obj )
{
	this->_obj_list.push_back( obj );
}*/
DEVICE f4 Scene::traceRay( Ray const &ray , int depth , unsigned int rand_seed ) const
{
	if( depth < 0 )
		return f4( 0.0f );
	Collision col;
	{
		Collision tcol;
		bool success = false;
		float td2 = 9999999999.0f;
		for( int i = 0; i < _obj_count; i++ )
		{
			if( _obj_list[ i ]->getCollision( ray , tcol ) )
			{
				success = true;
				if( tcol._dist2 < td2 )
				{
					td2 = tcol._dist2;
					col = tcol;
				}
			}
		}
		if( !success )
		{
			return f4( 0.4f + 0.6f * abs( ray._v.z() ) );
		}
	}
	f4 out( 0.0f );
	{
		const unsigned int SAMPLES_COUNT = 100;
		for( int i = 0; i < SAMPLES_COUNT; i++ )
		{
			f3 rand_vec = VectorFactory::getRandomSphere( rand_seed );
			if( rand_vec * col._n < 0.0f ) rand_vec.mul( -1.0f );
			out += traceRay( { col._pos , rand_vec } , depth - 1 , rand_seed );
		}
		out.mul( 1.0f / SAMPLES_COUNT );
	}
	return out;
}
DEVICE bool Sphere::getCollision( Ray const &ray , Collision &out ) const
{
	f3 dp = ray._pos - _pos;
	float b = 2.0f * dp * ray._v;
	if( b > 2.0f * _radius - 0.01 )
		return false;
	float c = dp * dp - _radius * _radius;
	float d = b * b - 4.0f * c;
	if( d < 0.0f )
		return false;
	d = sqrtf( d );
	float t = ( - b - d ) * 0.5f;
	if( t < 0.0f )
		t += d;
	if( t < 0.001f )
		return false;
	out._dist2 = t * t;
	out._pos = ray._pos + ray._v * t;
	out._n = ( out._pos - _pos ).g_norm();
	out._collided = true;
	return true;
}