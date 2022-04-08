//=============================================================================
//
//  経路探索処理[pathfind.h]
// Author : 星克昌
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAP_RES_X	(20)	// チェックポイントの数X
#define MAP_RES_Y	(20)
#define MAP_RES		(MAP_RES_X * MAP_RES_Y)
//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct NODE
{
	int			prev;									// 一つ前の座標保存用
	int			cost;									// コスト
	BOOL		visited;								// 処理したフラグ用

	XMFLOAT4X4			mtxWorld;						// ワールドマトリックス
	XMFLOAT3			rot;							// モデルの向き(回転)
	XMFLOAT3			scl;							// モデルの大きさ(スケール)
	BOOL				load;
	DX11_MODEL			model;							// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPathfind(void);
void UninitPathfind(void);
void UpdatePathfind(void);
void DrawPathfind(void);
int ShortestPath(int start, int goal);
NODE *GetNode(void);