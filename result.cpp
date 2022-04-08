//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "time.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH						(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT						(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX							(5)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO					(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO					(80)			// 

#define TEXTURE_WIDTH_GAMEOVER_LOGO			(850)			// ゲームオーバーロゴサイズ 幅
#define TEXTURE_HEIGHT_GAMEOVER_LOGO		(180)			// ゲームオーバーロゴサイズ 高さ
#define COORDINATE_HEIGHT_GAMEOVER_LOGO		(170)			// ゲームオーバーロゴテクスチャの座標高さ

#define TEXTURE_WIDTH_GAMECLEAR_LOGO		(850)			// ゲームクリアロゴサイズ 幅
#define TEXTURE_HEIGHT_GAMECLEAR_LOGO		(180)			// ゲームクリアロゴサイズ 高さ
#define COORDINATE_HEIGHT_GAMECLEAR_LOGO	(100)			// ゲームクリアロゴテクスチャの座標高さ

#define TEXTURE_WIDTH_TIME_LEFT_LOGO		(280)			// 残り時間のロゴサイズ
#define TEXTURE_HEIGHT_TIME_LEFT_LOGO		(60)			// 
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Result_GameOver/Result.png",		// ゲームクリアのロゴ
	"data/TEXTURE/Result_GameOver/Result2.png",		// 残り時間のロゴ
	"data/TEXTURE/number.png",						// 残り時間の数字のフォント画像
	"data/TEXTURE/Result_GameOver/Result3.png",		// PRESS ANY BUTTON のロゴ
	"data/TEXTURE/Result_GameOver/GameOver.png",	// ゲームオーバーのロゴ

};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static float					g_AlphaValue;				// ロゴのα値用変数
static int						g_Count;					// カウント変数

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { g_w / 2, 50.0f, 0.0f };
	g_TexNo = 0;

	g_AlphaValue = 0.0f;				// 最初は透明にしたいので0で初期化
	g_Count = 300;						// 5秒後に各ロゴを出したいので300フレームで初期化(このゲームは60FPS)

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_RESULT);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_M))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_C))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}


	// タイムカウントを減らす処理
	if (g_Count <= 0)				// カウントが0になったら(１秒経過したら)
	{
		if (g_AlphaValue < 1.0f)
		{
			g_AlphaValue += 0.01f;	// α値を増やす 5フレームに0.01ずつ増える
		}
		else
		{
			g_AlphaValue = 1.0f;	// α値固定させる
		}
		g_Count = 5;				// 一度カウントが0になったら5を代入する
	}

	g_Count--;						// 毎フレームカウントを1減らす


	PrintDebugProc("カウント:%d\n", g_Count);
	PrintDebugProc("リザルトロゴアルファ値:%f\n", g_AlphaValue);

#ifdef _DEBUG	// デバッグ情報を表示する
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	if (GetMode() == MODE_RESULT)
	{
		// マテリアル設定 ゲームクリアロゴのアルファ値を変数にしている
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_AlphaValue);
		SetMaterial(material);

		// リザルトのロゴを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_GAMECLEAR_LOGO, TEXTURE_WIDTH_GAMECLEAR_LOGO, TEXTURE_HEIGHT_GAMECLEAR_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// マテリアル設定 以降はα値固定の為
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		// 残り時間のロゴを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x - 150, g_Pos.y + 170.0f, TEXTURE_WIDTH_TIME_LEFT_LOGO, TEXTURE_HEIGHT_TIME_LEFT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// スコア表示
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			// 桁数分処理する
			int number = GetTime();
			//3桁目を消すためにifを使って分けている
			if (number >= 100)
			{
				for (int i = 0; i < TIME_DIGIT; i++)
				{
					// 今回表示する桁の数字
					float x = (float)(number % 10);

					// スコアの位置やテクスチャー座標を反映
					float pw = 16 * 3;			// スコアの表示幅
					float ph = 32 * 2;			// スコアの表示高さ
					float px = 670.0f - i * pw;	// スコアの表示位置X
					float py = 220.0f;			// スコアの表示位置Y

					float tw = 1.f / 10;		// テクスチャの幅
					float th = 1.0f / 1;		// テクスチャの高さ
					float tx = x * tw;			// テクスチャの左上X座標
					float ty = 0.0f;			// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);

					// 次の桁へ
					number /= 10;
				}
			}
			else	//残り時間が２桁の場合
			{
				for (int i = 0; i < TIME_DIGIT - 1; i++)
				{
					// 今回表示する桁の数字
					float x = (float)(number % 10);

					// スコアの位置やテクスチャー座標を反映
					float pw = 16 * 3;			// スコアの表示幅
					float ph = 32 * 2;			// スコアの表示高さ
					float px = 670.0f - i * pw;	// スコアの表示位置X
					float py = 220.0f;			// スコアの表示位置Y

					float tw = 1.f / 10;		// テクスチャの幅
					float th = 1.0f / 1;		// テクスチャの高さ
					float tx = x * tw;			// テクスチャの左上X座標
					float ty = 0.0f;			// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);

					// 次の桁へ
					number /= 10;
				}

			}
		}

		// PRESS ANY BUTTON のロゴを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y + 400, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}
	if (GetMode() == MODE_GAMEOVER)
	{
		// マテリアル設定 ゲームオーバーロゴのアルファ値を変数にしている
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_AlphaValue);
		SetMaterial(material);

		// ゲームオーバーのロゴを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_GAMEOVER_LOGO, TEXTURE_WIDTH_GAMEOVER_LOGO, TEXTURE_HEIGHT_GAMEOVER_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

		// マテリアル設定 以降はα値固定の為
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		// PRESS ANY BUTTON のロゴを描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y + 400, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}



}




