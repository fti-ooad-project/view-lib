#pragma once
#include <base/Base.h>
#include <view/GUILayot.h>
class GUIProcessor
{
private:
	static int _focused_elem;
public:
	static void process( GUILayout const * , MouseStates const & , KeyStates const & );
};