#pragma once
#include <memory>

class Renderer12
{
public:
	static std::shared_ptr<Renderer12> CreateRenderer12();

	virtual ~Renderer12() {};

	virtual void Initialize(HWND window, int width, int height) = 0;

	virtual void OnWindowSizeChanged(int width, int height) = 0;

protected:
	Renderer12() {}
};