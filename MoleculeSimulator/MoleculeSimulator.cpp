#include <MoleculeSimulator\Molecule.h>
#include <MoleculeSimulator\cudaMoleculeUtils.h>
#include <iostream>
void MoleculeSystem::update( float dt )
{
	int count = _atoms.size();
	calcLennardForceVector( &_positions[ 0 ] , &_forces[ 0 ] , count );
	for( int i = 0; i < count; i++ )
	{
		auto &cur_atom = _atoms[ i ];
		/*for( int j = i + 1; j < _atoms.size(); j++ )
		{
			auto &iter_atom = _atoms[ j ];
			f3 dr = cur_atom._pos - iter_atom._pos;
			float l = dr.g_mod();
			dr = dr.g_norm();
			float ql = 5.0f / l;
			float lennard = 2.0f * ( 6.0f * powf( ql , 7.0f ) - 12.0f * powf( ql , 13.0f ) );
			f3 dvm = dt * dr * ( cur_atom._chrg * iter_atom._chrg / powf( l , 2.0f ) - lennard );
			cur_atom._vel += dvm / cur_atom._mass;
			iter_atom._vel -= dvm / iter_atom._mass;
		}*/
		cur_atom._vel += _forces[ i ] / cur_atom._mass * dt;
		cur_atom._pos += cur_atom._vel * dt;
		_positions[ i ] = cur_atom._pos;
		cur_atom._vel *= 1.0f - 0.8f * ( dt > 1.0f ? 1.0f : dt );
	}
}
void MoleculeSystem::run()
{
	Timer timer;
	while( _working )
	{
		timer.updateTime();
		update( timer._dt );
		/*if( timer._dt < 0.002f )
		{
			sleep( 0x2 );
		}*/
	}
}
void MoleculeSystem::addAtom( Atom const &atom )
{
	_atoms.push_back( atom );
	_positions.push_back( atom._pos );
	_forces.push_back( f3( 0.0f ) );
}
AtomVec const &MoleculeSystem::getAtoms() const
{
	return _atoms;
}
f3Vec const &MoleculeSystem::getPositions() const
{
	return _positions;
}
void MoleculeSystem::release()
{
	_atoms.clear();
	_positions.clear();
	_forces.clear();
}