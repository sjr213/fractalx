#include "pch.h"
#include "Renderer12Box.h"

#include "Core12Box.h"

class Renderer12BoxImpl : public Renderer12
{
private:
	std::shared_ptr<Core12Box> m_core;

public:
	Renderer12BoxImpl()
		: m_core(std::make_shared<Core12Box>())
	{}

	virtual ~Renderer12BoxImpl()
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
};

std::shared_ptr<Renderer12> CreateRenderer12Box()
{
	return std::make_shared<Renderer12BoxImpl>();
}