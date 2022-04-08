//=============================================================================
//
// ライト処理 [light.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SHADOW_RANGE_WIDTH	(1000)			// ウインドウの幅
#define SHADOW_RANGE_HEIGHT	(1000)			// ウインドウの高さ



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;


//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 100.0f, 100.0f, 100.0f );
		g_Light[i].Direction = XMFLOAT3( 1.0f, 1.0f, 1.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	g_Light[0].Position = XMFLOAT3(0.0f, 200.0f, 0.0f);
	g_Light[0].Direction = XMFLOAT3(0.0f, -1.0f, 0.3f);		// ライトの角度  0.0 -1.0 0.3
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;				// 並行光源
	g_Light[0].Enable = TRUE;								// このライトをON
	SetLight(0, &g_Light[0]);								// これで設定している



	// フォグの初期化（霧の効果）
	g_Fog.FogStart = 50.0f;									// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogEnd   = 210.0f;								// ここまで離れるとフォグの色で見えなくなる
	g_Fog.FogColor = XMFLOAT4( 0.1f, 0.1f, 0.1f, 1.0f );	// フォグの色
	SetFog(&g_Fog);
	SetFogEnable(FALSE);		// 他の場所もチェックする shadow

}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{
	//PLAYER *player = GetPlayer();

	//g_Light[0].Position.z = player[0].pos.z;
	//g_Light[0].Position.x = player[0].pos.x;

}

void SetLightViewProjection(void)	// ライトのポジションからカメラを移してる
{
	// ライトビュー行列設定
	XMMATRIX mtxLightView;
	XMVECTOR pos = XMLoadFloat3(&g_Light[0].Position);
	XMVECTOR dir = XMLoadFloat3(&g_Light[0].Direction);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR vec = XMLoadFloat3(&up);
	mtxLightView = XMMatrixLookToLH(pos, dir, vec);
	SetLightViewMatrix(&mtxLightView);

	//ライト正射影行列設定
	XMMATRIX mtxLightProjection;
	mtxLightProjection = XMMatrixOrthographicLH(SHADOW_RANGE_WIDTH, SHADOW_RANGE_HEIGHT, VIEW_NEAR_Z, VIEW_FAR_Z); // 影の範囲
	SetLightProjectionMatrix(&mtxLightProjection);
}

//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}



