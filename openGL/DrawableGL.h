#ifndef RDRAWABLEGL_H
#define RDRAWABLEGL_H
#include <view/Drawable.h>
#include "GL.h"
#include "AnimGL.h"
#include "TexturesGL.h"
#include "ShaderSpecs.h"
#include <view/Vertex.h>
struct InstanceInfo
{
	float time;
	float last_time;
	float dist_to_cam;
	float selection_id;
	int mixing;
	int cur_anim;
	int last_anim;
	int auto_height;
	f3 pos;
	f3 look;
	f3 left;
	f3 up;
};
class PolyMeshGL : public Initable
{
protected:
	uint _vao;
	uint _instanced_buf = 0;
	int _indx_count;
public:
	int _flags = 0;
	f3 _size = f3( 1.0f , 1.0f , 1.0f );
public:
	bool isInstanced() const;
	void genInstancedBuffer();
	virtual void bindRes( InstanceInfo const & ) const = 0;
	void draw( InstanceInfo const & ) const;
	virtual void draw() const;
	void drawPatches( InstanceInfo const & ) const;
	void release();
	void drawInstanced( std::vector< InstanceInfo > const & );
	void drawInstancedPatches( std::vector< InstanceInfo > const & );
};
class ComplexPolyMeshGL : public PolyMeshGL
{
public:
	RTextureHolderGL __textures;
	RBoneAnimInTexHolderGL __anim_intex;
	std::unique_ptr< RPolymesh > __mesh;
	int _bone_count;
public:

	RComplexPolyMeshGL() = default;
	RComplexPolyMeshGL( RComplexPolyMeshGL && );
	void operator=( RComplexPolyMeshGL && );
	RComplexPolyMeshGL( std::unique_ptr< RPolymesh > && );
	void init( std::unique_ptr< RPolymesh > && );
	void init();
	void release() override;
	~RComplexPolyMeshGL();
	void bindRes( InstanceInfo const & ) const override;
};
struct PolyQuadGL : public PolyMeshGL
{
	void init();
	~RPolyQuadGL();
	void bindRes( InstanceInfo const & ) const override;
};
struct PolyBoxGL : public PolyMeshGL
{
	void init();
	~RPolyBoxGL();
	void bindRes( InstanceInfo const & ) const override;
};
#endif // RDRAWABLEGL_H
