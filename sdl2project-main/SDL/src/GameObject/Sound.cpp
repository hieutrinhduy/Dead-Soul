#include "Sound.h"
#include "DataSaver.h"
#include <SDL_ttf.h>

Sound::Sound() {
    musicVolume = DataSaver::GetInstance()->LoadGame("save.dat").musicVolume;
    sfxVolume = DataSaver::GetInstance()->LoadGame("save.dat").sfxVolume;

    m_PathMusic = "Data/Sounds/";
    Init();
}

Sound::~Sound()
{
}

bool Sound::Init()
{
    bool ret = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) //SDL_INIT_AUDIO for Init SDL audio
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        ret = false;
        
    }
    //Initialize SDL_mixer
      // first argument sets the sound frequency, and 44100 is a standard frequency that works on most systems. The second argument determines the sample format, which again here we're using the default. The third argument is the number of hardware channels, and here we're using 2 channels for stereo. The last argument is the sample size, which determines the size of the chunks we use when playing sound. 2048 bytes (AKA 2 kilobyes) worked fine for me, but you may have to experiment with this value to minimize lag when playing sounds.
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        ret = false;
    }
    return ret;
}

void Sound::PlaySound(const std::string& name)
{
    auto it = m_Sounds.find(name);
    if (it != m_Sounds.end())
    {
        m_Music = it->second;
    }


    //Play the music
    if (m_Music != nullptr)
    {
        Mix_VolumeMusic(musicVolume);
        Mix_PlayMusic(m_Music, -1);
    }
    else
    {
        printf("No sound available can play");
    }
}

void Sound::PauseSound()
{
    //Pause the music
    Mix_PauseMusic();
}

void Sound::ResumeSound()
{
    //Resume the music
    Mix_ResumeMusic();
}

void Sound::StopSound()
{
    //Stop the music
    Mix_HaltMusic();
}

void Sound::LoadSound(const std::string& name)
{
    //If music is being playing
    if (Mix_PlayingMusic() > 0)
    {
        Mix_HaltMusic();
    }

    //Load music
    std::string pathFile = m_PathMusic + name;
    auto it = m_Sounds.find(name);
    if (it != m_Sounds.end())
    {
        m_Music = it->second;
        return;
    }
    m_Music = Mix_LoadMUS(pathFile.c_str());
    if (m_Music != nullptr)
    {
        m_Sounds.insert(std::pair<const std::string&, Mix_Music*>(name, (m_Music)));
    }
    else
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
    }
}


void Sound::LoadSfx(const std::string& name)
{
    auto it = m_Sfxs.find(name);
    if (it != m_Sfxs.end())
    {
        return ;
    }
	unsigned int ret = 0;
    std::string path = m_PathMusic + name;
	Mix_Chunk * chunk = Mix_LoadWAV(path.c_str());
	if (chunk == nullptr)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	else
	{
		m_Sfxs.insert(std::pair<const std::string&, Mix_Chunk*>(name,(chunk)));
		
	}
}

void Sound::PlaySfx(const std::string& name, int repeat)
{
    Mix_Chunk* chunk = NULL;
    auto it = m_Sfxs.find(name);
    if (it != m_Sfxs.end())
    {
        chunk = it->second;
    }
	if (chunk!= nullptr)
	{
        Mix_VolumeChunk(chunk, sfxVolume);
		Mix_PlayChannel(-1, chunk, repeat);
	}
    else
    {
        printf("Cannot play sfx");
    }
}


void Sound::ClearAudioResource() {
    for (auto it = m_Sfxs.begin(); it != m_Sfxs.end(); it++)
    {
        Mix_FreeChunk(it->second);
    }
    m_Sfxs.clear();
    for (auto it = m_Sounds.begin(); it != m_Sounds.end(); it++)
    {
        Mix_FreeMusic(it->second);
    }
    m_Sounds.clear();
}


void Sound::CleanUp()
{
    //Free Music
	for (auto it = m_Sfxs.begin(); it != m_Sfxs.end(); it++)
	{
		Mix_FreeChunk(it->second);
	}
	m_Sfxs.clear();
    for (auto it = m_Sounds.begin(); it != m_Sounds.end(); it++)
    {
        Mix_FreeMusic(it->second);
    }
  
   
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}



void Sound::SetMusicVolume(int volume) {
    musicVolume = volume;
    Mix_VolumeMusic(volume);
}

void Sound::SetSfxVolume(int volume) {
    sfxVolume = volume;
    for (auto it = m_Sfxs.begin(); it != m_Sfxs.end(); it++) {
        Mix_VolumeChunk(it->second, volume);
    }
}