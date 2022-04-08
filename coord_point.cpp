//=============================================================================
//
// 座標ポイントの処理 [Coord_Point.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "sound.h"
#include "Coord_Point.h"
#include "meshfield.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(1)				// テクスチャの数

#define	COORDPOINT_WIDTH		(3.0f)			// 頂点サイズ
#define	COORDPOINT_HEIGHT		(3.0f)			// 頂点サイズ

#define	COORDPOINT_OFFSET		(0.5f)			// 座標表示ポイントを少し上げる

#define	MESHFIELD_BLOCK_MAX	(MESHFIELD_BlockX * MESHFIELD_BlockZ)	// メッシュフィールドの全ブロック数


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexCoordPoint(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static COORD_POINT					g_CoordPoint[MESHFIELD_BLOCK_MAX];
static int							g_TexNo;				// テクスチャ番号

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/Point.png",
};

static BOOL							g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCoordPoint(void)
{
	MakeVertexCoordPoint();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// 座標ポイントの初期化 全メッシュフィールドブロック数初期化する
	for (int nCntPoint = 0; nCntPoint < MESHFIELD_BlockX * MESHFIELD_BlockZ; nCntPoint++)
	{
		ZeroMemory(&g_CoordPoint[nCntPoint].material, sizeof(g_CoordPoint[nCntPoint].material));
		g_CoordPoint[nCntPoint].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_CoordPoint[nCntPoint].pos = { 0.0f, 1.0f, 0.0f };
		g_CoordPoint[nCntPoint].rot = { 0.0f, 0.0f, 0.0f };
		g_CoordPoint[nCntPoint].scl = { 1.0f, 1.0f, 1.0f };
		g_CoordPoint[nCntPoint].fWidth = COORDPOINT_WIDTH;
		g_CoordPoint[nCntPoint].fHeight = COORDPOINT_HEIGHT;
		g_CoordPoint[nCntPoint].use = FALSE;
		g_CoordPoint[nCntPoint].move = 0; // 全メッシュフィールド分初期化する 行けない場所はKEEP_OUT(-1)で初期化する

		// メッシュフィールド外周の当たり判定(メッシュウォール)
		if (0 == nCntPoint % MESHFIELD_BlockX || MESHFIELD_BlockX -1 == nCntPoint % MESHFIELD_BlockX)
		{
			g_CoordPoint[nCntPoint].move = KEEP_OUT; // 行けない場所(プレイヤーの当たり判定場所)
		}
		if (nCntPoint < MESHFIELD_BlockX)
		{
			g_CoordPoint[nCntPoint].move = KEEP_OUT;
		}
		if (nCntPoint >= MESHFIELD_BLOCK_MAX - MESHFIELD_BlockX)
		{
			g_CoordPoint[nCntPoint].move = KEEP_OUT;
		}
		
	}

	// モードゲームの時の当たり判定
	if (GetMode() == MODE_GAME)
	{
		// MAPの当たり判定の箇所を個別に設定する
		g_CoordPoint[26].move = KEEP_OUT;
		g_CoordPoint[30].move = KEEP_OUT;
		g_CoordPoint[31].move = KEEP_OUT;
		g_CoordPoint[46].move = KEEP_OUT;
		g_CoordPoint[50].move = KEEP_OUT;
		g_CoordPoint[51].move = KEEP_OUT;
		g_CoordPoint[74].move = KEEP_OUT;
		g_CoordPoint[75].move = KEEP_OUT;
		g_CoordPoint[83].move = KEEP_OUT;
		g_CoordPoint[84].move = KEEP_OUT;
		g_CoordPoint[94].move = KEEP_OUT;
		g_CoordPoint[95].move = KEEP_OUT;
		g_CoordPoint[103].move = KEEP_OUT;
		g_CoordPoint[104].move = KEEP_OUT;
		g_CoordPoint[114].move = KEEP_OUT;
		g_CoordPoint[115].move = KEEP_OUT;
		g_CoordPoint[134].move = KEEP_OUT;
		g_CoordPoint[135].move = KEEP_OUT;
		g_CoordPoint[150].move = KEEP_OUT;
		g_CoordPoint[151].move = KEEP_OUT;
		g_CoordPoint[162].move = KEEP_OUT;
		g_CoordPoint[163].move = KEEP_OUT;
		g_CoordPoint[166].move = KEEP_OUT;
		g_CoordPoint[170].move = KEEP_OUT;
		g_CoordPoint[171].move = KEEP_OUT;
		g_CoordPoint[182].move = KEEP_OUT;
		g_CoordPoint[183].move = KEEP_OUT;
		g_CoordPoint[186].move = KEEP_OUT;
		g_CoordPoint[202].move = KEEP_OUT;
		g_CoordPoint[203].move = KEEP_OUT;
		g_CoordPoint[214].move = KEEP_OUT;
		g_CoordPoint[215].move = KEEP_OUT;
		g_CoordPoint[222].move = KEEP_OUT;
		g_CoordPoint[223].move = KEEP_OUT;
		g_CoordPoint[234].move = KEEP_OUT;
		g_CoordPoint[235].move = KEEP_OUT;
		g_CoordPoint[261].move = KEEP_OUT;
		g_CoordPoint[262].move = KEEP_OUT;
		g_CoordPoint[267].move = KEEP_OUT;
		g_CoordPoint[268].move = KEEP_OUT;
		g_CoordPoint[269].move = KEEP_OUT;
		g_CoordPoint[270].move = KEEP_OUT;
		g_CoordPoint[287].move = KEEP_OUT;
		g_CoordPoint[288].move = KEEP_OUT;
		g_CoordPoint[289].move = KEEP_OUT;
		g_CoordPoint[290].move = KEEP_OUT;
		g_CoordPoint[303].move = KEEP_OUT;
		g_CoordPoint[304].move = KEEP_OUT;
		g_CoordPoint[313].move = KEEP_OUT;
		g_CoordPoint[314].move = KEEP_OUT;
		g_CoordPoint[317].move = KEEP_OUT;
		g_CoordPoint[318].move = KEEP_OUT;
		g_CoordPoint[323].move = KEEP_OUT;
		g_CoordPoint[324].move = KEEP_OUT;
		g_CoordPoint[333].move = KEEP_OUT;
		g_CoordPoint[334].move = KEEP_OUT;
		g_CoordPoint[353].move = KEEP_OUT;
		g_CoordPoint[354].move = KEEP_OUT;
		g_CoordPoint[373].move = KEEP_OUT;
		g_CoordPoint[374].move = KEEP_OUT;

	}

	XMFLOAT3 *coord = GetCoord();
	// 各メッシュフィールド中心座標を保存する 経路探索の為
	for (int i = 0; i < MESHFIELD_BLOCK_MAX; i++) // 全ブロック数のポイントをセット
	{
		coord[i].y = COORDPOINT_OFFSET; // 少し座標を上げる
		SetCoordPoint(coord[i]);

		// move がKEEP_OUT(-1)になっているところを赤い点にする
		if (g_CoordPoint[i].move == KEEP_OUT)
		{
			g_CoordPoint[i].material.Diffuse = { 1.0f, 0.0f, 0.0f, 1.0f };
		}
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCoordPoint(void)
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
void UpdateCoordPoint(void)
{


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCoordPoint(void)
{
	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MESHFIELD_BLOCK_MAX; i++)
	{
		if (g_CoordPoint[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_CoordPoint[i].scl.x, g_CoordPoint[i].scl.y, g_CoordPoint[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_CoordPoint[i].rot.x, g_CoordPoint[i].rot.y + XM_PI, g_CoordPoint[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_CoordPoint[i].pos.x, g_CoordPoint[i].pos.y, g_CoordPoint[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_CoordPoint[i].mtxWorld, mtxWorld);


			// マテリアル設定
			SetMaterial(g_CoordPoint[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexCoordPoint(void)
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

	float fWidth = COORDPOINT_WIDTH;
	float fHeight = COORDPOINT_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = { fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = { fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 座標ポイントをセット
//=============================================================================
int SetCoordPoint(XMFLOAT3 pos)
{
	int nIdxCoordPoint = KEEP_OUT;

	for (int nCntCoordPoint = 0; nCntCoordPoint < MESHFIELD_BLOCK_MAX; nCntCoordPoint++)
	{
		if (!g_CoordPoint[nCntCoordPoint].use)
		{
			g_CoordPoint[nCntCoordPoint].pos = pos;
			g_CoordPoint[nCntCoordPoint].rot = { 0.0f, 0.0f, 0.0f };
			g_CoordPoint[nCntCoordPoint].use = TRUE;

			nIdxCoordPoint = nCntCoordPoint;

			break;
		}
	}

	return nIdxCoordPoint;
}

//=============================================================================
// 座標ポイントの先頭アドレス取得
//=============================================================================
COORD_POINT *GetCoordPoint(void)
{
	return &(g_CoordPoint[0]);
}
