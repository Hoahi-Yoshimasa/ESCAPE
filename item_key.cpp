//=============================================================================
//
// 鍵のモデル処理 [key.cpp]
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
#include "item_key.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_KEY		"data/MODEL/key.obj"		// 読み込むモデル名


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static KEY			g_Key[MAX_KEY];						// 鍵

static BOOL			g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitKey(void)
{
	for (int i = 0; i < MAX_KEY; i++)
	{
		LoadModel(MODEL_KEY, &g_Key[i].model);

		g_Key[i].load = TRUE;

		g_Key[i].pos = XMFLOAT3(0.0f, KEY_OFFSET_Y, -20.0f);
		g_Key[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Key[i].scl = XMFLOAT3(0.8f, 0.8f, 0.8f);
		g_Key[i].size = KEY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Key[0].model, &g_Key[0].diffuse[0]);

		XMFLOAT3 pos = g_Key[i].pos;
		pos.y -= (KEY_OFFSET_Y - 0.1f);
		g_Key[i].use = TRUE;			// TRUE:使っている

	}

	// 各鍵の色指定
	// 赤い鍵
	for (int j = 0; j < g_Key[RED_KEY].model.SubsetNum; j++)
	{
		SetModelDiffuse(&g_Key[RED_KEY].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.99f)); // α値が１ならシャドウを描画するようにしている為 0.99にしている
	}
	// 青い鍵
	for (int k = 0; k < g_Key[BLUE_KEY].model.SubsetNum; k++)
	{
		SetModelDiffuse(&g_Key[BLUE_KEY].model, k, XMFLOAT4(0.0f, 0.0f, 1.0f, 0.99f));
	}
	// 黄色い鍵
	for (int l = 0; l < g_Key[YELLOW_KEY].model.SubsetNum; l++)
	{
		SetModelDiffuse(&g_Key[YELLOW_KEY].model, l, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.99f));
	}

	// メインゲーム時の鍵のPOS指定
	if (GetMode() == MODE_GAME)
	{
		g_Key[RED_KEY].pos = XMFLOAT3(290.0f, KEY_OFFSET_Y, -240.0f);
		g_Key[RED_KEY].rot = XMFLOAT3(0.0f, -XM_PIDIV2, 0.0f);

		g_Key[BLUE_KEY].pos = XMFLOAT3(241.0f, KEY_OFFSET_Y, 420.0f);
		g_Key[BLUE_KEY].rot = XMFLOAT3(0.0f, -XM_PIDIV2, 0.0f);

		g_Key[YELLOW_KEY].pos = XMFLOAT3(-50.0f, KEY_OFFSET_Y, 240.0f);
		g_Key[YELLOW_KEY].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);

	}

	// チュートリアル時の鍵のPOS指定
	if (GetMode() == MODE_TUTORIAL)
	{
		g_Key[RED_KEY].pos = XMFLOAT3(-420.0f, KEY_OFFSET_Y, -420.0f);
		g_Key[RED_KEY].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

		g_Key[BLUE_KEY].pos = XMFLOAT3(420.0f, KEY_OFFSET_Y, -420.0f);
		g_Key[BLUE_KEY].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

		g_Key[YELLOW_KEY].pos = XMFLOAT3(420.0f, KEY_OFFSET_Y, 420.0f);
		g_Key[YELLOW_KEY].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitKey(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_KEY; i++)
	{
		if (g_Key[i].load)
		{
			UnloadModel(&g_Key[i].model);
			g_Key[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateKey(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawKey(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング有効
	SetCullingMode(CULL_MODE_BACK);

	// ワイヤーフレームスイッチ有効
	SelectWireFrameMode();

	for (int i = 0; i < MAX_KEY; i++)
	{
		if (g_Key[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Key[i].scl.x, g_Key[i].scl.y, g_Key[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Key[i].rot.x, g_Key[i].rot.y + XM_PI, g_Key[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Key[i].pos.x, g_Key[i].pos.y, g_Key[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Key[i].mtxWorld, mtxWorld);

		// フチを光らせている
		SetFuchi(1);

		// モデル描画
		DrawModel(&g_Key[i].model);

		SetFuchi(0);

	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	// ワイヤーフレーム設定を戻す
	SetWireFrameMode(WIRE_FRAME_MODE_NONE);
}



//=============================================================================
// ゲートの取得
//=============================================================================
KEY *GetKey()
{
	return &g_Key[0];
}

