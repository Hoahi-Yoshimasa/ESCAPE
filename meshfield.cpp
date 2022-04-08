//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"
#include "player.h"
#include "enemy.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(2)				// テクスチャの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11Buffer					*g_IndexBuffer = NULL;	// インデックスバッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号

static XMFLOAT3		g_posField;								// ポリゴン表示位置の中心座標
static XMFLOAT3		g_rotField;								// ポリゴンの回転角

static int			g_nNumBlockXField, g_nNumBlockZField;	// ブロック数
static int			g_nNumVertexField;						// 総頂点数	
static int			g_nNumVertexIndexField;					// 総インデックス数
static int			g_nNumPolygonField;						// 総ポリゴン数
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// ブロックサイズ


static char* g_TextureName[] = {
	"data/TEXTURE/field_test.jpg",
	"data/TEXTURE/field.png",
};


// 波のように動かす処理

static VERTEX_3D	*g_Vertex = NULL;

// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
static XMFLOAT3		g_Center;					// 波の発生場所
static float		g_Time = 0.0f;				// 波の経過時間
static float		g_wave_frequency = 2.0f;	// 波の周波数
static float		g_wave_correction = 0.02f;	// 波の距離補正
static float		g_wave_amplitude = 20.0f;	// 波の振幅


static 	XMFLOAT3 g_Coord[MESHFIELD_BlockZ][MESHFIELD_BlockX]; // 全メッシュフィールド分の２次元配列

static BOOL			g_Load = FALSE;

// ２次元配列のメッシュフィールドの何処にいるかを調べる変数
static int g_Enemy_chip_num;
static int g_Player_chip_num;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	MakeVertexPoint();

	// ポリゴン表示位置の中心座標を設定
	g_posField = pos;

	g_rotField = rot;

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

	// ブロック数の設定
	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;


	// 各メッシュフィールド中心座標を保存する 中心座標が欲しいので 経路探索の為
	for (int i = 0; i < g_nNumBlockZField; i++) // 縦のブロック数
	{
		for (int j = 0; j < g_nNumBlockXField; j++) // 横のブロック数
		{
			//2次元配列に各メッシュフィールドのPOS情報を入れる
			g_Coord[i][j] = XMFLOAT3(j * fBlockSizeX - g_nNumBlockXField * fBlockSizeX/2 + fBlockSizeX/2, 0, i * fBlockSizeZ - g_nNumBlockZField * fBlockSizeZ / 2 + fBlockSizeZ/2);
		}
	}



	// 頂点数の設定
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// ポリゴン数の設定
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// ブロックサイズの設定
	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;


	// ２次元配列のメッシュフィールドの何処にいるかを調べる変数 初期化
	int g_Enemy_chip_num = 0;  //エネミーの座標ポイントを調べる用
	int g_Player_chip_num = 0; //プレイヤーの座標ポイントを調べる用



	// 頂点情報をメモリに作っておく（波の為）
	// 波の処理
	// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 波の発生場所
	g_Time = 0.0f;								// 波の経過時間(＋とーとで内側外側になる)
	g_wave_frequency = 1.0f;					// 波の周波数（上下運動の速さ）
	g_wave_correction = 0.02f;					// 波の距離補正（変えなくても良いと思う）
	g_wave_amplitude = 30.0f;					// 波の振幅(波の高さ)

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// 波紋の中心点からの距離を得る
			float len = (float)sqrt(dx * dx + dz * dz);

			// 波の高さを、sin関数で得る
			// 波の高さ　= sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;

			// 法線の設定
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// 反射光の設定
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			float texSizeX = 1.0f;
			float texSizeZ = 1.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}

	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//頂点バッファの中身を埋める

		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める

		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
				nCntIdx++;
			}

			for (int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if (nCntVtxZ < (g_nNumBlockZField - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	if (g_Load == FALSE) return;

	// インデックスバッファの解放
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{

	// return;	// 処理をスキップ！

	// 波の処理
	float dt = 0.03f;

	for (int z = 0; z < g_nNumBlockZField; z++)
	{
		for (int x = 0; x < g_nNumBlockXField; x++)
		{
			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// 波紋の中心点からの距離を得る
			float len = (float)sqrt(dx * dx + dz * dz);

			// 波の高さを、sin関数で得る
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			//g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
		}

	}
	g_Time += dt;


	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// 全頂点情報を毎回上書きしているのはDX11ではこの方が早いからです
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);



	// プレイヤーの位置情報からメッシュフィールドの何処にいるかを計算
	PLAYER *player = GetPlayer();

	int player_Xchip_num = 0;
	int player_Zchip_num = 0;

	// メッシュフィールドX座標の配列何番目にいるかを計算
	player_Xchip_num = ((int)player[0].pos.x / (int)MESHFIELD_WIDTH);
	player_Xchip_num += MESHFIELD_BlockX / 2;

	if (player[0].pos.x >= 0.0f)
	{
		player_Xchip_num++;
	}
	player_Xchip_num -= 1; // 2次元配列先頭に合わせるために-1している

	PrintDebugProc("PメッシュのX番目:%d\n", player_Xchip_num);

	// メッシュフィールドZ座標の配列何番目にいるかを計算
	player_Zchip_num = ((int)player[0].pos.z / (int)MESHFIELD_HEIGHT);
	player_Zchip_num += MESHFIELD_BlockZ / 2;

	if (player[0].pos.z >= 0.0f)
	{
		player_Zchip_num++;
	}
	player_Zchip_num -= 1; // 2次元配列先頭に合わせるために-1している

	PrintDebugProc("PメッシュのZ番目:%d\n", player_Zchip_num);

	g_Player_chip_num = (player_Zchip_num * MESHFIELD_BlockZ) + player_Xchip_num; // Getで一次元配列を返す用に要素番号として合わせている
	PrintDebugProc("Pメッシュの番目:%d\n", g_Player_chip_num);



	// エネミーの位置情報からメッシュフィールドの何処にいるかを計算
	ENEMY *enemy = GetEnemy();

	int enemy_Xchip_num = 0;
	int enemy_Zchip_num = 0;

	// メッシュフィールドX座標の配列何番目にいるかを計算
	enemy_Xchip_num = ((int)enemy[0].pos.x / (int)MESHFIELD_WIDTH);
	enemy_Xchip_num += MESHFIELD_BlockX / 2;

	if (enemy[0].pos.x >= 0.0f)
	{
		enemy_Xchip_num++;
	}
	enemy_Xchip_num -= 1; // 2次元配列先頭に合わせるために-1している

	PrintDebugProc("EメッシュのX番目:%d\n", enemy_Xchip_num);

	// メッシュフィールドZ座標の配列何番目にいるかを計算
	enemy_Zchip_num = ((int)enemy[0].pos.z / (int)MESHFIELD_HEIGHT);
	enemy_Zchip_num += MESHFIELD_BlockZ / 2;

	if (enemy[0].pos.z >= 0.0f)
	{
		enemy_Zchip_num++;
	}
	enemy_Zchip_num -= 1; // 2次元配列先頭に合わせるために-1している

	PrintDebugProc("EメッシュのZ番目:%d\n", enemy_Zchip_num);

	g_Enemy_chip_num = (enemy_Zchip_num * MESHFIELD_BlockZ) + enemy_Xchip_num; // Getで一次元配列を返す用に要素番号として合わせている
	PrintDebugProc("Eメッシュの番目:%d\n", g_Enemy_chip_num);

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);


	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

#ifdef _DEBUG  // デバッグ用
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
#endif

	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);


	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}

