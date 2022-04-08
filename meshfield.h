//=============================================================================
//
// メッシュ地面の処理 [meshfield.h]
// Author : 星克昌
//
//=============================================================================
#pragma once
#include "renderer.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define MESHFIELD_BlockX		(20)	// メッシュフィールドの横の数
#define MESHFIELD_BlockZ		(20)	// メッシュフィールドの縦の数

#define MESHFIELD_WIDTH			(50.0f)	// メッシュフィールドの横の大きさ
#define MESHFIELD_HEIGHT		(50.0f)	// メッシュフィールドの縦の大きさ

#define MAP_MESHFIELD_WIDTH			(MESHFIELD_BlockX * MESHFIELD_WIDTH)	// メッシュフィールド全体の横の大きさ
#define MAP_MESHFIELD_HEIGHT		(MESHFIELD_BlockZ * MESHFIELD_HEIGHT)	// メッシュフィールド全体の縦の大きさ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

XMFLOAT3 *GetCoord();

HRESULT MakeVertexPoint(void);

int *GetEnemy_chip_num();
int *GetPlayer_chip_num();
