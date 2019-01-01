#pragma once

#include <minwindef.h>

class cMessage
{
public:
	static const WPARAM CalcFinished = 0;
	static const WPARAM CalcCanceled = 1;

	static const WPARAM DlgOK = 0;
	static const WPARAM DlgCancel = 1;

	static const UINT tm_finish;

	static const UINT tm_updateprogress;

	static const UINT tm_render;

	static const UINT tm_killPaletteView;

	static const UINT tm_pinsChanged;

	static const UINT tm_pinEditDlgClosed;

	static const UINT tm_killPositionAngleDlg;

	static const UINT tm_mouseCoords;

	static const UINT tm_clickIDs;

	static const UINT tm_modelPositionChanged;

	static const UINT tm_modelAngleChanged;

	static WPARAM MakeWPARAMProgress(double progress);

	static double MakeDoubleProgress(WPARAM progress);

};