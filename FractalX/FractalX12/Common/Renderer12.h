#pragma once
#include <memory>

class Renderer12
{
public:
	virtual ~Renderer12() {};

	virtual void Initialize(HWND window, int width, int height) = 0;

	virtual void OnWindowSizeChanged(int width, int height) = 0;

	virtual void Draw() = 0;

	virtual void MouseDown(WPARAM btnState, int x, int y) = 0;

	virtual void MouseUp(WPARAM btnState, int x, int y) = 0;

	virtual void MouseMove(WPARAM btnState, int x, int y) = 0;

	virtual void OnTimer(bool start) = 0;

protected:
	Renderer12() {}
};