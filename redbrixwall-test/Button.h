#pragma once

#include "DrawablePrimitive.h"
#include <memory>
#include <cstdint>
#include <functional>
#include <string>

class Text;
class MeshColored;

class Button : public DrawablePrimitive
{
public:
	enum class eState {
		Default,
		Hover,
		Active
	};

public:
	Button(Render* render);
	void SetText(std::u32string text);
	void SetPosition(int32_t x, int32_t y);
	void SetSize(int32_t width, int32_t height);
	void MouseMove(int32_t x, int32_t y);
	void MouseClick(int32_t x, int32_t y);
	void SetOnClick(std::function<void()> callback);
	void SetActive();
	void SetInactive();

	void Draw() override;

private:
	std::unique_ptr<Text> buttonText;
	std::unique_ptr<MeshColored> bg;
	std::unique_ptr<MeshColored> bgHover;
	std::unique_ptr<MeshColored> bgActive;
	eState buttonState;

	int32_t x;
	int32_t y;
	uint32_t width;
	uint32_t height;

	std::function<void()> onClickCallback;
};
