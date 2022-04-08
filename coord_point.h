//=============================================================================
//
// 座標ポイント処理 [Coord_Point.h]
// Author : 星克昌
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	KEEP_OUT				(-1)			// 当たり判定箇所

//*****************************************************************************
// 座標ポイント構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ワールドマトリックス
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 角度
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	BOOL		use;			// 使用しているかどうか
	int			move;			// KEEP_OUT(-1) なら動けない場所
} COORD_POINT;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitCoordPoint(void);
void UninitCoordPoint(void);
void UpdateCoordPoint(void);
void DrawCoordPoint(void);

XMFLOAT3 *GetCoord(void);

HRESULT MakeVertexCoordPoint(void);

int SetCoordPoint(XMFLOAT3 pos);
COORD_POINT *GetCoordPoint(void);

