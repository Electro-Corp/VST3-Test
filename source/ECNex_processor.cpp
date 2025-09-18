//------------------------------------------------------------------------
// Copyright(c) 2025 Electro-Corp.
//------------------------------------------------------------------------

#include "ECNex_processor.h"
#include "ECNex_cids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"

using namespace Steinberg;

namespace ElectroCorp {
//------------------------------------------------------------------------
// CECNexProcessor
//------------------------------------------------------------------------
CECNexProcessor::CECNexProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kCECNexControllerUID);
}

//------------------------------------------------------------------------
CECNexProcessor::~CECNexProcessor ()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
	addEventInput (STR16 ("Event In"), 1);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::process (Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			auto* paramQueue = data.inputParameterChanges->getParameterData (index);
			if (paramQueue)
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				switch (paramQueue->getParameterId())
				{
					case GainParams::kParamGainId:
						printf("somthing changed...\n");
						if(paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue){
							masterGainDial = value;
						}
						break;
				}
			}
		}
	}
	
	//--- Here you have to implement your processing

	if(data.numInputs == 0 || data.numSamples == 0){
		return kResultOk;
	}

	int32 numChannels = data.inputs[0].numChannels;

	//
	uint32 sampleFrameSize = getSampleFramesSizeInBytes(processSetup, data.numSamples);
	void** inBuf = getChannelBuffersPointer(processSetup, data.inputs[0]);
	void** outBuf = getChannelBuffersPointer(processSetup, data.outputs[0]);

	// am i mutted
	data.outputs[0].silenceFlags = 0;

	float gain = masterGainDial;
	for(int32 i = 0; i < numChannels; i++){
		int32 samples = data.numSamples;
		Vst::Sample32* ptrIn = (Vst::Sample32*)inBuf[i];
		Vst::Sample32* ptrOut = (Vst::Sample32*)outBuf[i];
		Vst::Sample32 tmp;
		// Modify the gain
		while(--samples >= 0){
			tmp = (*ptrIn) * gain;
			(*ptrOut++) = tmp;
		}
	}

	

	// 

	/*if (data.numSamples > 0)
	{
		//--- ------------------------------------------
		// here as example a default implementation where we try to copy the inputs to the outputs:
		// if less input than outputs then clear outputs
		//--- ------------------------------------------
		
		int32 minBus = std::min (data.numInputs, data.numOutputs);
		for (int32 i = 0; i < minBus; i++)
		{
			int32 minChan = std::min (data.inputs[i].numChannels, data.outputs[i].numChannels);
			for (int32 c = 0; c < minChan; c++)
			{
				// do not need to be copied if the buffers are the same
				if (data.outputs[i].channelBuffers32[c] != data.inputs[i].channelBuffers32[c])
				{
					memcpy (data.outputs[i].channelBuffers32[c], data.inputs[i].channelBuffers32[c],
							data.numSamples * sizeof (Vst::Sample32));
				}
			}
			data.outputs[i].silenceFlags = data.inputs[i].silenceFlags;
				
			// clear the remaining output buffers
			for (int32 c = minChan; c < data.outputs[i].numChannels; c++)
			{
				// clear output buffers
				memset (data.outputs[i].channelBuffers32[c], 0,
						data.numSamples * sizeof (Vst::Sample32));

				// inform the host that this channel is silent
				data.outputs[i].silenceFlags |= ((uint64)1 << c);
			}
		}
		// clear the remaining output buffers
		for (int32 i = minBus; i < data.numOutputs; i++)
		{
			// clear output buffers
			for (int32 c = 0; c < data.outputs[i].numChannels; c++)
			{
				memset (data.outputs[i].channelBuffers32[c], 0,
						data.numSamples * sizeof (Vst::Sample32));
			}
			// inform the host that this bus is silent
			data.outputs[i].silenceFlags = ((uint64)1 << data.outputs[i].numChannels) - 1;
		}
	}*/

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);

	// Gain
	float saveParam1 = 0.0f;
	if(streamer.readFloat(saveParam1) == false) return kResultFalse;
	masterGainDial = saveParam1;
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexProcessor::getState (IBStream* state)
{
	// here we need to save the model
	float saveParam1 = masterGainDial;
	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat(saveParam1);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace Electro-Corp
