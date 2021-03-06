/***********************************************************************************
**
** Target.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "Target.h"
#include "Game objects/GameWorld.h"
#include "Network/Packets.h"
#include "OrionUO.h"
#include "Managers/MapManager.h"
//----------------------------------------------------------------------------------
CTarget g_Target;
//----------------------------------------------------------------------------------
CTarget::CTarget()
: m_Type(0), m_Targeting(false), m_MultiGraphic(0), m_Multi(NULL), m_CursorID(0)
{
	//��������
	memset(m_Data, 0, sizeof(m_Data));
	memset(m_LastData, 0, sizeof(m_LastData));
}
//----------------------------------------------------------------------------------
void CTarget::Reset()
{
	//��������
	memset(m_Data, 0, sizeof(m_Data));
	memset(m_LastData, 0, sizeof(m_LastData));

	if (m_Multi != NULL)
	{
		delete m_Multi;
		m_Multi = NULL;
	}

	m_Type = 0;
	m_CursorType = 0;
	m_CursorID = 0;
	m_Targeting = false;
	m_MultiGraphic = 0;
}
//----------------------------------------------------------------------------------
void CTarget::SetData(WISP_DATASTREAM::CDataReader &reader)
{
	//�������� ������
	memcpy(&m_Data[0], reader.Start, reader.Size);

	//� ������������� ��������������� ��������
	m_Type = reader.ReadUInt8();
	m_CursorID = reader.ReadUInt32BE();
	m_CursorType = reader.ReadUInt8();
	m_Targeting = true;
	m_MultiGraphic = false;
}
//----------------------------------------------------------------------------------
void CTarget::SetMultiData(WISP_DATASTREAM::CDataReader &reader)
{
	//������������� ��������������� ��������
	m_Type = 1;
	m_CursorType = 0;
	m_Targeting = true;
	m_CursorID = reader.ReadUInt32BE(1);

	//�������� ������
	memset(&m_Data[0], 0, 19);
	m_Data[0] = 0x6C;
	m_Data[1] = 1; //������ �� ��������
	memcpy(m_Data + 2, reader.Start + 2, 4); //�������� ID ������� (ID ����)

	reader.ResetPtr();
	m_MultiGraphic = reader.ReadUInt16BE(18);
}
//----------------------------------------------------------------------------------
void CTarget::SendTargetObject(const uint &serial)
{
	if (!m_Targeting)
		return; //���� � ������� ��� ������� - �����

	//����� �������� �������, �� ������� ������ ��������, ��������� - ��������
	pack32(m_Data + 7, serial);
	m_Data[1] = 0;

	CGameObject *obj = (g_World != NULL ? g_World->FindWorldObject(serial) : NULL);

	if (obj != NULL)
	{
		pack16(m_Data + 11, obj->X);
		pack16(m_Data + 13, obj->Y);
		m_Data[15] = 0xFF;
		m_Data[16] = obj->Z;
		pack16(m_Data + 17, obj->Graphic);
	}
	else
	{
		pack32(m_Data + 11, 0);
		pack32(m_Data + 15, 0);
	}

	if (serial != g_PlayerSerial)
	{
		g_LastTargetObject = serial;

		//��������� ��� LastTarget
		memcpy(m_LastData, m_Data, sizeof(m_Data));

		if (serial < 0x40000000)
			CPacketStatusRequest(serial).Send();
	}

	SendTarget();
}
//----------------------------------------------------------------------------------
void CTarget::SendTargetTile(const ushort &tileID, const short &x, const short &y, char z)
{
	if (!m_Targeting)
		return; //���� � ������� ��� ������� - �����

	m_Data[1] = 1;

	//����� ���������� � ������ �����, �� ������� ������, ��������� ����
	pack32(m_Data + 7, 0);
	pack16(m_Data + 11, x);
	pack16(m_Data + 13, y);
	m_Data[15] = 0xFF;

	if (m_MultiGraphic != 0)
	{
		int grZ = 0;
		int stZ = 0;
		g_MapManager->GetMapZ(x, y, grZ, stZ);
		z = grZ;
	}

	m_Data[16] = z;
	pack16(m_Data + 17, tileID);

	//��������� ��� LastTarget
	memcpy(m_LastData, m_Data, sizeof(m_Data));

	SendTarget();
}
//----------------------------------------------------------------------------------
void CTarget::SendCancelTarget()
{
	if (!m_Targeting)
		return; //���� � ������� ��� ������� - �����

	//������ ������ ����
	pack32(m_Data + 7, 0);
	pack32(m_Data + 11, 0xFFFFFFFF);
	pack32(m_Data + 15, 0);

	SendTarget();
}
//----------------------------------------------------------------------------------
void CTarget::SendLastTarget()
{
	if (!m_Targeting)
		return; //���� � ������� ��� ������� - �����

	//����������� ����� ���������� ����������� �������
	memcpy(m_Data, m_LastData, sizeof(m_Data));
	m_Data[0] = 0x6C;
	m_Data[1] = m_Type;
	m_Data[6] = m_CursorType;
	pack32(m_Data + 2, m_CursorID);

	SendTarget();
}
//----------------------------------------------------------------------------------
void CTarget::SendTarget()
{
	g_Orion.Send(m_Data, sizeof(m_Data));

	//������ ������
	memset(m_Data, 0, sizeof(m_Data));
	m_Targeting = false;
	m_MultiGraphic = 0;
}
//----------------------------------------------------------------------------------
void CTarget::UnloadMulti()
{
	if (m_Multi != NULL)
	{
		delete m_Multi;
		m_Multi = NULL;
	}
}
//----------------------------------------------------------------------------------
void CTarget::LoadMulti(const int &x, const int &y, const char &z)
{
	CIndexMulti &index = g_Orion.m_MultiDataIndex[m_MultiGraphic];
	
	if (index.Address != NULL)
	{
		int count = (int)index.Count;

		IFOR(j, 0, count)
		{
			PMULTI_BLOCK pmb = (PMULTI_BLOCK)(index.Address + (j * sizeof(MULTI_BLOCK)));
			
			CMultiObject *mo = new CMultiObject(pmb->ID, x + pmb->X, y + pmb->Y, z + (char)pmb->Z, 2);
			g_MapManager->AddRender(mo);
			AddMultiObject(mo);
		}
	}
}
//----------------------------------------------------------------------------------
void CTarget::AddMultiObject(CMultiObject *obj)
{
	if (m_Multi == NULL)
	{
		m_Multi = new CMulti(obj->X, obj->Y);
		m_Multi->m_Next = NULL;
		m_Multi->m_Items = obj;
		obj->m_Next = NULL;
		obj->m_Prev = NULL;
	}
	else
	{
		CMulti *multi = GetMultiAtXY(obj->X, obj->Y);

		if (multi != NULL)
		{
			QFOR(multiobj, multi->m_Items, CMultiObject*)
			{
				if (obj->Z < multiobj->Z)
				{
					if (multiobj->m_Prev == NULL)
					{
						obj->m_Prev = NULL;
						obj->m_Next = multiobj;
						multiobj->m_Prev = obj;
						multi->m_Items = obj;
					}
					else
					{
						obj->m_Next = multiobj->m_Next;
						multiobj->m_Next = obj;
						obj->m_Prev = multiobj;
					}

					return;
				}

				if (multiobj->m_Next == NULL)
				{
					multiobj->m_Next = obj;
					obj->m_Prev = multiobj;
					obj->m_Next = NULL;

					return;
				}
			}

			//���� ������ ���� - ���-�� ����� �� ���
		}
		else
		{
			CMulti *newmulti = new CMulti(obj->X, obj->Y);
			newmulti->m_Next = NULL;
			newmulti->m_Items = obj;
			obj->m_Next = NULL;
			obj->m_Prev = NULL;

			multi = m_Multi;

			while (multi != NULL)
			{
				if (multi->m_Next == NULL)
				{
					multi->m_Next = newmulti;
					break;
				}

				multi = (CMulti*)multi->m_Next;
			}
		}
	}
}
//----------------------------------------------------------------------------------
CMulti *CTarget::GetMultiAtXY(const short &x, const short &y)
{
	CMulti *multi = m_Multi;

	while (multi != NULL)
	{
		if (multi->X == x && multi->Y == y)
			return multi;

		multi = (CMulti*)multi->m_Next;
	}

	return multi;
}
//----------------------------------------------------------------------------------
