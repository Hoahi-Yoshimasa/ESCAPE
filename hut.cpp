//=============================================================================
//
// 小屋のモデル処理 [hut.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"
#include "debugproc.h"
#include "collision.h"
#include "coord_point.h"
#include "hut.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_HUT			"data/MODEL/hut.obj"		// 読み込むモデル名


#define HUT_SHADOW_SIZE	(0.4f)							// 影の大きさ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static HUT				g_Hut[MAX_HUT];					// 小屋
static BOOL				g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHut(void)
{
	for (int i = 0; i < MAX_HUT; i++)
	{
		LoadModel(MODEL_HUT, &g_Hut[i].model);
		g_Hut[i].load = TRUE;

		g_Hut[i].pos = XMFLOAT3(0.0f, HUT_OFFSET_Y, 12.0f);
		g_Hut[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Hut[i].scl = XMFLOAT3(0.9f, 1.2f, 0.8f);
		g_Hut[i].size = HUT_SIZE;		// 当たり判定の大きさ

		g_Hut[i].use = TRUE;			// TRUE:使っている

	}

	// 各小屋ごとに個別で初期化する
	g_Hut[0].pos = XMFLOAT3(-40.0f, HUT_OFFSET_Y, 195.0f);
	g_Hut[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Hut[1].pos = XMFLOAT3(245.0f, HUT_OFFSET_Y, -250.0f);
	g_Hut[1].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Hut[2].pos = XMFLOAT3(-355.0f, HUT_OFFSET_Y, -5.0f);
	g_Hut[2].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitHut(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_HUT; i++)
	{
		if (g_Hut[i].load)
		{
			UnloadModel(&g_Hut[i].model);
			g_Hut[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateHut(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawHut(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング有効
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_HUT; i++)
	{
		if (g_Hut[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Hut[i].scl.x, g_Hut[i].scl.y, g_Hut[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Hut[i].rot.x, g_Hut[i].rot.y + XM_PI, g_Hut[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Hut[i].pos.x, g_Hut[i].pos.y, g_Hut[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Hut[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Hut[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}



//=============================================================================
// 小屋の情報取得
//=============================================================================
HUT *GetHut()
{
	return &g_Hut[0];
}

