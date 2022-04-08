//=============================================================================
//
// 扉のモデル処理 [gate.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "debugproc.h"
#include "collision.h"
#include "gate.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_GATE			"data/MODEL/gate.obj"		// 読み込むモデル名


#define GATE_SHADOW_SIZE	(0.4f)						// 影の大きさ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static GATE			g_Gate[MAX_GATE];					// ゲート

static BOOL				g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGate(void)
{
	for (int i = 0; i < MAX_GATE; i++)
	{
		LoadModel(MODEL_GATE, &g_Gate[i].model);
		g_Gate[i].load = TRUE;

		g_Gate[i].pos = XMFLOAT3(-400.0f, GATE_OFFSET_Y, 470.0f);
		g_Gate[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Gate[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
		g_Gate[i].size = GATE_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Gate[0].model, &g_Gate[0].diffuse[0]);

		XMFLOAT3 pos = g_Gate[i].pos;
		pos.y -= (GATE_OFFSET_Y - 0.1f);
		g_Gate[i].shadowIdx = CreateShadow(pos, GATE_SHADOW_SIZE, GATE_SHADOW_SIZE);



		g_Gate[i].use = TRUE;			// TRUE:使っている

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGate(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_GATE; i++)
	{
		if (g_Gate[i].load)
		{
			UnloadModel(&g_Gate[i].model);
			g_Gate[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGate(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGate(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_GATE; i++)
	{
		if (g_Gate[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Gate[i].scl.x, g_Gate[i].scl.y, g_Gate[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Gate[i].rot.x, g_Gate[i].rot.y + XM_PI, g_Gate[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Gate[i].pos.x, g_Gate[i].pos.y, g_Gate[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Gate[i].mtxWorld, mtxWorld);

		DrawModel(&g_Gate[i].model);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}



//=============================================================================
// ゲートの取得
//=============================================================================
GATE *GetGate()
{
	return &g_Gate[0];
}

