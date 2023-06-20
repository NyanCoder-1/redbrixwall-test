#include "Button.h"
#include "Mesh.h"
#include "Text.h"
#include <vector>
#include <array>


namespace {
	std::array<float, 4> colorDefault = { 1.0f, 1.0f, 1.0f, 0.04f };
	std::array<float, 4> colorActive  = { 1.0f, 1.0f, 1.0f, 0.4f };
	std::array<float, 4> colorHover   = { 1.0f, 1.0f, 1.0f, 0.2f };
	std::array<float, 2> calculateCenter(float x, float y, float width, float height)
	{
		return { x + width / 2, y + height / 2 };
	}
};

Button::Button(Render* render) :
	DrawablePrimitive(render)
{
	buttonText = std::make_unique<Text>(render, U"");
	bg = std::make_unique<MeshColored>(render, std::vector<TVertexColored>{}, std::vector<uint32_t>{});
	bgHover = std::make_unique<MeshColored>(render, std::vector<TVertexColored>{}, std::vector<uint32_t>{});
	bgActive = std::make_unique<MeshColored>(render, std::vector<TVertexColored>{}, std::vector<uint32_t>{});
}

void Button::SetText(std::u32string text)
{
	if (buttonText)
	{
		buttonText->SetText(text);
		auto textCenter = calculateCenter(buttonText->GetMinX(), buttonText->GetMinY(), buttonText->GetWidth(), buttonText->GetHeight());
		auto buttonCenter = calculateCenter(x, y, width, height);
		buttonText->SetPosition(buttonCenter[0] - textCenter[0], buttonCenter[1] - textCenter[1]);
	}
}
void Button::SetPosition(int32_t x, int32_t y)
{
	this->x = x;
	this->y = y;
	auto mat = DirectX::XMMatrixTranslation(x, y, 0);
	for (auto& mesh : { bg.get(), bgHover.get(), bgActive.get() })
		if (mesh) mesh->UpdateMatrixFor2D(mat);
	if (buttonText)
	{
		auto textCenter = calculateCenter(buttonText->GetMinX(), buttonText->GetMinY(), buttonText->GetWidth(), buttonText->GetHeight());
		auto buttonCenter = calculateCenter(x, y, width, height);
		buttonText->SetPosition(buttonCenter[0] - textCenter[0], buttonCenter[1] - textCenter[1]);
	}
}
void Button::SetSize(int32_t width, int32_t height)
{
	this->width = width;
	this->height = height;
	std::vector<TVertexColored> vertices = {
		TVertexColored{ 0.0f,         0.0f,          0.0f, colorDefault[0], colorDefault[1], colorDefault[2], colorDefault[3] },
		TVertexColored{ (float)width, 0.0f,          0.0f, colorDefault[0], colorDefault[1], colorDefault[2], colorDefault[3] },
		TVertexColored{ 0.0f,         (float)height, 0.0f, colorDefault[0], colorDefault[1], colorDefault[2], colorDefault[3] },
		TVertexColored{ (float)width, (float)height, 0.0f, colorDefault[0], colorDefault[1], colorDefault[2], colorDefault[3] }
	};
	std::vector<uint32_t> indices = { 0, 1, 2, 2, 1, 3 };
	auto setColor = [&](std::array<float, 4> color) {
		for (auto& vertex : vertices)
		{
			for (int i = 0; i < 4; i++)
				vertex.color[i] = color[i];
		}
	};
	if (bg) bg->UpdateGeometry(vertices, indices);
	setColor(colorHover);
	if (bgHover) bgHover->UpdateGeometry(vertices, indices);
	setColor(colorActive);
	if (bgActive) bgActive->UpdateGeometry(vertices, indices);
	if (buttonText)
	{
		auto textCenter = calculateCenter(buttonText->GetMinX(), buttonText->GetMinY(), buttonText->GetWidth(), buttonText->GetHeight());
		auto buttonCenter = calculateCenter(x, y, width, height);
		buttonText->SetPosition(buttonCenter[0] - textCenter[0], buttonCenter[1] - textCenter[1]);
	}
}
void Button::MouseMove(int32_t x, int32_t y)
{
	if (buttonState != eState::Active)
	{
		if ((x >= this->x) && (x < (this->x + width)) && (y >= this->y) && (y < (this->y + height)))
			buttonState = eState::Hover;
		else
			buttonState = eState::Default;
	}
}

void Button::MouseClick(int32_t x, int32_t y)
{
	if (onClickCallback && (x >= this->x) && (x < (this->x + width)) && (y >= this->y) && (y < (this->y + height)))
	{
		onClickCallback();
	}
}

void Button::SetOnClick(std::function<void()> callback)
{
	onClickCallback = callback;
}

void Button::SetActive()
{
	buttonState = eState::Active;
}

void Button::SetInactive()
{
	if (buttonState == eState::Active)
		buttonState = eState::Default;
}

void Button::Draw()
{
	switch (buttonState)
	{
	case eState::Default:
		if (bg) bg->Draw();
		break;
	case eState::Hover:
		if (bgHover) bgHover->Draw();
		break;
	case eState::Active:
		if (bgActive) bgActive->Draw();
		break;
	}
	if (buttonText) buttonText->Draw();
}
