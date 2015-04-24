#ifndef RDRAWABLEGL_H
#define RDRAWABLEGL_H
#include <view/Drawable.h>
#include "GLincludes.h"
#include "AnimGL.h"
#include "TexturesGL.h"
#include <view/ShaderSpecs.h>
#include <view/Vertex.h>
class PolyMeshGL : public Initable
{
protected:
	uint _vao;
	int _indx_count;
	uint _instanced_buf;
public:
	int _flags = 0;
	f3 _size = f3( 1.0f , 1.0f , 1.0f );
	bool isInstanced() const;
	void genInstancedBuffer( uint , std::vector< uint > const & );
	virtual void draw() const;
	void release();
	void drawInstanced( void const * , uint , uint ) const;
	void drawInstancedPatches( void const * , uint , uint ) const;
};
struct PolyQuadGL : public PolyMeshGL
{
	void init();
	~PolyQuadGL();
};
struct PolyBoxGL : public PolyMeshGL
{
	void init();
	~PolyBoxGL();
};
#endif // RDRAWABLEGL_H
