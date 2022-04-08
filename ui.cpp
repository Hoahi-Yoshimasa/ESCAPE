//=============================================================================
//
// UI処理 [ui.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "sprite.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(16)		// サイズ
#define TEXTURE_HEIGHT				(32)		// 

#define DASH_TEXTURE_WIDTH			(60.0f)		// ダッシュアイコンの表示幅
#define DASH_TEXTURE_HEIGHT			(40.0f)		// 高さ

#define KEY_UI_TEXTURE_WIDTH		(100.0f)	// アイテム取得欄表示幅
#define KEY_UI_TEXTURE_HEIGHT		(250.0f)	// 高さ

#define KEY_TEXTURE_WIDTH			(70.0f)		// キーアイテム表示幅
#define KEY_TEXTURE_HEIGHT			(70.0f)		// 高さ

#define TEXT_FIELD_TEXTURE_WIDTH	(600.0f)	// テキスト表示欄の表示幅
#define TEXT_FIELD_TEXTURE_HEIGHT	(150.0f)	// 高さ

#define TEXT_TEXTURE_WIDTH			(400.0f)	// テキストの表示幅
#define TEXT_TEXTURE_HEIGHT			(50.0f)		// 高さ

#define TEXTURE_MAX					(19)		// 総テクスチャ数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/UI/DASH.png",
	"data/TEXTURE/UI/UI.png",
	"data/TEXTURE/UI/key.png",
	"data/TEXTURE/Tutorial/Tutorial_Text1.png",
	"data/TEXTURE/Tutorial/Tutorial_Text2.png",
	"data/TEXTURE/Tutorial/Tutorial_Text3.png",
	"data/TEXTURE/Tutorial/Tutorial_Text4.png",
	"data/TEXTURE/Tutorial/Tutorial_Text5.png",
	"data/TEXTURE/Tutorial/Tutorial_Text6.png",
	"data/TEXTURE/Tutorial/Tutorial_Text7.png",
	"data/TEXTURE/Tutorial/Tutorial_Text8.png",
	"data/TEXTURE/Tutorial/Tutorial_Text9.png",
	"data/TEXTURE/Tutorial/Tutorial_Text10.png",
	"data/TEXTURE/Tutorial/Tutorial_Text11.png",
	"data/TEXTURE/Tutorial/Tutorial_Text12.png",
	"data/TEXTURE/Tutorial/Tutorial_Text13.png",
	"data/TEXTURE/Tutorial/Tutorial_Text14.png",
	"data/TEXTURE/Tutorial/Tutorial_Text15.png",
	"data/TEXTURE/Tutorial/Tutorial_Text16.png",
};


