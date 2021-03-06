//----------------------------------------------------------------------------------
#include "Globals.h"
#include "Multi.h"
#include "Game objects/GameWorld.h"
//----------------------------------------------------------------------------------
bool g_AltPressed = false;
bool g_CtrlPressed = false;
bool g_ShiftPressed = false;
//----------------------------------------------------------------------------------
bool g_MovingFromMouse = false;
bool g_AutoMoving = false;
bool g_AbyssPacket03First = true;
//----------------------------------------------------------------------------------
int g_LandObjectsCount = 0;
int g_StaticsObjectsCount = 0;
int g_GameObjectsCount = 0;
int g_MultiObjectsCount = 0;
int g_RenderedObjectsCountInGameWindow = 0;
//----------------------------------------------------------------------------------
GLdouble g_GlobalScale = 1.0;

CGLTexture g_MapTexture[6];

int g_FrameDelay[2] = { FRAME_DELAY_UNACTIVE_WINDOW, FRAME_DELAY_ACTIVE_WINDOW };

uchar g_PingCount = 0;
uchar g_PingSequence = 0;

uint g_LastSendTime = 0;
uint g_LastPacketTime = 0;
uint g_TotalSendSize = 0;
uint g_TotalRecvSize = 0;

uint g_Ticks = 0;

GLuint ShaderColorTable = 0;
GLuint g_ShaderDrawMode = 0;

string g_Language = "ENU";

GAME_STATE g_GameState = GS_MAIN;

CGLTexture g_TextureGumpState[2];

WISP_GEOMETRY::CSize g_MapSize[6];
WISP_GEOMETRY::CSize g_MapBlockSize[6];

int g_MultiIndexCount = 0;

CGLFrameBuffer g_LightBuffer;

bool g_GumpPressed = false;
class CRenderObject *g_GumpSelectedElement = NULL;
class CRenderObject *g_GumpPressedElement = NULL;
WISP_GEOMETRY::CPoint2Di g_GumpMovingOffset;
WISP_GEOMETRY::CPoint2Df g_GumpTranslate;
bool g_ShowGumpLocker = false;

bool g_GrayedPixels = false;

bool g_ConfigLoaded = false;

uchar g_LightLevel = 0;
uchar g_PersonalLightLevel = 0;

char g_SelectedCharName[30] = { 0 };

//!������ ������� �����
uchar g_CurrentMap = 0;

//!����� �������
uchar g_ServerTimeHour = 0;
uchar g_ServerTimeMinute = 0;
uchar g_ServerTimeSecond = 0;

bool g_PacketLoginComplete = false;

uint g_ClientFlag = 0;

bool g_SendLogoutNotification = false;
bool g_NPCPopupEnabled = false;
bool g_ChatEnabled = false;

uchar g_GameSeed[4] = { 0 };

ushort g_OutOfRangeColor = 0;
char g_MaxGroundZ = 0;
bool g_NoDrawRoof = false;
char g_FoliageIndex = 0;
bool g_UseCircleTrans = false;

bool g_JournalShowSystem = true;
bool g_JournalShowObjects = true;
bool g_JournalShowClient = true;

uint g_PlayerSerial = 0;
uint g_StatusbarUnderMouse = 0;

int g_LastSpellIndex = 1;
int g_LastSkillIndex = 1;
uint g_LastUseObject = 0;
uint g_LastTargetObject = 0;
uint g_LastAttackObject = 0;

CHARACTER_SPEED_TYPE g_SpeedMode = CST_NORMAL;

uint g_DeathScreenTimer = 0;
uchar g_WalkRequestCount = 0;
uint g_PendingDelayTime = 0;

float g_AnimCharactersDelayValue = 80.0f; //0x50

CORPSE_LIST_MAP g_CorpseSerialList;

WISP_GEOMETRY::CPoint2Di g_RemoveRangeXY;

int g_GrayMenuCount = 0;

PROMPT_TYPE g_ConsolePrompt = PT_NONE;
uchar g_LastASCIIPrompt[11] = { 0 };
uchar g_LastUnicodePrompt[11] = { 0 };

uint g_PartyHelperTarget = 0;
uint g_PartyHelperTimer = 0;

float g_DrawColor = 1.0f;

float g_SkillsTotal = 0.0f;
SEASON_TYPE g_Season = ST_SUMMER;
SEASON_TYPE g_OldSeason = ST_SUMMER;
int g_OldSeasonMusic = 0;

uint g_LockedClientFeatures = 0;

bool g_GeneratedMouseDown = false;
bool g_DrawFoliage = true;

