#ifndef RDRAWABLEGL_H
#define RDRAWABLEGL_H
#include <view/Drawable.h>
#include "GLincludes.h"
#include "AnimGL.h"
#include "TexturesGL.h"
#include <view/ShaderSpecs.h>
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
	TextureHolderGL __textures;
	BoneAnimInTexHolderGL __anim_intex;
	std::unique_ptr< Polymesh > __mesh;
	int _bone_count;
public:

	ComplexPolyMeshGL() = default;
	ComplexPolyMeshGL( ComplexPolyMeshGL && );
	void operator=( ComplexPolyMeshGL && );
	ComplexPolyMeshGL( std::unique_ptr< Polymesh > && );
	void init( std::unique_ptr< Polymesh > && );
	void init();
	void release() override;
	~ComplexPolyMeshGL();
	void bindRes( InstanceInfo const & ) const override;
};
struct PolyQuadGL : public PolyMeshGL
{
	void init();
	~PolyQuadGL();
	void bindRes( InstanceInfo const & ) const override;
};
struct PolyBoxGL : public PolyMeshGL
{
	void init();
	~PolyBoxGL();
	void bindRes( InstanceInfo const & ) const override;
};
#endif // RDRAWABLEGL_H
