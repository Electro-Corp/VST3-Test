//------------------------------------------------------------------------
// Copyright(c) 2025 Electro-Corp.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace ElectroCorp {
//------------------------------------------------------------------------
static const Steinberg::FUID kCECNexProcessorUID (0x8D052393, 0x69E4512F, 0xA503B05B, 0xFF8F50D3);
static const Steinberg::FUID kCECNexControllerUID (0x7330623A, 0x56305D91, 0x8E210C6B, 0xF78CA6EA);

// Gain
enum GainParams : Steinberg::Vst::ParamID{
    kParamGainId = 102,
};

#define CECNexVST3Category "ECCat"

//------------------------------------------------------------------------
} // namespace Electro-Corp
