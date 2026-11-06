//
// Created by Nemesis Verstraete on 03/11/2026.
//

#pragma once
#include <memory>
#include <vector>

namespace rhi {
template<typename Api>
class RenderPassImpl;
}

namespace renderium {
class TextureView;

class Color {
public:
    constexpr Color(const double r, const double g, const double b, const double a = 1.0) : r(r), g(g), b(b), a(a) {}

    static const Color Transparent;
    static const Color White;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;

    const double r;
    const double g;
    const double b;
    const double a;
};

constexpr Color Color::Transparent = Color(0.0, 0.0, 0.0, 0.0);
constexpr Color Color::White = Color(1.0, 1.0, 1.0, 1.0);
constexpr Color Color::Black = Color(0.0, 0.0, 0.0, 1.0);
constexpr Color Color::Red = Color(1.0, 0.0, 0.0, 1.0);
constexpr Color Color::Green = Color(0.0, 1.0, 0.0, 1.0);
constexpr Color Color::Blue = Color(0.0, 0.0, 1.0, 1.0);

enum class LoadOp {
    Load,
    Clear
};

enum class StoreOp {
    Store,
    Discard
};

struct RenderPassColorAttachment {
    TextureView& textureView;
    Color clearValue;
    LoadOp loadOp;
    StoreOp storeOp;
};

struct RenderPassDescriptor {
    std::vector<RenderPassColorAttachment> colorAttachments;
    uint64_t maxDrawCount = 50000000;
};

class RenderPass {
    struct Impl {
        virtual ~Impl() = default;
    };
    std::unique_ptr<Impl> impl;

    template<typename Api>
    friend class rhi::RenderPassImpl;
};

}