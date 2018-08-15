#include "stdafx.h"
#include "Messages.h"


const UINT cMessage::tm_finish = RegisterWindowMessage(_T("Finish calculation"));

const UINT cMessage::tm_updateprogress = RegisterWindowMessage(_T("Update progress"));

const UINT cMessage::tm_render = RegisterWindowMessage(_T("Render 3D if needed"));

const UINT cMessage::tm_killPaletteView = RegisterWindowMessage(_T("Kill palette view"));

const double PROGRESS_SIZE = 10000;

WPARAM cMessage::MakeWPARAMProgress(double progress)
{
	return static_cast<WPARAM>(PROGRESS_SIZE * progress + 0.5);
}

double cMessage::MakeDoubleProgress(WPARAM progress)
{
	return static_cast<double>(progress) / PROGRESS_SIZE;
}