/***********************************************************************************
**
** DefinitionMacro.h
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef DEFINITIONMACRO_H
#define DEFINITIONMACRO_H
//----------------------------------------------------------------------------------
#define UO_RENDER_LIST_SORT 1
#define UO_CHECKERBOARD_SEQUENCE_RENDER_LIST 1
//----------------------------------------------------------------------------------
#define UO_USE_SHADER_FILES 0
//----------------------------------------------------------------------------------
#define UO_DEBUG_INFO 1
//----------------------------------------------------------------------------------
//!������� ������
#define UOFONT_SOLID			0x01 //!������
#define UOFONT_ITALIC			0x02 //!���������
#define UOFONT_INDENTION		0x04 //!������ ������ (����� 1 ������) ��� �������
#define UOFONT_BLACK_BORDER		0x08 //!������ �������
#define UOFONT_UNDERLINE		0x10 //!������������
#define UOFONT_FIXED			0x20 //!������������� �����
#define UOFONT_CROPPED			0x40 //!������������� ����� � �����������
#define UOFONT_BQ				0x80 //!������ ��� ���� BQ
//----------------------------------------------------------------------------------
#define PLUGIN_FLAGS_PARSE_RECV			0x01
#define PLUGIN_FLAGS_PARSE_SEND			0x02
#define PLUGIN_FLAGS_RECV				0x04
#define PLUGIN_FLAGS_SEND				0x08
#define PLUGIN_FLAGS_WINDOW_PROC		0x10
#define PLUGIN_FLAGS_CLIENT_ACCESS		0x20
#define PLUGIN_FLAGS_GAME_WORLD_DRAW	0x40
#define PLUGIN_FLAGS_SCENE_DRAW			0x80
//----------------------------------------------------------------------------------
#define RANDOM_FIDGET_ANIMATION_DELAY		(30000 + (rand() % 5000))
//----------------------------------------------------------------------------------
#define MODKEY_ALT					0x0100
#define MODKEY_CTRL					0x0200
#define MODKEY_SHIFT				0x0400
//----------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------
