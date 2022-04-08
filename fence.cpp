//=============================================================================
//
// フェンスのモデル処理 [fence.cpp]
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
#include "fence.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_FENCE			"data/MODEL/fence.obj"		// 読み込むモデル名


#define FENCE_SHADOW_SIZE	(0.4f)						// 影の大きさ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static FENCE			g_Fence[MAX_FENCE];				// フェンス

static BOOL				g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFence(void)
{
	for (int i = 0; i < MAX_FENCE; i++)
	{
		LoadModel(MODEL_FENCE, &g_Fence[i].model);
		g_Fence[i].load = TRUE;

		g_Fence[i].pos = XMFLOAT3(0.0f, FENCE_OFFSET_Y, 12.0f);
		g_Fence[i].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
		g_Fence[i].scl = XMFLOAT3(1.0f, 0.9f, 1.0f);
		g_Fence[i].size = FENSE_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Fence[0].model, &g_Fence[0].diffuse[0]);

		XMFLOAT3 pos = g_Fence[i].pos;
		pos.y -= (FENCE_OFFSET_Y - 0.1f);
		g_Fence[i].shadowIdx = CreateShadow(pos, FENCE_SHADOW_SIZE, FENCE_SHADOW_SIZE);

		g_Fence[i].use = TRUE;			// TRUE:使っている

	}

	// フェンスの数分個別で初期化する

	g_Fence[0].pos = XMFLOAT3(50.0f, FENCE_OFFSET_Y, -360.0f);
	g_Fence[0].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[1].pos = XMFLOAT3(10.0f, FENCE_OFFSET_Y, -400.0f);
	g_Fence[1].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[2].pos = XMFLOAT3(90.0f, FENCE_OFFSET_Y, -400.0f);
	g_Fence[2].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);


	g_Fence[3].pos = XMFLOAT3(-300.0f, FENCE_OFFSET_Y, -290.0f);
	g_Fence[3].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[4].pos = XMFLOAT3(-300.0f, FENCE_OFFSET_Y, -210.0f);
	g_Fence[4].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[5].pos = XMFLOAT3(-340.0f, FENCE_OFFSET_Y, -250.0f);
	g_Fence[5].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[6].pos = XMFLOAT3(-260.0f, FENCE_OFFSET_Y, -250.0f);
	g_Fence[6].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);


	g_Fence[7].pos = XMFLOAT3(50.0f, FENCE_OFFSET_Y, -140.0f);
	g_Fence[7].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[8].pos = XMFLOAT3(50.0f, FENCE_OFFSET_Y, -60.0f);
	g_Fence[8].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[9].pos = XMFLOAT3(90.0f, FENCE_OFFSET_Y, -100.0f);
	g_Fence[9].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[10].pos = XMFLOAT3(10.0f, FENCE_OFFSET_Y, -100.0f);
	g_Fence[10].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);


	g_Fence[11].pos = XMFLOAT3(250.0f, FENCE_OFFSET_Y, 10.0f);
	g_Fence[11].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[12].pos = XMFLOAT3(250.0f, FENCE_OFFSET_Y, 90.0f);
	g_Fence[12].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[13].pos = XMFLOAT3(210.0f, FENCE_OFFSET_Y, 50.0f);
	g_Fence[13].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[14].pos = XMFLOAT3(290.0f, FENCE_OFFSET_Y, 50.0f);
	g_Fence[14].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);


	g_Fence[15].pos = XMFLOAT3(-300.0f, FENCE_OFFSET_Y, 260.0f);
	g_Fence[15].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[16].pos = XMFLOAT3(-300.0f, FENCE_OFFSET_Y, 340.0f);
	g_Fence[16].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[17].pos = XMFLOAT3(-260.0f, FENCE_OFFSET_Y, 300.0f);
	g_Fence[17].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[18].pos = XMFLOAT3(-340.0f, FENCE_OFFSET_Y, 300.0f);
	g_Fence[18].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);


	g_Fence[19].pos = XMFLOAT3(200.0f, FENCE_OFFSET_Y, 260.0f);
	g_Fence[19].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Fence[20].pos = XMFLOAT3(160.0f, FENCE_OFFSET_Y, 305.0f);
	g_Fence[20].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[21].pos = XMFLOAT3(160.0f, FENCE_OFFSET_Y, 395.0f);
	g_Fence[21].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[22].pos = XMFLOAT3(240.0f, FENCE_OFFSET_Y, 305.0f);
	g_Fence[22].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[23].pos = XMFLOAT3(240.0f, FENCE_OFFSET_Y, 395.0f);
	g_Fence[23].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Fence[24].pos = XMFLOAT3(200.0f, FENCE_OFFSET_Y, 440.0f);
	g_Fence[24].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFence(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_FENCE; i++)
	{
		if (g_Fence[i].load)
		{
			UnloadModel(&g_Fence[i].model);
			g_Fence[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFence(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFence(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング有効
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_FENCE; i++)
	{
		if (g_Fence[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Fence[i].scl.x, g_Fence[i].scl.y, g_Fence[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Fence[i].rot.x, g_Fence[i].rot.y + XM_PI, g_Fence[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Fence[i].pos.x, g_Fence[i].pos.y, g_Fence[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Fence[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Fence[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}



//=============================================================================
// フェンスの取得
//=============================================================================
FENCE *GetFence()
{
	return &g_Fence[0];
}

