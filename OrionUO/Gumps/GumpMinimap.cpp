/***********************************************************************************
**
** GumpMinimap.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "GumpMinimap.h"
#include "../OrionUO.h"
#include "../Game objects/GameWorld.h"
#include "../Game objects/GamePlayer.h"
#include "../Managers/MulReader.h"
#include "../Managers/MapManager.h"
#include "../Managers/ColorManager.h"
#include "../Managers/ConfigManager.h"
#include "../Managers/MouseManager.h"
//----------------------------------------------------------------------------------
CGumpMinimap::CGumpMinimap(uint serial, short x, short y, bool minimized)
: CGump(GT_MINIMAP, serial, x, y), m_Count(0), m_LastX(0), m_LastY(0), m_Body(NULL),
m_DataBox(NULL), m_Texture()
{
	m_Minimized = minimized;
	m_Locker.Serial = ID_GMM_LOCK_MOVING;
	GenerateMap();
}
//----------------------------------------------------------------------------------
CGumpMinimap::~CGumpMinimap()
{
	m_Texture.Clear();
}
//----------------------------------------------------------------------------------
void CGumpMinimap::CalculateGumpState()
{
	bool minimized = m_Minimized;
	m_Minimized = false;

	CGump::CalculateGumpState();

	m_Minimized = minimized;
}
//----------------------------------------------------------------------------------
void CGumpMinimap::GenerateMap()
{
	if (g_Player != NULL)
	{
		m_LastX = g_Player->X;
		m_LastY = g_Player->Y;
	}

	m_Texture.Clear();
	
	ushort gumpID = 0x1393 - (int)m_Minimized;
	CIndexObject &io = g_Orion.m_GumpDataIndex[gumpID];

	int gumpWidth = io.Width;
	int gumpHeight = io.Height;

	USHORT_LIST data = g_MulReader->GetGumpPixels(io);
	
	if (!data.size())
		return;

#if UO_ENABLE_TEXTURE_DATA_SAVING == 1
	m_Texture.PixelsData = data;
#endif

	int blockOffsetX = gumpWidth / 4;
	int blockOffsetY = gumpHeight / 4;

	int gumpCenterX = gumpWidth / 2;
	int gumpCenterY = gumpHeight / 2;

	//0xFF080808 - pixel32
	//0x8421 - pixel16

	int minBlockX = (m_LastX - blockOffsetX) / 8 - 1;
	int minBlockY = (m_LastY - blockOffsetY) / 8 - 1;
	int maxBlockX = ((m_LastX + blockOffsetX) / 8) + 1;
	int maxBlockY = ((m_LastY + blockOffsetY) / 8) + 1;

	if (minBlockX < 0)
		minBlockX = 0;

	if (minBlockY < 0)
		minBlockY = 0;

	int map = g_MapManager->GetActualMap();

	for (int i = minBlockX; i <= maxBlockX; i++)
	{
		for (int j = minBlockY; j <= maxBlockY; j++)
		{
			int blockIndex = (i * g_MapBlockSize[map].Height) + j;

			CMapBlock *mapBlock = g_MapManager->GetBlock(blockIndex);
			MAP_BLOCK mb = { 0 };

			if (mapBlock == NULL)
				g_MapManager->GetRadarMapBlock(i, j, mb);

			IFOR(x, 0, 8)
			{
				IFOR(y, 0, 8)
				{
					int px = (((i * 8) + x) - m_LastX) + gumpCenterX;
					int py = ((j * 8) + y) - m_LastY;

					int gx = px - py;
					int gy = px + py;

					if (gy < 0 || gy >= gumpHeight)
						continue;

					IFOR(i1, 0, 2)
					{
						gx += i1;

						if (gx < 0 || gx >= gumpWidth)
							continue;

						int block = gy * gumpWidth + gx;

						if (data[block] == 0x8421)
						{
							ushort color = (mapBlock != NULL ? mapBlock->GetRadarColor(x, y) : mb.Cells[(y * 8) + x].TileID);

							if (mapBlock == NULL)
							{
								if (color >= 0x4000)
									color = g_Orion.GetSeasonGraphic(color - 0x4000) + 0x4000;
								else
									color = g_Orion.GetLandSeasonGraphic(color);
							}

							data[block] = 0x8000 | g_ColorManager.GetRadarColorData(color);
						}
					}
				}
			}
		}
	}

	m_Texture.Width = gumpWidth;
	m_Texture.Height = gumpHeight;

	g_GL.BindTexture16(m_Texture.Texture, gumpWidth, gumpHeight, &data[0]);

	m_WantUpdateContent = true;
}
//----------------------------------------------------------------------------------
void CGumpMinimap::PrepareContent()
{
	if (g_Player->X != m_LastX || g_Player->Y != m_LastY || m_Texture.Texture == 0)
		GenerateMap();
	else if (!m_Count || m_Count == 6 || m_WantRedraw)
		m_WantUpdateContent = true;

	static uint ticks = 0;

	if (ticks < g_Ticks)
	{
		m_Count++;
		ticks = g_Ticks + 50;
	}

	if (m_Count > 12)
		m_Count = 0;
}
//----------------------------------------------------------------------------------
void CGumpMinimap::UpdateContent()
{
	ushort graphic = 0x1393 - (int)m_Minimized;

	CGLTexture *th = g_Orion.ExecuteGump(graphic);

	if (th == NULL)
		return;

	if (m_Items == NULL)
	{
		m_DataBox = (CGUIDataBox*)Add(new CGUIDataBox());
		m_Body = (CGUIGumppic*)Add(new CGUIGumppic(graphic, 0, 0));
		m_Body->SelectOnly = true;
	}
	else
		m_DataBox->Clear();

	m_Body->Graphic = graphic;

	int playerX = g_Player->X;
	int playerY = g_Player->Y;

	int gumpWidth = th->Width;
	int gumpHeight = th->Height;

	int gumpCenterX = (gumpWidth / 2) - 1;
	int gumpCenterY = (gumpHeight / 2) - 1;

	m_DataBox->Add(new CGUIExternalTexture(&m_Texture, false, 0, 0));

	if (m_Count < 6)
	{
		QFOR(go, g_World->m_Items, CGameObject*)
		{
			if (go->Container != 0xFFFFFFFF || go->Graphic >= 0x4000)
				continue; //multi

			if (go->NPC && !go->IsPlayer())
			{
				ushort color = g_ConfigManager.GetColorByNotoriety(go->GameCharacterPtr()->Notoriety);

				if (color)
				{
					uint pcl = g_ColorManager.GetPolygoneColor(16, color);

					int x = go->X - playerX;
					int y = go->Y - playerY;

					int gx = x - y;
					int gy = x + y;

					m_DataBox->Add(new CGUIColoredPolygone(0, 0, gumpCenterX + gx, gumpCenterY + gy, 2, 2, pcl));
				}
			}
		}

		m_DataBox->Add(new CGUIColoredPolygone(0, 0, gumpCenterX, gumpCenterY, 2, 2, 0xFFFFFFFF));
	}
}
//----------------------------------------------------------------------------------
void CGumpMinimap::GUMP_BUTTON_EVENT_C
{
	if (serial == ID_GMM_LOCK_MOVING)
		m_LockMoving = !m_LockMoving;
}
//----------------------------------------------------------------------------------
