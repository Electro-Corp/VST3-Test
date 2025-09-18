//------------------------------------------------------------------------
// Copyright(c) 2025 Electro-Corp.
//------------------------------------------------------------------------

#include "ECNex_controller.h"
#include "ECNex_cids.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;

namespace ElectroCorp {

//------------------------------------------------------------------------
// CECNexController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API CECNexController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Here you could register some parameters
	// i guess i could

	parameters.addParameter(STR16("Gain"), STR16("dB"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, GainParams::kParamGainId, 0);

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexController::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API CECNexController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API CECNexController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "ECNex_Main.uidesc");
		// return view
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
} // namespace Electro-Corp
