#include "stdafx.h"
#include "Messages.h"


const UINT cMessage::tm_finish = RegisterWindowMessage(_T("Finish calculation"));

const UINT cMessage::tm_updateprogress = RegisterWindowMessage(_T("Update progress"));

const UINT cMessage::tm_render = RegisterWindowMessage(_T("Render 3D if needed"));

const UINT cMessage::tm_killPaletteView = RegisterWindowMessage(_T("Kill palette view"));

const UINT cMessage::tm_pinsChanged = RegisterWindowMessage(_T("Pins changed"));

const UINT cMessage::tm_pinEditDlgClosed = RegisterWindowMessage(_T("Pin edit dlg closed"));

const UINT cMessage::tm_killPositionAngleDlg = RegisterWindowMessage(_T("Kill position angle dialog"));

const UINT cMessage::tm_mouseCoords = RegisterWindowMessage(_T("Mouse coordinates"));

const UINT cMessage::tm_clickIDs = RegisterWindowMessage(_T("Click IDs"));

const UINT cMessage::tm_modelPositionChanged = RegisterWindowMessage(_T("Model Position Changed"));

const UINT cMessage::tm_modelAngleChanged = RegisterWindowMessage(_T("Model Angle Changed"));

const double PROGRESS_SIZE = 10000;

WPARAM cMessage::MakeWPARAMProgress(double progress)
{
	return static_cast<WPARAM>(PROGRESS_SIZE * progress + 0.5);
}

double cMessage::MakeDoubleProgress(WPARAM progress)
{
	return static_cast<double>(progress) / PROGRESS_SIZE;
}