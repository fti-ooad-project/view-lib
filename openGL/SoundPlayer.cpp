#include <openGL\SoundPlayer.h>
#include <SDL2\SDL_audio.h>
#include <vector>
class Sound
{
public:
	/*Sound() = default;
	Sound( Sound const & ) = delete;
	void operator=( Sound const & ) = delete;
	Sound( Sound && ) = delete;
	void operator=( Sound && ) = delete;*/
public:
	float const * _data;
	float _pos;
	uint _length;
	float _period;
	float getNext( float );
};
float Sound::getNext( float dt )
{
	if( _data == nullptr || _pos < 0.0f )
		return 0.0f;
	_pos += dt;
check:
	if( _pos > _period )
	{
		_pos = _pos - _period;
		goto check;
	}
	int indx = ( _pos / _period ) * _length;
	return _data[ indx ];
}
std::vector< Sound > sound_vector;
std::vector< std::unique_ptr< float[] > > sound_data;
float DT;
void fill_audio( void *userdata , uchar *stream , int len )
{
	float *nstream = reinterpret_cast< float * >( stream );
	int n = len / sizeof( float );
	for( int i = 0; i < n; i++ )
	{
		nstream[ i ] = 0.0f;
		//nstream[ i * 2 + 1 ] = 0.0f;
		for( int j = 0; j < sound_vector.size(); j++ )
		{
			float data = sound_vector[ j ].getNext( ::DT / n );
			nstream[ i ] += data;
			//nstream[ i * 2 + 1 ] = data;
		}
	}
}
SoundPlayer *SoundPlayer::getSingleton()
{
	static std::unique_ptr< SoundPlayer > sngl( new SoundPlayer() );
	return sngl.get();
}
void SoundPlayer::init( uint freq , uint sampl , uint channels )
{
	if( isInited() ) return;
	setInited( true );
	SDL_AudioSpec wanted;
	wanted.freq = freq;
	wanted.format = AUDIO_F32SYS;
	wanted.channels = channels;
	wanted.samples = sampl;
	wanted.callback = fill_audio;
	wanted.userdata = NULL;
	::DT = float( sampl ) / freq;
	if( SDL_OpenAudio( &wanted , NULL ) < 0 )
	{
		fprintf( stderr , "Couldn't open audio: %s\n" , SDL_GetError() );
	}

	SDL_PauseAudio( 0 );
}
void SoundPlayer::release()
{
	if( !isInited() ) return;
	setInited( false );
	SDL_CloseAudio();
}
void SoundPlayer::playSound( std::unique_ptr< float[] > && , uint , float )
{

}
uint SoundPlayer::playStream( float const *data , uint length , float period )
{
	sound_vector.push_back( Sound{ data , 0.0f , length , period } );
	return sound_vector.size() - 1;
}
void SoundPlayer::stopStream( uint sid )
{
	sound_vector[ sid ]._data = nullptr;
}
SoundPlayer::~SoundPlayer()
{
	release();
}