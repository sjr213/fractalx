#include "pch.h"
#include "Renderer12LandAndWaves.h"

#include "Core12LandAndWaves.h"

class Renderer12LandAndWavesImpl : public Renderer12
{
private:
	std::shared_ptr<Core12LandAndWaves> m_core;

public:
	Renderer12LandAndWavesImpl()
		: m_core(std::make_shared<Core12LandAndWaves>())
	{}

	virtual ~Renderer12LandAndWavesImpl()
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

std::shared_ptr<Renderer12> CreateRenderer12LandAndWaves()
{
	return std::make_shared<Renderer12LandAndWavesImpl>();
}