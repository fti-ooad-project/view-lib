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
	f3 _size = f3( 1.0f , 1.0f , 1.0f );
	void init(){}
	bool isInstanced() const;
	void genInstancedBuffer( uint , std::vector< uint > const & );
	void genVboFromMesh( std::unique_ptr< Polymesh > && );
	void draw() const;
	void release();
	void drawInstanced( void const * , uint , uint ) const;
	void drawInstancedPatches( void const * , uint , uint ) const;
};
struct PolyQuadGL : public PolyMeshGL
{
	void init() override;
	static PolyQuadGL* getSingleton();
	~PolyQuadGL();
};
struct PolyBoxGL : public PolyMeshGL
{
	void init() override;
	~PolyBoxGL();
};
#endif // RDRAWABLEGL_H
