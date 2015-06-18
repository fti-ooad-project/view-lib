#pragma once
#include <vector>
#include <linalg\vec.h>
#include <base\Base.h>
#include <base\Parallel.h>
struct Atom
{
	float _chrg;
	float _mass;
	f3 _pos;
	f3 _vel;
};
typedef std::vector< Atom > AtomVec;
typedef std::vector< f3 > f3Vec;
class MoleculeSystem : public Initable , public Parallel
{
private:
	AtomVec _atoms;
	f3Vec _positions;
	f3Vec _forces;
	void update( float dt );
public:
	void run() override;
	void addAtom( Atom const &atom );
	AtomVec const &getAtoms() const;
	f3Vec const &getPositions() const;
	void release() override;
};