#pragma once
#include <base/Base.h>
#include <base/Eventer.h>
class GUIElem
{
public:
	class GUIFloat
	{
	public:
		static const int LEFT = 0 , RIGHT = 1 , TOP = 0 , BOTTOM = 1 , CENTER = 2;
	};
	class GUIStatus
	{
	public:
		static const int CLICKED = 0 , NONE = 1 , HOVER = 2 , DRAG = 3;
	};
	std::string _text;
	f2 _size_pix;
	mutable f2 _calculated_pos;
	mutable f2 _calculated_size;
	int _floatX , _floatY , _layer;
	int _status;
	bool _visible;
	std::function< void() > _onClick;
	std::function< void( f2 const & ) > _onDrag;
	std::vector< std::shared_ptr< GUIElem > > _child;
};
class GUILayout
{
private:
	std::vector< std::shared_ptr< GUIElem > > _elems;
public:
	GUILayout() = default;
	GUILayout( GUILayout const & ) = delete;
	GUILayout operator=( GUILayout const & ) = delete;
	void addElem( std::shared_ptr< GUIElem > elem )
	{
		_elems.push_back( elem );
	}
	std::vector< std::shared_ptr< GUIElem > > const getElemVec() const
	{
		return _elems;
	}
};