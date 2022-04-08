//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : ������
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_OP,			// OP BGM
	SOUND_LABEL_BGM_RESULT,		// RESULT BGM
	SOUND_LABEL_BGM_TUTORIAL,	// TUTORIAL BGM
	SOUND_LABEL_BGM_GAME,		// GAME BGM
	SOUND_LABEL_SE_Footsteps,	// ��������
	SOUND_LABEL_SE_RUN,			// ���鑫��
	SOUND_LABEL_SE_KEY_GET,		// ���擾��
	SOUND_LABEL_SE_OPEN_DOOR,	// �����J���鉹
	SOUND_LABEL_SE_HIT,			// �E���鉹
	SOUND_LABEL_SE_HEARTBEAT,	// �S����
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

