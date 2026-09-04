//
// Created by Nemesis Verstraete on 06/08/2026.
//

#pragma once

namespace renderium {

enum class SamplerFilter {
    Point,
    Linear,
    Anisotropic
};

enum class SamplerWrap {
    Repeat,
    Clamp,
    Mirror,
    Border
};

struct SamplerState {
    SamplerFilter samplerFilter = SamplerFilter::Anisotropic;
    SamplerWrap samplerWrap = SamplerWrap::Repeat;
    bool operator==(const SamplerState& other) const {
        return other.samplerFilter == samplerFilter && other.samplerWrap == samplerWrap;
    }
};

class Sampler {
public:
    SamplerState getSamplerState() const { return samplerState; }
protected:
    SamplerState samplerState;
};

}