//static BOOL					g_Use;						// TRUE:使っている  FALSE:未使用
//static float					g_w, g_h;					// 幅と高さ
//static XMFLOAT3				g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static UI						g_Ui[MAX_UI];				// UIの数

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUi(void)
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

	g_TexNo = 0;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// UIの初期化
	for (int i = 0; i < MAX_UI; i++)
	{
		g_Ui[i].pos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };				// 位置
		g_Ui[i].scl = XMFLOAT3{ 0.0f, 0.0f, 0.0f };				// スケール
		g_Ui[i].fWidth = TEXTURE_WIDTH;							// 幅
		g_Ui[i].fHeight = TEXTURE_HEIGHT;;						// 高さ
		g_Ui[i].collected = 0.2f;								// アイテム(鍵)の取得状況 初期化時は0.2f(半透明で初期化)
		g_Ui[i].use = TRUE;										// 使用しているかどうか
	}

	// DASHゲージの初期化処理
	g_Ui[0].pos = XMFLOAT3{ 40.0f, 60.0f, 0.0f };				// 位置
	g_Ui[0].fWidth = DASH_TEXTURE_WIDTH;						// 幅
	g_Ui[0].fHeight = DASH_TEXTURE_HEIGHT;						// 高さ

	//鍵の取得ゲージの初期化処理
	g_Ui[1].pos = XMFLOAT3{ 910.0f, 280.0f, 0.0f };				// 位置
	g_Ui[1].fWidth = KEY_UI_TEXTURE_WIDTH;						// 幅
	g_Ui[1].fHeight = KEY_UI_TEXTURE_HEIGHT;					// 高さ

	//赤い鍵画像の初期化処理
	g_Ui[2].pos = XMFLOAT3{ 910.0f, 200.0f, 0.0f };				// 位置
	g_Ui[2].fWidth = KEY_TEXTURE_WIDTH;							// 幅
	g_Ui[2].fHeight = KEY_TEXTURE_HEIGHT;						// 高さ

	//青い鍵画像の初期化処理
	g_Ui[3].pos = XMFLOAT3{ 910.0f, 280.0f, 0.0f };				// 位置
	g_Ui[3].fWidth = KEY_TEXTURE_WIDTH;							// 幅
	g_Ui[3].fHeight = KEY_TEXTURE_HEIGHT;						// 高さ

	//黄色の鍵画像の初期化処理
	g_Ui[4].pos = XMFLOAT3{ 910.0f, 360.0f, 0.0f };				// 位置
	g_Ui[4].fWidth = KEY_TEXTURE_WIDTH;							// 幅
	g_Ui[4].fHeight = KEY_TEXTURE_HEIGHT;						// 高さ

	//テキスト欄の初期化処理
	g_Ui[5].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[5].fWidth = TEXT_FIELD_TEXTURE_WIDTH;					// 幅
	g_Ui[5].fHeight = TEXT_FIELD_TEXTURE_HEIGHT;				// 高さ

	//テキスト1の初期化処理
	g_Ui[6].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[6].fWidth = TEXT_TEXTURE_WIDTH;						// 幅
	g_Ui[6].fHeight = TEXT_TEXTURE_HEIGHT;						// 高さ
	g_Ui[6].use = TRUE;											// 使用しているかどうか

	//テキスト2の初期化処理
	g_Ui[7].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[7].fWidth = TEXT_TEXTURE_WIDTH;						// 幅
	g_Ui[7].fHeight = TEXT_TEXTURE_HEIGHT;						// 高さ
	g_Ui[7].use = FALSE;										// 使用しているかどうか

	//テキスト3の初期化処理
	g_Ui[8].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[8].fWidth = TEXT_TEXTURE_WIDTH;						// 幅
	g_Ui[8].fHeight = TEXT_TEXTURE_HEIGHT * 2;					// 高さ
	g_Ui[8].use = FALSE;										// 使用しているかどうか

	//テキスト4の初期化処理
	g_Ui[9].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[9].fWidth = TEXT_TEXTURE_WIDTH;						// 幅
	g_Ui[9].fHeight = TEXT_TEXTURE_HEIGHT;						// 高さ
	g_Ui[9].use = FALSE;										// 使用しているかどうか

	//テキスト5の初期化処理
	g_Ui[10].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[10].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// 幅
	g_Ui[10].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// 高さ
	g_Ui[10].use = FALSE;										// 使用しているかどうか


	//テキスト6の初期化処理
	g_Ui[11].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[11].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// 幅
	g_Ui[11].fHeight = TEXT_TEXTURE_HEIGHT * 2.2f;				// 高さ
	g_Ui[11].use = FALSE;										// 使用しているかどうか

	//テキスト7の初期化処理
	g_Ui[12].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[12].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// 幅
	g_Ui[12].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// 高さ
	g_Ui[12].use = FALSE;										// 使用しているかどうか

	//テキスト8の初期化処理
	g_Ui[13].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[13].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// 幅
	g_Ui[13].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// 高さ
	g_Ui[13].use = FALSE;										// 使用しているかどうか

	//テキスト9の初期化処理
	g_Ui[14].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[14].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// 幅
	g_Ui[14].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// 高さ
	g_Ui[14].use = FALSE;										// 使用しているかどうか

	//テキスト10の初期化処理
	g_Ui[15].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[15].fWidth = TEXT_TEXTURE_WIDTH * 1.4f;				// 幅
	g_Ui[15].fHeight = TEXT_TEXTURE_HEIGHT * 1.7f;				// 高さ
	g_Ui[15].use = FALSE;										// 使用しているかどうか

	//テキスト11の初期化処理
	g_Ui[16].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[16].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// 幅
	g_Ui[16].fHeight = TEXT_TEXTURE_HEIGHT * 1.7f;				// 高さ
	g_Ui[16].use = FALSE;										// 使用しているかどうか

	//テキスト12の初期化処理
	g_Ui[17].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[17].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// 幅
	g_Ui[17].fHeight = TEXT_TEXTURE_HEIGHT * 1.7f;				// 高さ
	g_Ui[17].use = FALSE;										// 使用しているかどうか

	//テキスト13の初期化処理
	g_Ui[18].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[18].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// 幅
	g_Ui[18].fHeight = TEXT_TEXTURE_HEIGHT * 3;					// 高さ
	g_Ui[18].use = FALSE;										// 使用しているかどうか

	//テキスト14の初期化処理
	g_Ui[19].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[19].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// 幅
	g_Ui[19].fHeight = TEXT_TEXTURE_HEIGHT * 3;					// 高さ
	g_Ui[19].use = FALSE;										// 使用しているかどうか

	//テキスト15の初期化処理
	g_Ui[20].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// 位置
	g_Ui[20].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// 幅
	g_Ui[20].fHeight = TEXT_TEXTURE_HEIGHT * 2.0f;				// 高さ
	g_Ui[20].use = FALSE;										// 使用しているかどうか

	//テキスト16の初期化処理
	g_Ui[21].pos = XMFLOAT3{ SCREEN_WIDTH / 1.5f, 510.0f, 0.0f };	// 位置
	g_Ui[21].fWidth = TEXT_TEXTURE_WIDTH * 0.7f;				// 幅
	g_Ui[21].fHeight = TEXT_TEXTURE_HEIGHT * 0.7f;				// 高さ
	g_Ui[21].use = TRUE;										// 使用しているかどうか

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUi(void)
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
void UpdateUi(void)
{
	// 最後のテキストが表示されたら Next BUTTON の表示を消す処理
	if (g_Ui[19].use == TRUE)
	{
		g_Ui[20].use = FALSE;
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUi(void)
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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// スタミナゲージのアイコン表示

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// 位置やテクスチャー座標を反映
	float px = g_Ui[0].pos.x;			// 表示位置X
	float py = g_Ui[0].pos.y;			// 表示位置Y
	float pw = g_Ui[0].fWidth;			// 表示幅
	float ph = g_Ui[0].fHeight;			// 表示高さ

	float tw = 1.0f;					// テクスチャの幅
	float th = 1.0f;					// テクスチャの高さ
	float tx = 0.0f;					// テクスチャの左上X座標
	float ty = 0.0f;					// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標と色の設定
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	// KEYの取得欄の表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	// 位置やテクスチャー座標を反映
	float px1 = g_Ui[1].pos.x;			// 表示位置X
	float py1 = g_Ui[1].pos.y;			// 表示位置Y
	float pw1 = g_Ui[1].fWidth;			// 表示幅
	float ph1 = g_Ui[1].fHeight;		// 表示高さ

	float tw1 = 1.0f;					// テクスチャの幅
	float th1 = 1.0f;					// テクスチャの高さ
	float tx1 = 0.0f;					// テクスチャの左上X座標
	float ty1 = 0.0f;					// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標と色の設定
	SetSpriteColor(g_VertexBuffer, px1, py1, pw1, ph1, tx1, ty1, tw1, th1,
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);


	// 赤い鍵の表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// 位置やテクスチャー座標を反映
	float px2 = g_Ui[2].pos.x;			// 表示位置X
	float py2 = g_Ui[2].pos.y;			// 表示位置Y
	float pw2 = g_Ui[2].fWidth;			// 表示幅
	float ph2 = g_Ui[2].fHeight;		// 表示高さ

	float tw2 = 1.0f;					// テクスチャの幅
	float th2 = 1.0f;					// テクスチャの高さ
	float tx2 = 0.0f;					// テクスチャの左上X座標
	float ty2 = 0.0f;					// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標と色の設定
	SetSpriteColor(g_VertexBuffer, px2, py2, pw2, ph2, tx2, ty2, tw2, th2,
		XMFLOAT4(1.0f, 0.0f, 0.0f, g_Ui[2].collected));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// 青い鍵の表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// 位置やテクスチャー座標を反映
	float px3 = g_Ui[3].pos.x;			// 表示位置X
	float py3 = g_Ui[3].pos.y;			// 表示位置Y
	float pw3 = g_Ui[3].fWidth;			// 表示幅
	float ph3 = g_Ui[3].fHeight;		// 表示高さ

	float tw3 = 1.0f;					// テクスチャの幅
	float th3 = 1.0f;					// テクスチャの高さ
	float tx3 = 0.0f;					// テクスチャの左上X座標
	float ty3 = 0.0f;					// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標と色の設定
	SetSpriteColor(g_VertexBuffer, px3, py3, pw3, ph3, tx3, ty3, tw3, th3,
		XMFLOAT4(0.0f, 0.0f, 1.0f, g_Ui[3].collected));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// 黄色い鍵の表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// 位置やテクスチャー座標を反映
	float px4 = g_Ui[4].pos.x;			// 表示位置X
	float py4 = g_Ui[4].pos.y;			// 表示位置Y
	float pw4 = g_Ui[4].fWidth;			// 表示幅
	float ph4 = g_Ui[4].fHeight;		// 表示高さ

	float tw4 = 1.0f;					// テクスチャの幅
	float th4 = 1.0f;					// テクスチャの高さ
	float tx4 = 0.0f;					// テクスチャの左上X座標
	float ty4 = 0.0f;					// テクスチャの左上Y座標

	// １枚のポリゴンの頂点とテクスチャ座標と色の設定
	SetSpriteColor(g_VertexBuffer, px4, py4, pw4, ph4, tx4, ty4, tw4, th4,
		XMFLOAT4(1.0f, 1.0f, 0.0f, g_Ui[4].collected));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	if (GetMode() == MODE_TUTORIAL)
	{
		// テキスト欄の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// 位置やテクスチャー座標を反映
		float px5 = g_Ui[5].pos.x;			// 表示位置X
		float py5 = g_Ui[5].pos.y;			// 表示位置Y
		float pw5 = g_Ui[5].fWidth;			// 表示幅
		float ph5 = g_Ui[5].fHeight;		// 表示高さ

		float tw5 = 1.0f;					// テクスチャの幅
		float th5 = 1.0f;					// テクスチャの高さ
		float tx5 = 0.0f;					// テクスチャの左上X座標
		float ty5 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px5, py5, pw5, ph5, tx5, ty5, tw5, th5,
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テキスト1の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// 位置やテクスチャー座標を反映
		float px6 = g_Ui[6].pos.x;			// 表示位置X
		float py6 = g_Ui[6].pos.y;			// 表示位置Y
		float pw6 = g_Ui[6].fWidth;			// 表示幅
		float ph6 = g_Ui[6].fHeight;		// 表示高さ

		float tw6 = 1.0f;					// テクスチャの幅
		float th6 = 1.0f;					// テクスチャの高さ
		float tx6 = 0.0f;					// テクスチャの左上X座標
		float ty6 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px6, py6, pw6, ph6, tx6, ty6, tw6, th6,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[6].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト2の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// 位置やテクスチャー座標を反映
		float px7 = g_Ui[7].pos.x;			// 表示位置X
		float py7 = g_Ui[7].pos.y;			// 表示位置Y
		float pw7 = g_Ui[7].fWidth;			// 表示幅
		float ph7 = g_Ui[7].fHeight;		// 表示高さ

		float tw7 = 1.0f;					// テクスチャの幅
		float th7 = 1.0f;					// テクスチャの高さ
		float tx7 = 0.0f;					// テクスチャの左上X座標
		float ty7 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px7, py7, pw7, ph7, tx7, ty7, tw7, th7,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[7].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト3の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// 位置やテクスチャー座標を反映
		float px8 = g_Ui[8].pos.x;			// 表示位置X
		float py8 = g_Ui[8].pos.y;			// 表示位置Y
		float pw8 = g_Ui[8].fWidth;			// 表示幅
		float ph8 = g_Ui[8].fHeight;		// 表示高さ

		float tw8 = 1.0f;					// テクスチャの幅
		float th8 = 1.0f;					// テクスチャの高さ
		float tx8 = 0.0f;					// テクスチャの左上X座標
		float ty8 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px8, py8, pw8, ph8, tx8, ty8, tw8, th8,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[8].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト4の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// 位置やテクスチャー座標を反映
		float px9 = g_Ui[9].pos.x;			// 表示位置X
		float py9 = g_Ui[9].pos.y;			// 表示位置Y
		float pw9 = g_Ui[9].fWidth;			// 表示幅
		float ph9 = g_Ui[9].fHeight;		// 表示高さ

		float tw9 = 1.0f;					// テクスチャの幅
		float th9 = 1.0f;					// テクスチャの高さ
		float tx9 = 0.0f;					// テクスチャの左上X座標
		float ty9 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px9, py9, pw9, ph9, tx9, ty9, tw9, th9,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[9].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テキスト5の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// 位置やテクスチャー座標を反映
		float px10 = g_Ui[10].pos.x;			// 表示位置X
		float py10 = g_Ui[10].pos.y;			// 表示位置Y
		float pw10 = g_Ui[10].fWidth;			// 表示幅
		float ph10 = g_Ui[10].fHeight;			// 表示高さ

		float tw10 = 1.0f;					// テクスチャの幅
		float th10 = 1.0f;					// テクスチャの高さ
		float tx10 = 0.0f;					// テクスチャの左上X座標
		float ty10 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px10, py10, pw10, ph10, tx10, ty10, tw10, th10,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[10].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト6の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		// 位置やテクスチャー座標を反映
		float px11 = g_Ui[11].pos.x;			// 表示位置X
		float py11 = g_Ui[11].pos.y;			// 表示位置Y
		float pw11 = g_Ui[11].fWidth;			// 表示幅
		float ph11 = g_Ui[11].fHeight;			// 表示高さ

		float tw11 = 1.0f;					// テクスチャの幅
		float th11 = 1.0f;					// テクスチャの高さ
		float tx11 = 0.0f;					// テクスチャの左上X座標
		float ty11 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px11, py11, pw11, ph11, tx11, ty11, tw11, th11,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[11].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テキスト7の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// 位置やテクスチャー座標を反映
		float px12 = g_Ui[12].pos.x;			// 表示位置X
		float py12 = g_Ui[12].pos.y;			// 表示位置Y
		float pw12 = g_Ui[12].fWidth;			// 表示幅
		float ph12 = g_Ui[12].fHeight;			// 表示高さ

		float tw12 = 1.0f;					// テクスチャの幅
		float th12 = 1.0f;					// テクスチャの高さ
		float tx12 = 0.0f;					// テクスチャの左上X座標
		float ty12 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px12, py12, pw12, ph12, tx12, ty12, tw12, th12,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[12].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト8の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

		// 位置やテクスチャー座標を反映
		float px13 = g_Ui[13].pos.x;			// 表示位置X
		float py13 = g_Ui[13].pos.y;			// 表示位置Y
		float pw13 = g_Ui[13].fWidth;			// 表示幅
		float ph13 = g_Ui[13].fHeight;			// 表示高さ

		float tw13 = 1.0f;					// テクスチャの幅
		float th13 = 1.0f;					// テクスチャの高さ
		float tx13 = 0.0f;					// テクスチャの左上X座標
		float ty13 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px13, py13, pw13, ph13, tx13, ty13, tw13, th13,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[13].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テキスト9の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

		// 位置やテクスチャー座標を反映
		float px14 = g_Ui[14].pos.x;			// 表示位置X
		float py14 = g_Ui[14].pos.y;			// 表示位置Y
		float pw14 = g_Ui[14].fWidth;			// 表示幅
		float ph14 = g_Ui[14].fHeight;			// 表示高さ

		float tw14 = 1.0f;					// テクスチャの幅
		float th14 = 1.0f;					// テクスチャの高さ
		float tx14 = 0.0f;					// テクスチャの左上X座標
		float ty14 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px14, py14, pw14, ph14, tx14, ty14, tw14, th14,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[14].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト10の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

		// 位置やテクスチャー座標を反映
		float px15 = g_Ui[15].pos.x;			// 表示位置X
		float py15 = g_Ui[15].pos.y;			// 表示位置Y
		float pw15 = g_Ui[15].fWidth;			// 表示幅
		float ph15 = g_Ui[15].fHeight;			// 表示高さ

		float tw15 = 1.0f;					// テクスチャの幅
		float th15 = 1.0f;					// テクスチャの高さ
		float tx15 = 0.0f;					// テクスチャの左上X座標
		float ty15 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px15, py15, pw15, ph15, tx15, ty15, tw15, th15,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[15].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テキスト11の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

		// 位置やテクスチャー座標を反映
		float px16 = g_Ui[16].pos.x;			// 表示位置X
		float py16 = g_Ui[16].pos.y;			// 表示位置Y
		float pw16 = g_Ui[16].fWidth;			// 表示幅
		float ph16 = g_Ui[16].fHeight;			// 表示高さ

		float tw16 = 1.0f;					// テクスチャの幅
		float th16 = 1.0f;					// テクスチャの高さ
		float tx16 = 0.0f;					// テクスチャの左上X座標
		float ty16 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px16, py16, pw16, ph16, tx16, ty16, tw16, th16,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[16].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト12の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);

		// 位置やテクスチャー座標を反映
		float px17 = g_Ui[17].pos.x;			// 表示位置X
		float py17 = g_Ui[17].pos.y;			// 表示位置Y
		float pw17 = g_Ui[17].fWidth;			// 表示幅
		float ph17 = g_Ui[17].fHeight;			// 表示高さ

		float tw17 = 1.0f;					// テクスチャの幅
		float th17 = 1.0f;					// テクスチャの高さ
		float tx17 = 0.0f;					// テクスチャの左上X座標
		float ty17 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px17, py17, pw17, ph17, tx17, ty17, tw17, th17,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[17].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テキスト13の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

		// 位置やテクスチャー座標を反映
		float px18 = g_Ui[18].pos.x;			// 表示位置X
		float py18 = g_Ui[18].pos.y;			// 表示位置Y
		float pw18 = g_Ui[18].fWidth;			// 表示幅
		float ph18 = g_Ui[18].fHeight;			// 表示高さ

		float tw18 = 1.0f;					// テクスチャの幅
		float th18 = 1.0f;					// テクスチャの高さ
		float tx18 = 0.0f;					// テクスチャの左上X座標
		float ty18 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px18, py18, pw18, ph18, tx18, ty18, tw18, th18,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[18].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テキスト14の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

		// 位置やテクスチャー座標を反映
		float px19 = g_Ui[19].pos.x;			// 表示位置X
		float py19 = g_Ui[19].pos.y;			// 表示位置Y
		float pw19 = g_Ui[19].fWidth;			// 表示幅
		float ph19 = g_Ui[19].fHeight;			// 表示高さ

		float tw19 = 1.0f;					// テクスチャの幅
		float th19 = 1.0f;					// テクスチャの高さ
		float tx19 = 0.0f;					// テクスチャの左上X座標
		float ty19 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px19, py19, pw19, ph19, tx19, ty19, tw19, th19,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[19].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


		// テキスト15の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[17]);

		// 位置やテクスチャー座標を反映
		float px20 = g_Ui[20].pos.x;			// 表示位置X
		float py20 = g_Ui[20].pos.y;			// 表示位置Y
		float pw20 = g_Ui[20].fWidth;			// 表示幅
		float ph20 = g_Ui[20].fHeight;			// 表示高さ

		float tw20 = 1.0f;					// テクスチャの幅
		float th20 = 1.0f;					// テクスチャの高さ
		float tx20 = 0.0f;					// テクスチャの左上X座標
		float ty20 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px20, py20, pw20, ph20, tx20, ty20, tw20, th20,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[20].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// Next画像の表示
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[18]);

		// 位置やテクスチャー座標を反映
		float px21 = g_Ui[21].pos.x;			// 表示位置X
		float py21 = g_Ui[21].pos.y;			// 表示位置Y
		float pw21 = g_Ui[21].fWidth;			// 表示幅
		float ph21 = g_Ui[21].fHeight;			// 表示高さ

		float tw21 = 1.0f;					// テクスチャの幅
		float th21 = 1.0f;					// テクスチャの高さ
		float tx21 = 0.0f;					// テクスチャの左上X座標
		float ty21 = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標と色の設定
		SetSpriteColor(g_VertexBuffer, px21, py21, pw21, ph21, tx21, ty21, tw21, th21,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[21].use)));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


	}

}


//=============================================================================
// UI情報を取得
//=============================================================================

UI *GetUi(void)
{
	return g_Ui;
}

