#pragma once

class Render;

class DrawablePrimitive
{
public:
	DrawablePrimitive(Render* render) : render(render) {}
	virtual ~DrawablePrimitive() = default;

	virtual void Draw() = 0;

protected:
	Render* render;
};
