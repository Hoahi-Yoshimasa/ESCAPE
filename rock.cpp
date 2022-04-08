//=============================================================================
//
// 岩のモデル処理 [rock.cpp]
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
#include "rock.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ROCK			"data/MODEL/rock.obj"		// 読み込むモデル名


#define ROCK_SHADOW_SIZE	(0.4f)						// 影の大きさ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ROCK			g_Rock[MAX_ROCK];					// 岩

static BOOL			g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRock(void)
{
	for (int i = 0; i < MAX_ROCK; i++)
	{
		LoadModel(MODEL_ROCK, &g_Rock[i].model);
		g_Rock[i].load = TRUE;

		g_Rock[i].pos = XMFLOAT3(0.0f, ROCK_OFFSET_Y, 12.0f);
		g_Rock[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rock[i].scl = XMFLOAT3(0.9f, 0.7f, 0.5f);
		g_Rock[i].size = ROCK_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Rock[0].model, &g_Rock[0].diffuse[0]);

		XMFLOAT3 pos = g_Rock[i].pos;
		pos.y -= (ROCK_OFFSET_Y - 0.1f);
		g_Rock[i].shadowIdx = CreateShadow(pos, ROCK_SHADOW_SIZE, ROCK_SHADOW_SIZE);

		g_Rock[i].use = TRUE;			// TRUE:使っている

	}

	// 各岩ごとに個別で初期化する
	g_Rock[0].pos = XMFLOAT3(-400.0f, ROCK_OFFSET_Y, 175.0f);
	g_Rock[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Rock[0].scl = XMFLOAT3(1.0f, 0.7f, 0.5f);

	g_Rock[1].pos = XMFLOAT3(-175.0f, ROCK_OFFSET_Y, -405.0f);
	g_Rock[1].rot = XMFLOAT3(0.0f, XM_PI + XM_PIDIV2, 0.0f);
	g_Rock[1].scl = XMFLOAT3(1.0f, 0.7f, 0.5f);

	g_Rock[2].pos = XMFLOAT3(-175.0f, ROCK_OFFSET_Y, -53.0f);
	g_Rock[2].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Rock[2].scl = XMFLOAT3(0.8f, 0.7f, 0.5f);

	g_Rock[3].pos = XMFLOAT3(400.0f, ROCK_OFFSET_Y, 275.0f);
	g_Rock[3].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
	g_Rock[3].scl = XMFLOAT3(1.0f, 0.7f, 0.5f);


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitRock(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].load)
		{
			UnloadModel(&g_Rock[i].model);
			g_Rock[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateRock(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawRock(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング有効
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Rock[i].scl.x, g_Rock[i].scl.y, g_Rock[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Rock[i].rot.x, g_Rock[i].rot.y + XM_PI, g_Rock[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Rock[i].pos.x, g_Rock[i].pos.y, g_Rock[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Rock[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Rock[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}



//=============================================================================
// 岩の情報取得
//=============================================================================
ROCK *GetRock()
{
	return &g_Rock[0];
}

