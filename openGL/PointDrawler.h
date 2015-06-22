#pragma once
#include <base/Base.h>
#include <openGL/ViewModelsGL.h>
#include <memory>
class GraphDrawler : public Initable
{
private:
	uint _N;
	uint _vbo , _vao;
	std::unique_ptr< f2[] > _data;
	GraphicProgrammGL _prog;
public:
	void init( uint );
	void release() override;
	void draw( float const * , f3 const & , float , uint );
	~GraphDrawler();
};
