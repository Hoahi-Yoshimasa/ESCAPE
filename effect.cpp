//=============================================================================
//
// エフェクト処理 [effect.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "effect.h"
#include "sound.h"
#include "sprite.h"
#include "enemy.h"
#include "player.h"
#include "debugproc.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(1)				// テクスチャの数

#define EFFECT_START_POS			(120.0f)		// エフェクトが出始める距離(エネミーと比較したときの距離)

#define	FADE_RATE					(0.02f)			// フェード係数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/effect_red.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static XMFLOAT4					g_Color;					// エフェクトのカラー（α値）

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// エフェクトの初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	g_Color = { 1.0f, 0.0f, 0.0f, 1.0f };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEffect(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEffect(void)
{
	// エネミー近づくにつれて赤いエフェクトを画面に出す処理

	ENEMY *enemy = GetEnemy();
	PLAYER *player = GetPlayer();					// プレイヤーのポインターを初期化

	g_Color.w = 1.0f - (enemy[0].p_dist / EFFECT_START_POS);	// 距離が小さくなるほどエネミーとプレイヤーの距離が近い

	// プレイヤーが死んだら画面を赤くする処理
	if (player[0].use == FALSE)
	{
		g_Color.w = 1.0f;
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("エフェクト:%f\n", g_Color.w);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEffect(void)
{
	// マトリクス設定
	SetWorldViewProjection2D();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// エフェクト画像を描画

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	//SetVertex(0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f);
	SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, TEXTURE_WIDTH / 2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
		g_Color);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

}
