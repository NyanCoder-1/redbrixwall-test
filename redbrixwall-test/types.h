#pragma once

#include <cstdint>
#include <stdexcept>

enum class ePixelFormat : uint8_t
{
    bits = 0,
    r = 1,
    rg = 2,
    rgb = 3,
    rgba = 4
};

struct TVertexColored
{
	union
	{
		float pos[3];
		struct
		{
			float x, y, z;
		};
	};
	union
	{
		float color[2];
		struct
		{
			float r, g, b, a;
		};
	};
	TVertexColored() = default;
	TVertexColored(float x, float y, float z, float r, float g, float b, float a) :
		x(x), y(y), z(z), r(r), g(g), b(b), a(a) {}
};
struct TVertexTextured
{
	union
	{
		float pos[3];
		struct
		{
			float x, y, z;
		};
	};
	union
	{
		float uv[2];
		struct
		{
			float u, v;
		};
	};
	TVertexTextured() = default;
	TVertexTextured(float x, float y, float z, float u, float v) :
		x(x), y(y), z(z), u(u), v(v) {}
};

namespace Exception
{
    class InvalidUTF8Sequence : public std::runtime_error
    {
    public:
        InvalidUTF8Sequence() : runtime_error("Invalid UTF-8 sequence") {}
    };
}