#pragma once
#include <vector>
#include <base/Base.h>
class ShaderType
{
private:
	int _type = 0;
	int _location = 0;
	void const *_data = nullptr;
public:
	void set( void const * );
	void const *get();
};
class ShaderBase : public Initable
{
private:
public:
	virtual void bind();
};
