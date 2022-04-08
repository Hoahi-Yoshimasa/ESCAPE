//=============================================================================
//
// UI処理 [ui.h]
// Author : 星克昌
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_UI			(22)	// UIの最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct UI
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	float		collected;		// アイテム(鍵)の取得状況
	BOOL		use;			// 使用しているかどうか

} ;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitUi(void);
void UninitUi(void);
void UpdateUi(void);
void DrawUi(void);

UI *GetUi(void);


