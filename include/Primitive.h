//
// Created by Nemesis Verstraete on 13/08/2026.
//

#pragma once

namespace renderium {

enum class PrimitiveTopology {
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip
};

enum class CullMode {
    None,
    Front,
    Back
};

enum class FrontFace {
    CW,
    CCW
};

struct PrimitiveState {
    PrimitiveTopology primitiveTopology = PrimitiveTopology::TriangleList;
    FrontFace frontFace = FrontFace::CCW;
    CullMode cullMode = CullMode::None;
};

}