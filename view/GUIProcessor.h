#pragma once
#include <base\Base.h>
#include <view\GUILayot.h>
class GUIProcessor : public Initable
{
public:
	static void process( GUILayout const * , MouseStates const * );
};