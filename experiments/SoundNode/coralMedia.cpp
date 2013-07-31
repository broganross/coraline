
#ifndef CORALMEDIA_H
#define CORALMEDIA_H

#include <math.h>
#include <boost/python.hpp>
#include <ImathVec.h>
#include <ImathRandom.h>

#include <fmod.hpp>
#include <fmod_errors.h>

#include <coral/Numeric.h>
#include <coral/pythonWrapperUtils.h>

using namespace coral;

namespace{
	FMOD::System *sys;
	FMOD::ChannelGroup *channelgroup;
	FMOD::Sound *sound;
	FMOD::Channel *channel;
}

class SoundManager{
public:
	static void init(){		
		FMOD::System_Create(&sys);
		sys->init(1, FMOD_INIT_NORMAL, 0);
		sys->getMasterChannelGroup(&channelgroup);
	}

	static void load(const std::string &filename){
		FMOD_RESULT result = sys->createSound(filename.c_str(),  FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
		
		if(result != FMOD_OK){
			std::cout << "could not load sound" << std::endl;
		}
		else{
			//unsigned int length;
			//FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_PCM);
		}
	}

	static void play(){
		bool pause = false;
		sys->playSound(FMOD_CHANNEL_FREE, sound, pause, &channel);
		sys->update();
	}

	static void stop(){
		channelgroup->stop();
	}

	static std::vector<float> spectrum(){
		int nBandsToGet = 64;
		float spectrum[64];

		channel->getSpectrum(spectrum, nBandsToGet, 0, FMOD_DSP_FFT_WINDOW_BLACKMAN);

		std::vector<float> outSpectrum(nBandsToGet);
		for(int i = 0; i < nBandsToGet; ++i){
			outSpectrum[i] = spectrum[i];
		}

		return outSpectrum;
	}

	static void setSpectrumOnNumeric(Numeric *numeric, int channelOffset){
		int nBandsToGet = 64;
		std::vector<float> outSpectrum(nBandsToGet);
		channel->getSpectrum(&outSpectrum[0], nBandsToGet, channelOffset, FMOD_DSP_FFT_WINDOW_BLACKMAN);

		numeric->setFloatValues(outSpectrum);
	}

	static void terminate(){
		sys->close();
	}
};

BOOST_PYTHON_MODULE(coralMedia){
	boost::python::class_<SoundManager>("SoundManager")
		.def("init", &SoundManager::init)
		.staticmethod("init")
		.def("terminate", &SoundManager::terminate)
		.staticmethod("terminate")
		.def("load", &SoundManager::load)
		.staticmethod("load")
		.def("play", &SoundManager::play)
		.staticmethod("play")
		.def("stop", &SoundManager::stop)
		.staticmethod("stop")
		.def("spectrum", &SoundManager::spectrum)
		.staticmethod("spectrum")
		.def("setSpectrumOnNumeric", &SoundManager::setSpectrumOnNumeric)
		.staticmethod("setSpectrumOnNumeric")
	;
}

#endif
