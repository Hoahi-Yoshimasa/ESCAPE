//=============================================================================
//
// 鍵アイコン処理 [key_icon.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "key_icon.h"
#include "player.h"
#include "debugproc.h"
#include "collision.h"
#include "item_key.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(1)				// テクスチャの数

#define	KEYICON_WIDTH				(10.0f)			// 頂点サイズ
#define	KEYICON_HEIGHT				(10.0f)			// 頂点サイズ

#define	MAX_KEYICON					(3)				// アイコン最大数

#define KEYICON_RISE_LIMIT			(50.0f)			// エネミーのアニメーション上昇最大値
#define KEYICON_DESCENDING_LIMIT	(40.0f)			// エネミーのアニメーション下降最大値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexKeyIcon(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static KEYICON				g_KeyIcon[MAX_KEYICON];						// KEY_ICON
static int					g_TexNo;									// テクスチャ番号
static BOOL					g_bAlpaTest;								// アルファテストON/OFF

static BOOL					g_Load = FALSE;

static float anime_speed;												// エネミーのアニメーション速度用変数


static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/UI/keyIcon.png",
};

// モードゲーム用
static INTERPOLATION_DATA move_tbl_game_red[] = {		// 赤い鍵 pos, rot, scl, frame
	{ XMFLOAT3(293.0f, KEYICON_RISE_LIMIT,       -240.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // 開始地点
	{ XMFLOAT3(293.0f, KEYICON_DESCENDING_LIMIT, -240.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(293.0f, KEYICON_RISE_LIMIT,       -240.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_game_blue[] = {		// 青い鍵 pos, rot, scl, frame
	{ XMFLOAT3(245.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // 開始地点
	{ XMFLOAT3(245.0f, KEYICON_DESCENDING_LIMIT,  420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(245.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_game_yellow[] = {	// 黄色い鍵 pos, rot, scl, frame
	{ XMFLOAT3(-50.0f, KEYICON_RISE_LIMIT,        243.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // 開始地点
	{ XMFLOAT3(-50.0f, KEYICON_DESCENDING_LIMIT,  243.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-50.0f, KEYICON_RISE_LIMIT,        243.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};



// チュートリアル用
static INTERPOLATION_DATA move_tbl_tutorial_red[] = {		// 赤い鍵 pos, rot, scl, frame
	{ XMFLOAT3(-420.0f, KEYICON_RISE_LIMIT,       -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // 開始地点
	{ XMFLOAT3(-420.0f, KEYICON_DESCENDING_LIMIT, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-420.0f, KEYICON_RISE_LIMIT,       -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_tutorial_blue[] = {		// 青い鍵 pos, rot, scl, frame
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // 開始地点
	{ XMFLOAT3(420.0f, KEYICON_DESCENDING_LIMIT,  -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_tutorial_yellow[] = {	// 黄色い鍵 pos, rot, scl, frame
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // 開始地点
	{ XMFLOAT3(420.0f, KEYICON_DESCENDING_LIMIT,  420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitKeyIcon(void)
{
	MakeVertexKeyIcon();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// 鍵アイコンの初期化
	for (int i = 0; i < MAX_KEYICON; i++)
	{
		ZeroMemory(&g_KeyIcon[i].material, sizeof(g_KeyIcon[i].material));
		g_KeyIcon[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_KeyIcon[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_KeyIcon[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_KeyIcon[i].fWidth = KEYICON_WIDTH;
		g_KeyIcon[i].fHeight = KEYICON_HEIGHT;
		g_KeyIcon[i].bUse = FALSE;
		g_KeyIcon[i].p_dist = 0.0f;		// プレイヤーまでの距離

		g_KeyIcon[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_KeyIcon[i].tbl_adr = NULL;	// 再生するアニメデータの先頭アドレスをセット
		g_KeyIcon[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

	}

	g_bAlpaTest = TRUE;

	// 鍵アイコンの位置の設定
	SetKeyIcon(XMFLOAT3(293.0f, KEYICON_OFFSET_Y, -240.0f), 10.0f, 10.0f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));	// 赤い鍵のアイコン
	SetKeyIcon(XMFLOAT3(244.0f, KEYICON_OFFSET_Y, 420.0f), 10.0f, 10.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));		// 青い鍵のアイコン
	SetKeyIcon(XMFLOAT3(-50.0f, KEYICON_OFFSET_Y, 243.0f), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));		// 黄色い鍵のアイコン


	// 線形補間で動かしてる
	if (GetMode() == MODE_GAME)
	{
		// 赤い鍵アイコン
		g_KeyIcon[0].move_time = 0.0f;															// 線形補間用のタイマーをクリア
		g_KeyIcon[0].tbl_adr = move_tbl_game_red;												// 再生するアニメデータの先頭アドレスをセット
		g_KeyIcon[0].tbl_size = sizeof(move_tbl_game_red) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット

		// 青い鍵アイコン
		g_KeyIcon[1].move_time = 0.0f;															// 線形補間用のタイマーをクリア
		g_KeyIcon[1].tbl_adr = move_tbl_game_blue;												// 再生するアニメデータの先頭アドレスをセット
		g_KeyIcon[1].tbl_size = sizeof(move_tbl_game_blue) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		// 黄色い鍵アイコン
		g_KeyIcon[2].move_time = 0.0f;															// 線形補間用のタイマーをクリア
		g_KeyIcon[2].tbl_adr = move_tbl_game_yellow;											// 再生するアニメデータの先頭アドレスをセット
		g_KeyIcon[2].tbl_size = sizeof(move_tbl_game_yellow) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	}
	else if (GetMode() == MODE_TUTORIAL)
	{
		// 赤い鍵アイコン
		g_KeyIcon[0].move_time = 0.0f;															// 線形補間用のタイマーをクリア
		g_KeyIcon[0].tbl_adr = move_tbl_tutorial_red;											// 再生するアニメデータの先頭アドレスをセット
		g_KeyIcon[0].tbl_size = sizeof(move_tbl_tutorial_red) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット

		// 青い鍵アイコン
		g_KeyIcon[1].move_time = 0.0f;															// 線形補間用のタイマーをクリア
		g_KeyIcon[1].tbl_adr = move_tbl_tutorial_blue;											// 再生するアニメデータの先頭アドレスをセット
		g_KeyIcon[1].tbl_size = sizeof(move_tbl_tutorial_blue) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

		// 黄色い鍵アイコン
		g_KeyIcon[2].move_time = 0.0f;															// 線形補間用のタイマーをクリア
		g_KeyIcon[2].tbl_adr = move_tbl_tutorial_yellow;										// 再生するアニメデータの先頭アドレスをセット
		g_KeyIcon[2].tbl_size = sizeof(move_tbl_tutorial_yellow) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitKeyIcon(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateKeyIcon(void)
{
	PLAYER *player = GetPlayer();						// プレイヤーのポインターを初期化
	KEY *key = GetKey();								// 鍵のポインターを初期化

	for (int i = 0; i < MAX_KEYICON; i++)
	{
		if (key[i].use == TRUE)
		{
			if (g_KeyIcon[i].tbl_adr != NULL)				// 線形補間を実行する？
			{												// 線形補間の処理
				// 移動処理
				int		index = (int)g_KeyIcon[i].move_time;
				float	time = g_KeyIcon[i].move_time - index;
				int		size = g_KeyIcon[i].tbl_size;

				float dt = 1.0f / g_KeyIcon[i].tbl_adr[index].frame;				// 1フレームで進める時間
				g_KeyIcon[i].move_time += dt;										// アニメーションの合計時間に足す

				if (index > (size - 2))												// ゴールをオーバーしていたら、最初へ戻す
				{
					g_KeyIcon[i].move_time = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				XMVECTOR p1 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 1].pos);	// 次の場所
				XMVECTOR p0 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 0].pos);	// 現在の場所
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_KeyIcon[i].pos, p0 + vec * time);

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				XMVECTOR r1 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 1].rot);	// 次の角度
				XMVECTOR r0 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 0].rot);	// 現在の角度
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_KeyIcon[i].rot, r0 + rot * time);

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				XMVECTOR s1 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 1].scl);	// 次のScale
				XMVECTOR s0 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 0].scl);	// 現在のScale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_KeyIcon[i].scl, s0 + scl * time);

			}

			// 鍵アイコンとプレイヤーの距離(ベクトル)を求める
			float dist;												// 距離保存用
			XMFLOAT3 PlayerPos = XMFLOAT3(player[0].pos.x, player[0].pos.y - 20, player[0].pos.z);
			XMVECTOR p = XMLoadFloat3(&PlayerPos);					// 計算用に一度XMVECTORに入れる
			XMVECTOR e = XMLoadFloat3(&g_KeyIcon[i].pos);
			XMVECTOR d = p - e;										// 距離を計算
			dist = VectorMagnitude(&d);								// エネミーとプレイヤーのベクトルの長さを保存

			g_KeyIcon[i].p_dist = dist;

			PrintDebugProc("プレイヤーからカギまでの距離:%f\n", g_KeyIcon[i].p_dist);

			if (GetMode() == MODE_GAME)
			{
				// 鍵アイコンの表示・非表示処理
				if (g_KeyIcon[i].p_dist < 170)	// 一定の距離以下で各カギのuseがTURE(未取得)なら
				{
					g_KeyIcon[i].bUse = TRUE;	// 表示する
				}
				else
				{
					g_KeyIcon[i].bUse = FALSE;	// 非表示
				}
			}
		}

	}

#ifdef _DEBUG
	// アルファテストON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE : TRUE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawKeyIcon(void)
{
	KEY *key = GetKey();	// 鍵のポインターを初期化

	// αテスト設定
	if (g_bAlpaTest == TRUE)
	{
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_KEYICON; i++)
	{
		if (key[i].use == TRUE && g_KeyIcon[i].bUse == TRUE) // キーが未取得で表示範囲内に入ったら
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_KeyIcon[i].scl.x, g_KeyIcon[i].scl.y, g_KeyIcon[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_KeyIcon[i].pos.x, g_KeyIcon[i].pos.y, g_KeyIcon[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_KeyIcon[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexKeyIcon(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 10.0f;
	float fHeight = 10.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 鍵アイコンのパラメータをセット
//=============================================================================
int SetKeyIcon(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxKeyIcon = -1;

	for (int i = 0; i < MAX_KEYICON; i++)
	{
		if (!g_KeyIcon[i].bUse)
		{
			g_KeyIcon[i].pos = pos;
			g_KeyIcon[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_KeyIcon[i].fWidth = fWidth;
			g_KeyIcon[i].fHeight = fHeight;
			g_KeyIcon[i].bUse = TRUE;
			g_KeyIcon[i].material.Diffuse = col;

			nIdxKeyIcon = i;

			break;
		}
	}

	return nIdxKeyIcon;
}

//=============================================================================
// 鍵アイコンの取得
//=============================================================================
KEYICON *GetKeyIcon()
{
	return &g_KeyIcon[0];
}
