#include "pch.h"
#include "Renderer12Stencil.h"

#include "Core12Stencil.h"

class Renderer12StencilImpl : public Renderer12
{
private:
	std::shared_ptr<Core12Stencil> m_core;

public:
	Renderer12StencilImpl()
		: m_core(std::make_shared<Core12Stencil>())
	{}

	virtual ~Renderer12StencilImpl()
	{}

	void Initialize(HWND window, int width, int height) override
	{
		m_core->Initialize(window, width, height);
	}

	void OnWindowSizeChanged(int width, int height) override
	{
		if (m_core->MainWnd() != nullptr)
			m_core->Resize(width, height);
	}

	void Draw() override
	{
		if (m_core->MainWnd() != nullptr)
		{
			m_core->Update();
			m_core->Draw();
		}
	}

	void MouseDown(WPARAM btnState, int x, int y) override
	{
		if (m_core->MainWnd() != nullptr)
			m_core->OnMouseDown(btnState, x, y);
	}

	void MouseUp(WPARAM btnState, int x, int y) override
	{
		if (m_core->MainWnd() != nullptr)
			m_core->OnMouseUp(btnState, x, y);
	}

	void MouseMove(WPARAM btnState, int x, int y) override
	{
		if (m_core->MainWnd() != nullptr)
		{
			m_core->OnMouseMove(btnState, x, y);
		}
	}

	void OnTimer(bool start) override
	{
		m_core->OnTimer(start);
	}
};

std::shared_ptr<Renderer12> CreateRenderer12Stencil()
{
	return std::make_shared<Renderer12StencilImpl>();
}