#pragma once
#include <openGL/PassGL.h>
#include <openGL/ViewModelsGL.h>
#include <openGL/DrawableGL.h>
#include <base/Base.h>
class Blurer : public Initable
{
private:
	DrawPassGL _process_pass;
	GraphicProgrammGL _process_prog;
	PolyQuadGL _screen_quad;
public:
	Blurer() = default;
	DrawPassGL const &process( uint , float );
	//static Blurer *getSingleton();
	void init() override;
	void release() override;
};
