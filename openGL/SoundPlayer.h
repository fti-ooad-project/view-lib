#pragma once
#include <base/Base.h>
#include <memory>
class SoundPlayer : public Initable
{
private:
	SoundPlayer() = default;
public:
	static SoundPlayer *getSingleton();
	void init( uint , uint , uint );
	void release() override;
	void playSound( std::unique_ptr< float[] > && , uint , float );
	uint playStream( float const * , uint , float );
	void stopStream( uint );
	~SoundPlayer();
};
