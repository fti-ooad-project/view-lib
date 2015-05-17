#pragma once
#include <view\ShaderBuilder.h>
#include <openGL\GLincludes.h>
#include <openGL\ViewModelsGL.h>
class ShaderBaseGL : public ShaderBase
{
private:
	GraphicProgrammGL _shader;
public:
	void bind() override;
	void release() override;
};