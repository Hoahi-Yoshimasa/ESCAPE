//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数


#define TEXTURE_WIDTH_TITLE_LOGO			(850)		// タイトルロゴサイズ 幅
#define TEXTURE_HEIGHT_TITLE_LOGO			(180)		// 高さ

#define TEXTURE_WIDTH_PRESS_LOGO			(480)		// PRESS ANY BUTTONロゴサイズ
#define TEXTURE_HEIGHT_PRESS_LOGO			(80)		// 高さ

#define TEXTURE_WIDTH_MODE_CHOICE_LOGO		(400)		// モード選択ロゴサイズ
#define TEXTURE_HEIGHT_MODE_CHOICE_LOGO		(60)		// 高さ

#define TEXTURE_WIDTH_CURSOR_LOGO			(150)		// カーソルロゴサイズ 幅
#define TEXTURE_HEIGHT_CURSOR_LOGO			(50)		// 高さ

#define COORDINATE_HEIGHT_GAME_LOGO			(380)		// ゲームスタートロゴの座標高さ
#define COORDINATE_HEIGHT_TUTORIAL_LOGO		(460)		// チュートリアルロゴの座標高さ
#define COORDINATE_HEIGHT_TITLE_LOGO		(170)		// タイトルテクスチャの座標高さ


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Title/Title1.png",		// タイトル画像
	"data/TEXTURE/Title/Title2.png",		// ゲームスタート画像
	"data/TEXTURE/Title/Title3.png",		// チュートリアル画像
	"data/TEXTURE/Title/Skeleton_hand.png",	// カーソル画像
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static XMFLOAT4					g_CursorColourRed;			// カーソル用赤色の設定
static XMFLOAT4					g_CursorColourWhite;		// カーソル用白色の設定

static XMFLOAT4					CursorColourGame;			// ゲームロゴの色変え用変数
static XMFLOAT4					CursorColourTutorial;		// チュートリアルロゴの色変え用変数

static float					g_AlphaValue;				// タイトルロゴのα値用変数
static int						g_Count;					// カウント変数

static BOOL						g_Load = FALSE;

static int						g_Cursor;					// 選択カーソル用変数
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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


	// 変数の初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;
	g_Cursor = 0;										// 選択カーソル用変数
	g_AlphaValue = 0.0f;								// 最初はタイトル画面を透明にしたいので0で初期化
	g_Count = 540;										// 9秒後にタイトルロゴを出したいので540フレームで初期化(このゲームは60FPS)

	g_CursorColourRed = { 1.0f, 0.0f, 0.0f, 1.0f };		// カーソル用赤色の設定
	g_CursorColourWhite = { 1.0f, 1.0f, 1.0f, 1.0f };	// カーソル用白色の設定
	CursorColourGame = g_CursorColourWhite;				// ゲームロゴの色変え用変数 (白色で初期化)
	CursorColourTutorial = g_CursorColourRed;			// チュートリアルロゴの色変え用変数 (赤色で初期化)

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_OP);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	PrintDebugProc("カーソル:%d %n", g_Cursor);

	// キーボード操作
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_Cursor++;
	}
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_Cursor++;
	}

	// コントローラー操作
	if (IsButtonTriggered(0, BUTTON_UP))
	{
		g_Cursor++;
	}
	if (IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_Cursor++;
	}

	g_Cursor = g_Cursor % 2;

	if (g_Cursor == 0)
	{
		CursorColourGame = g_CursorColourWhite;				// ゲームロゴの色変え用変数 (白色で初期化)
		CursorColourTutorial = g_CursorColourRed;			// チュートリアルロゴの色変え用変数 (赤色で初期化)
	}
	else
	{
		CursorColourGame = g_CursorColourRed;				// ゲームロゴの色変え用変数 (白色で初期化)
		CursorColourTutorial = g_CursorColourWhite;			// チュートリアルロゴの色変え用変数 (赤色で初期化)
	}

	// シーン遷移する処理
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_C) || IsButtonTriggered(0, BUTTON_M))
	{
		switch (g_Cursor)
		{
		case 0:
			SetFade(FADE_OUT, MODE_GAME);
			break;

		case 1:
			SetFade(FADE_OUT, MODE_TUTORIAL);
			break;

		default:
			break;
		}
	}

	// タイムカウントを減らす処理
	if (g_Count <= 0)				// カウントが0になったら(１秒経過したら)
	{
		if (g_AlphaValue < 1.0f)
		{
			g_AlphaValue += 0.01f;	// タイトルロゴのα値を増やす 5フレームに0.01ずつ増える
		}
		else
		{
			g_AlphaValue = 1.0f;	// α値固定させる
		}
		g_Count = 5;				// 一度カウントが0になったら5を代入する
	}
	g_Count--;						// 毎フレームカウントを1減らす


	PrintDebugProc("タイトルカウント:%d\n", g_Count);
	PrintDebugProc("タイトルアルファ値:%f\n", g_AlphaValue);
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_AlphaValue);	// タイトルロゴを時間によって出現させる
	SetMaterial(material);

	// タイトルのロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_TITLE_LOGO, TEXTURE_WIDTH_TITLE_LOGO, TEXTURE_HEIGHT_TITLE_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// マテリアル設定
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// ゲームスタートのロゴ描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_GAME_LOGO, TEXTURE_WIDTH_MODE_CHOICE_LOGO, TEXTURE_HEIGHT_MODE_CHOICE_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
			CursorColourGame);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// チュートリアルのロゴ描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_TUTORIAL_LOGO, TEXTURE_WIDTH_MODE_CHOICE_LOGO, TEXTURE_HEIGHT_MODE_CHOICE_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
			CursorColourTutorial);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// カーソルのロゴ描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Pos.x / 2.5f + 50 * g_Cursor, 380.0f + 90.0f *g_Cursor, TEXTURE_WIDTH_CURSOR_LOGO, TEXTURE_HEIGHT_CURSOR_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
			g_CursorColourWhite);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}