//                           ぶつかったポリゴン場所 ぶつかったポリゴンの向き
BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;
	XMFLOAT3 org = *HitPosition;

	// 少し上から、ズドーンと下へレイを飛ばす
	start.y += 1000.0f;
	end.y -= 1000.0f;

	// 処理を高速化する為に全検索ではなくて、座標からポリゴンを割り出すと
	float fz = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	float fx = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;
	int sz = (int)((-start.z + fz) / g_fBlockSizeZField);
	int sx = (int)((start.x + fx) / g_fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;

	if ((sz < 0) || (sz > g_nNumBlockZField - 1) ||
		(sx < 0) || (sx > g_nNumBlockXField - 1))
	{
		*Normal = { 0.0f, 1.0f, 0.0f };
		return FALSE;
	}


	// 必要数分検索を繰り返す
	for (int z = sz; z < ez; z++) // Z軸のマス　当たり判定
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXField + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + (x + 1)].Position;

			// 三角ポリゴンだから２枚分の当たり判定
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}

	// 何処にも当たっていなかった時
	*HitPosition = org;
	return FALSE;
}

//=============================================================================
// ポイントテクスチャの頂点情報の作成
//=============================================================================
HRESULT MakeVertexPoint(void)
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

	float fWidth = 10;
	float fHeight = 10;

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
// 各メッシュフィールドの座標 先頭アドレスを取得
//=============================================================================
XMFLOAT3 *GetCoord()
{
	return &g_Coord[0][0];
}

//=============================================================================
// エネミーが現在メッシュフィールドの座標何番目にいるかを要素番号で返す
//=============================================================================
int *GetEnemy_chip_num()
{
	return &g_Enemy_chip_num;
}


//=============================================================================
// プレイヤーが現在メッシュフィールドの座標何番目にいるかを要素番号で返す
//=============================================================================
int *GetPlayer_chip_num()
{
	return &g_Player_chip_num;
}