ushort g_ObjectHandlesBackgroundPixels[g_ObjectHandlesWidth * g_ObjectHandlesHeight] = { 0 };
//----------------------------------------------------------------------------------
void TileOffsetOnMonitorToXY(int &ofsX, int &ofsY, int &x, int &y)
{
	if (!ofsX)
		x = y = ofsY / 2;
	else if (!ofsY)
	{
		x = ofsX / 2;
		y = -x;
	}
	else //if (ofsX && ofsY)
	{
		int absX = abs(ofsX);
		int absY = abs(ofsY);
		x = ofsX;

		if (ofsY > ofsX)
		{
			if (ofsX < 0 && ofsY < 0)
				y = absX - absY;
			else if (ofsX > 0 && ofsY > 0)
				y = absY - absX;
		}
		else if (ofsX > ofsY)
		{
			if (ofsX < 0 && ofsY < 0)
				y = -(absY - absX);
			else if (ofsX > 0 && ofsY > 0)
				y = -(absX - absY);
		}

		if (!y && ofsY != ofsX)
		{
			if (ofsY < 0)
				y = -(absX + absY);
			else
				y = absX + absY;
		}

		y /= 2;
		x += y;
	}
}
//----------------------------------------------------------------------------------
int GetDistance(CGameObject *current, CGameObject *target)
{
	if (current != NULL && target != NULL)
	{
		int distx = abs(target->X - current->X);
		int disty = abs(target->Y - current->Y);

		if (disty > distx)
			distx = disty;

		return distx;
	}

	return 100500;
}
//----------------------------------------------------------------------------------
int GetDistance(CGameObject *current, WISP_GEOMETRY::CPoint2Di target)
{
	if (current != NULL)
	{
		int distx = abs(target.X - current->X);
		int disty = abs(target.Y - current->Y);

		if (disty > distx)
			distx = disty;

		return distx;
	}

	return 100500;
}
//----------------------------------------------------------------------------------
int GetDistance(WISP_GEOMETRY::CPoint2Di current, CGameObject *target)
{
	if (target != NULL)
	{
		int distx = abs(target->X - current.X);
		int disty = abs(target->Y - current.Y);

		if (disty > distx)
			distx = disty;

		return distx;
	}

	return 100500;
}
//----------------------------------------------------------------------------------
int GetMultiDistance(WISP_GEOMETRY::CPoint2Di current, CGameObject *target)
{
	int result = 100500;

	if (target != NULL && target->Graphic >= 0x4000 && target->m_Items != NULL)
	{
		CMulti *multi = (CMulti*)target->m_Items;

		int multiDistance = abs(multi->MinX);
		int testDistance = abs(multi->MinY);

		if (multiDistance < testDistance)
			multiDistance = testDistance;

		testDistance = abs(multi->MaxX);

		if (multiDistance < testDistance)
			multiDistance = testDistance;

		testDistance = abs(multi->MaxY);

		if (multiDistance < testDistance)
			multiDistance = testDistance;

		multiDistance--;

		int distX = abs(target->X - current.X) - multiDistance;
		int distY = abs(target->Y - current.Y) - multiDistance;

		if (distY > distX)
			distX = distY;

		if (distX < result)
			result = distX;
	}

	return result;
}
//----------------------------------------------------------------------------------
int GetDistance(WISP_GEOMETRY::CPoint2Di current, WISP_GEOMETRY::CPoint2Di target)
{
	int distx = abs(target.X - current.X);
	int disty = abs(target.Y - current.Y);

	if (disty > distx)
		distx = disty;

	return distx;
}
//----------------------------------------------------------------------------------
int GetTopObjDistance(CGameObject *current, CGameObject *target)
{
	if (current != NULL && target != NULL)
	{
		while (target != NULL && target->Container != 0xFFFFFFFF)
			target = g_World->FindWorldObject(target->Container);

		if (target != NULL)
		{
			int distx = abs(target->X - current->X);
			int disty = abs(target->Y - current->Y);

			if (disty > distx)
				distx = disty;

			return distx;
		}
	}

	return 100500;
}
//---------------------------------------------------------------------------
const char *GetReagentName(const ushort &id)
{
	switch (id)
	{
		case 0x0F7A:
			return "Black pearl";
		case 0x0F7B:
			return "Bloodmoss";
		case 0x0F84:
			return "Garlic";
		case 0x0F85:
			return "Ginseng";
		case 0x0F86:
			return "Mandrake root";
		case 0x0F88:
			return "Nightshade";
		case 0x0F8C:
			return "Sulfurous ash";
		case 0x0F8D:
			return "Spiders silk";
		default:
			break;
	}

	return "";
}
//----------------------------------------------------------------------------------
