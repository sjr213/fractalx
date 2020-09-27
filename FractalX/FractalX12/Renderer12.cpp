#include "pch.h"
#include "Renderer12.h"

class Renderer12Impl : public Renderer12
{
private:
	HWND m_window;
	int m_outputWidth;
	int m_outputHeight;

public:
	Renderer12Impl()
	{}

	virtual ~Renderer12Impl()
	{}

	void Initialize(HWND window, int width, int height) override
	{
		m_window = window;
		m_outputWidth = max(width, 1);
		m_outputHeight = max(height, 1);
/*
			try
			{
				CreateDevice();
				CreateResources();
				CreateBuffers();
				CreateBuffers2();
				m_ready = true;
			}
			catch (DxException& ex)
			{
				m_ready = false;
				CString msg(_T("Initialization failed! : "));
				msg += ex.what();
				AfxMessageBox(msg, MB_OK);
			}
		}
*/
	}

	void OnWindowSizeChanged(int width, int height) override
	{
		m_outputWidth = max(width, 1);
		m_outputHeight = max(height, 1);
	}
};

std::shared_ptr<Renderer12> Renderer12::CreateRenderer12()
{
	return std::make_shared<Renderer12Impl>();
}