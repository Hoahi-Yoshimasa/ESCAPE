//=============================================================================
//
// スタミナゲージ処理 [stamina_bar.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "stamina_bar.h"
#include "sprite.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(300.0f)	// 幅
#define TEXTURE_HEIGHT				(23.0f)		// 高さ
#define TEXTURE_MAX					(1)			// テクスチャの数

#define STAMINA_BAR_MAX				(2)			// スタミナバーの総数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/stamina.png",
};

static int						g_TexNo;					// テクスチャ番号

static BOOL			g_Load = FALSE;		// 初期化を行ったかのフラグ
static STAMINA_BAR	g_StaminaBar[STAMINA_BAR_MAX];


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStaminaBar(void)
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


	// スタミナバーの構造体の初期化
	for (int i = 0; i < STAMINA_BAR_MAX; i++)
	{
		g_StaminaBar[i].w = TEXTURE_WIDTH;
		g_StaminaBar[i].h = TEXTURE_HEIGHT;
		g_StaminaBar[i].pos = XMFLOAT3(230.0f, 60.0f, 0.0f);	// 座標
		g_StaminaBar[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_StaminaBar[i].texNo = 0;
	}
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStaminaBar(void)
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
void UpdateStaminaBar(void)
{
	SetStaminaBar();

	// プレイヤー用
	g_StaminaBar[0].pos.x = 230 - (TEXTURE_WIDTH / 2 - g_StaminaBar[0].w / 2);


#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStaminaBar(void)
{
	PLAYER *player = GetPlayer();
	int *P_StaminaRecovery_count = GetStaminaRecovery_count();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	// スタミナバーの最大値描画　プレイヤー用

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_StaminaBar[0].texNo]);

	//スタミナバーの位置やテクスチャー座標を反映
	float px = 230.0f;						// スタミナバーの表示位置X
	float py = g_StaminaBar[0].pos.y;		// スタミナバーの表示位置Y
	float pw = TEXTURE_WIDTH;				// スタミナバーの表示幅
	float ph = g_StaminaBar[0].h;			// スタミナバーの表示高さ

	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteColorRotation(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		g_StaminaBar[0].rot.z);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);



	// スタミナバーの描画処理

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_StaminaBar[0].texNo]);

	//スタミナバーの位置やテクスチャー座標を反映
	px = g_StaminaBar[0].pos.x;		// スタミナバーの表示位置X
	py = g_StaminaBar[0].pos.y;		// スタミナバーの表示位置Y
	pw = g_StaminaBar[0].w;			// スタミナバーの表示幅
	ph = g_StaminaBar[0].h;			// スタミナバーの表示高さ

	tw = 1.0f;	// テクスチャの幅
	th = 1.0f;	// テクスチャの高さ
	tx = 0.0f;	// テクスチャの左上X座標
	ty = 0.0f;	// テクスチャの左上Y座標

	// スタミナゲージを使い切ったら回復時間中赤くする処理
	if (*P_StaminaRecovery_count == 0)
	{	// 通常時(黄色)
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 0.84f, 0.0f, 1.0f),
			g_StaminaBar[1].rot.z);
	}
	else
	{	// 使い切ったので回復時間中は赤くする
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
			g_StaminaBar[1].rot.z);

	}


	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}


void SetStaminaBar(void)
{
	PLAYER *player = GetPlayer();
	g_StaminaBar[0].w = TEXTURE_WIDTH * player[0].staminaPercent;

}
