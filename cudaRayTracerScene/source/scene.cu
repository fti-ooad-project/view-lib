#include <cudaRayTracerScene\scene.h>
#include <cuda_occupancy.h>
/*DEVICE void Scene::addObj( Object *obj )
{
	this->_obj_list.push_back( obj );
}*/
/*DEVICE f3 Scene::traceRay( Ray const &ray , int depth , unsigned int rand_seed , unsigned int SAMPLES_COUNT ) const
{
	Collision col;
	Material mat;
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
					mat = _obj_list[ i ]->_material;
					col = tcol;
				}
			}
		}
		if( !success )
		{
			return f3( 0.2f , 0.23f , 0.46f ) + 0.6f * ray._v.z() * f3( 1.9f , 1.6f , 1.5f );
		}
	}
	if( mat._emit )
		return mat._color;
	f3 out( 0.0f );
	if( depth > 0 )
	{
		Ray temp_ray;
		temp_ray._pos = col._pos;
		f3 refl( 0.0f );
		if( mat._transperency < 1.0f )
		{
			for( int i = 0; i < SAMPLES_COUNT; i++ )
			{
				f3 v = VectorFactory::getDiffuseReflected( ray._v , col._n , mat._spec , rand_seed );
				temp_ray._v = v;
				refl += traceRay( temp_ray , depth - 1 , rand_seed , max( 1 , SAMPLES_COUNT >> 1 ) );
			}
		}
		f3 refr( 0.0f );
		if( mat._transperency > 0.0f )
		{
			float kn;
			if( !col._inside )
				kn = 1.0f / mat._n;
			else
				kn = mat._n;
			for( int i = 0; i < SAMPLES_COUNT; i++ )
			{
				f3 v = VectorFactory::getDiffuseRefracted( ray._v , col._n , mat._spec , kn , rand_seed );
				temp_ray._v = v;
				refr += traceRay( temp_ray , depth - 1 , rand_seed , max( 1 , SAMPLES_COUNT >> 1 ) );
			}
		}
		out = mat._color & ( refr * mat._transperency + refl * ( 1.0f - mat._transperency ) );
		out.mul( 1.0f / SAMPLES_COUNT );
	}
	return out;
}*/
DEVICE void Scene::traceRayQueued( QueueRay const &qray , QueueRay *rqueue , QueuePixel * pqueue , int index , int SAMPLES_COUNT ) const
{
	Collision col;
	Material mat;
	{
		Collision tcol;
		bool success = false;
		float td2 = 9999999999.0f;
		for( int i = 0; i < _obj_count; i++ )
		{
			if( _obj_list[ i ]->getCollision( { qray._pos , qray._v } , tcol ) )
			{
				success = true;
				if( tcol._dist2 < td2 )
				{
					td2 = tcol._dist2;
					mat = _obj_list[ i ]->_material;
					col = tcol;
				}
			}
		}
		if( !success )
		{
			pqueue[ index ] = { qray._target_pixel , qray._color_k & ( f3( 0.2f , 0.23f , 0.46f ) + qray._v.z() * f3( 1.9f , 1.6f , 1.5f ) ) };
			//rqueue->writePixel( qray._color_k & ( f3( 0.2f , 0.23f , 0.46f ) + qray._v.z() * f3( 1.9f , 1.6f , 1.5f ) ) , qray._dir_pixel );
			return;
		}
		if( mat._emit )
		{
			pqueue[ index ] = { qray._target_pixel , qray._color_k & mat._color };
			//rqueue->writePixel( qray._color_k & mat._color , qray._dir_pixel );
			return;
		}
	}
	f3 out( 0.0f );
	{
		QueueRay temp_ray;
		temp_ray._pos = col._pos;
		temp_ray._target_pixel = qray._target_pixel;
		temp_ray._have_value = 1;
		f3 refl( 0.0f );
		if( mat._transperency < 1.0f )
		{
			temp_ray._color_k = qray._color_k & ( mat._color * ( 1.0f - mat._transperency ) );
			for( int i = 0; i < SAMPLES_COUNT; i++ )
			{
				f3 v = VectorFactory::getDiffuseReflected( qray._v , col._n , mat._spec , index );
				temp_ray._v = v;
				rqueue[ index * SAMPLES_COUNT + i ] = temp_ray;
			}
		}
		f3 refr( 0.0f );
		if( mat._transperency > 0.0f )
		{
			temp_ray._color_k = qray._color_k & ( mat._color * mat._transperency );
			float kn;
			if( !col._inside )
				kn = 1.0f / mat._n;
			else
				kn = mat._n;
			for( int i = 0; i < SAMPLES_COUNT; i++ )
			{
				f3 v = VectorFactory::getDiffuseRefracted( qray._v , col._n , mat._spec , kn , index );
				temp_ray._v = v;
				rqueue[ index * SAMPLES_COUNT + SAMPLES_COUNT + i ] = temp_ray;
			}
		}
	}
}
DEVICE bool Sphere::getCollision( Ray const &ray , Collision &out ) const
{
	const float EPS = 0.001f;
	f3 dp = ray._pos - _pos;
	float b = 2.0f * dp * ray._v;
	float c = dp * dp - _radius * _radius;
	float d = b * b - 4.0f * c;
	if( d < 0.0f )
		return false;
	d = sqrtf( d );
	float t = ( -b - d ) * 0.5f;
	if( t < EPS )
		t += d;
	if( t < EPS )
		return false;
	out._dist2 = t * t;
	out._pos = ray._pos + ray._v * t;
	out._n = ( out._pos - _pos ).g_norm();
	if( c < EPS )
	{
		out._n.mul( -1.0f );
		out._inside = true;
	}
	out._inside = false;
	return true;
}
DEVICE bool InfPlane::getCollision( Ray const &ray , Collision &out ) const
{
	float d = ray._v * _n;
	f3 dp = _pos - ray._pos;
	float p = dp * _n;
	float t = p / d;
	if( t < 0.0001f )
		return false;
	out._pos = ray._pos + ray._v * t;
	out._dist2 = t * t;
	if( p < 0.0f )
		out._n = _n;
	else
		out._n = -_n;
	out._inside = false;
	return true;
}