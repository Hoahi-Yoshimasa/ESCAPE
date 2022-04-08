//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 星克昌
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_OP,			// OP BGM
	SOUND_LABEL_BGM_RESULT,		// RESULT BGM
	SOUND_LABEL_BGM_TUTORIAL,	// TUTORIAL BGM
	SOUND_LABEL_BGM_GAME,		// GAME BGM
	SOUND_LABEL_SE_Footsteps,	// 歩く足音
	SOUND_LABEL_SE_RUN,			// 走る足音
	SOUND_LABEL_SE_KEY_GET,		// 鍵取得音
	SOUND_LABEL_SE_OPEN_DOOR,	// 扉を開ける音
	SOUND_LABEL_SE_HIT,			// 殺られる音
	SOUND_LABEL_SE_HEARTBEAT,	// 心拍音
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

