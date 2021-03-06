/***********************************************************************************
**
** GUIPolygonal.h
**
** ���������� ��� ���������
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef GUIPOLYGONAL_H
#define GUIPOLYGONAL_H
//----------------------------------------------------------------------------------
#include "BaseGUI.h"
//----------------------------------------------------------------------------------
class CGUIPolygonal : public CBaseGUI
{
	//!������
	SETGET(int, Width);

	//!������
	SETGET(int, Height);

	//!����� ������� �� ������� ����� ������ ���� ��� �� ����������
	SETGET(bool, CallOnMouseUp);

public:
	CGUIPolygonal(const GUMP_OBJECT_TYPE &type, const int &x, const int &y, const int &width, const int &height, const bool &callOnMouseUp = false);
	virtual ~CGUIPolygonal();

	virtual bool Select();
};
//----------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------
