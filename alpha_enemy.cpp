//=============================================================================
//
// 透明エネミーモデル処理 [alpha_enemy.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"
#include "debugproc.h"
#include "collision.h"
#include "coord_point.h"
#include "light.h"
#include "alpha_enemy.h"

#include "fade.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ALPHA_ENEMY			"data/MODEL/alpha_enemy.obj"	// 読み込むモデル名

#define ALPHA_ENEMY_SHADOW_SIZE			(0.4f)					// 影の大きさ
#define ALPHA_ENEMY_ANIME_SPEED			(0.05f)					// エネミーのアニメーション速度
#define ALPHA_ENEMY_RISE_LIMIT			(30.0f)					// エネミーのアニメーション上昇最大値
#define ALPHA_ENEMY_DESCENDING_LIMIT	(25.0f)					// エネミーのアニメーション下降最大値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ALPHA_ENEMY		g_AlphaEnemy[MAX_ALPHA_ENEMY];				// 透明のエネミー

static BOOL				g_Load = FALSE;


static float anime_speed;		// エネミーのアニメーション速度用変数

// リザルト画面用の線形補間データ
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-400.0f, ALPHA_ENEMY_OFFSET_Y,  425.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 }, // 開始地点
	{ XMFLOAT3(-175.0f, ALPHA_ENEMY_OFFSET_Y,  425.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 },
	{ XMFLOAT3(-175.0f, ALPHA_ENEMY_OFFSET_Y,  275.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(125.0f,  ALPHA_ENEMY_OFFSET_Y,  275.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(125.0f,  ALPHA_ENEMY_OFFSET_Y, -175.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(-225.0f, ALPHA_ENEMY_OFFSET_Y, -175.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(-225.0f, ALPHA_ENEMY_OFFSET_Y,  225.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 },
	{ XMFLOAT3(-400.0f, ALPHA_ENEMY_OFFSET_Y,  225.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 }, 
	{ XMFLOAT3(-400.0f, ALPHA_ENEMY_OFFSET_Y,  425.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 }, // 開始地点

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitAlphaEnemy(void)
{
	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		LoadModel(MODEL_ALPHA_ENEMY, &g_AlphaEnemy[i].model);
		g_AlphaEnemy[i].load = TRUE;

		g_AlphaEnemy[i].pos = XMFLOAT3(-375.0f, ALPHA_ENEMY_OFFSET_Y, 425.0f);
		g_AlphaEnemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_AlphaEnemy[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_AlphaEnemy[i].chase_mode = FALSE;			// TRUEなら追いかけるモードになる

		g_AlphaEnemy[i].angle = 0.0f;
		g_AlphaEnemy[i].p_dist = 0.0f;				// プレイヤーまでの距離


		anime_speed = ALPHA_ENEMY_ANIME_SPEED;		// エネミーのアニメーション速度用変数


		g_AlphaEnemy[i].spd = 0.0f;					// 移動スピードクリア
		g_AlphaEnemy[i].size = ALPHA_ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_AlphaEnemy[0].model, &g_AlphaEnemy[0].diffuse[0]);

		XMFLOAT3 pos = g_AlphaEnemy[i].pos;
		pos.y -= (ALPHA_ENEMY_OFFSET_Y - 0.1f);
		g_AlphaEnemy[i].shadowIdx = CreateShadow(pos, ALPHA_ENEMY_SHADOW_SIZE, ALPHA_ENEMY_SHADOW_SIZE);


		g_AlphaEnemy[i].move_time = 0.0f;			// 線形補間用のタイマーをクリア
		g_AlphaEnemy[i].tbl_adr = NULL;				// 再生するアニメデータの先頭アドレスをセット
		g_AlphaEnemy[i].tbl_size = 0;				// 再生するアニメデータのレコード数をセット

		g_AlphaEnemy[i].use = TRUE;					// TRUE:生きてる

	}

	// 線形補間で動かしてる
	if (GetMode() == MODE_GAME)
	{
		g_AlphaEnemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
		g_AlphaEnemy[0].tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
		g_AlphaEnemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitAlphaEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		if (g_AlphaEnemy[i].load)
		{
			UnloadModel(&g_AlphaEnemy[i].model);
			g_AlphaEnemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateAlphaEnemy(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		if (g_AlphaEnemy[i].use == TRUE)				// このエネミーが使われている？
		{												// Yes
			if (GetMode() == MODE_GAME)					// ゲームモードの時の動作
			{
				PrintDebugProc("透明:move_time: %f\n", g_AlphaEnemy[i].move_time);

				if (g_AlphaEnemy[i].tbl_adr != NULL)	// 線形補間を実行する？
				{										// 線形補間の処理
					// 移動処理
					int		index = (int)g_AlphaEnemy[i].move_time;
					float	time = g_AlphaEnemy[i].move_time - index;
					int		size = g_AlphaEnemy[i].tbl_size;

					float dt = 1.0f / g_AlphaEnemy[i].tbl_adr[index].frame;					// 1フレームで進める時間
					g_AlphaEnemy[i].move_time += dt;										// アニメーションの合計時間に足す

					if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
					{
						g_AlphaEnemy[i].move_time = 0.0f;
						index = 0;
					}

					// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
					XMVECTOR p1 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 1].pos);	// 次の場所
					XMVECTOR p0 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 0].pos);	// 現在の場所
					XMVECTOR vec = p1 - p0;
					XMStoreFloat3(&g_AlphaEnemy[i].pos, p0 + vec * time);

					// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
					XMVECTOR r1 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 1].rot);	// 次の角度
					XMVECTOR r0 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 0].rot);	// 現在の角度
					XMVECTOR rot = r1 - r0;
					XMStoreFloat3(&g_AlphaEnemy[i].rot, r0 + rot * time);

					// scaleを求める S = StartX + (EndX - StartX) * 今の時間
					XMVECTOR s1 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 1].scl);	// 次のScale
					XMVECTOR s0 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 0].scl);	// 現在のScale
					XMVECTOR scl = s1 - s0;
					XMStoreFloat3(&g_AlphaEnemy[i].scl, s0 + scl * time);
				}


				PLAYER *player = GetPlayer();
				// エネミーとプレイヤーの距離(ベクトル)を求める
				XMFLOAT3 dist;									// 距離保存用

				XMVECTOR p = XMLoadFloat3(&player[0].pos);		// 計算用に一度XMVECTORに入れる
				XMVECTOR e = XMLoadFloat3(&g_AlphaEnemy[i].pos);
				XMVECTOR d = p - e;								// 距離を計算
				XMStoreFloat3(&dist, d);						// distに戻す

				// エネミーが見ている方向
				XMFLOAT3 looking_direction = { sinf(g_AlphaEnemy[0].rot.y),0.0f,cosf(g_AlphaEnemy[0].rot.y) };//エネミーの見ている方向(ベクトル)

				// エネミーとプレイヤーのベクトルとエネミーが見ている方向のベクトルの内積計算
				float dot;										// 内積保存用

				XMVECTOR d1 = XMLoadFloat3(&dist);				// エネミーとプレイヤーのベクトル 1
				XMVECTOR l = XMLoadFloat3(&looking_direction);	// エネミーが見ているベクトル 2
				dot = dotProduct(&d, &l);						// 内積計算した結果を保存する

				float len = VectorMagnitude(&d1);				// エネミーとプレイヤーのベクトルの長さを保存
				float len2 = VectorMagnitude(&l);				// エネミーが見ているベクトルの長さを保存

				g_AlphaEnemy[i].p_dist = len;					// エネミーからプレイヤーまでの距離を保存



				// 上記２つのベクトルの角度の計算を行う
				float angle_cos = dot / (len*len2);				// 角度のcosを求める

				// 小数点を１と－１の間に固定する
				if (angle_cos > 1)
				{
					angle_cos = 1;
				}
				if (angle_cos < -1)
				{
					angle_cos = -1;
				}

				//エネミーのY回転が 6.28(-6.28)を超えたら0に戻す
				if (g_AlphaEnemy[i].rot.y > XM_2PI || g_AlphaEnemy[i].rot.y < -XM_2PI)
				{
					g_AlphaEnemy[i].rot.y = 0.0f;
				}


				// プレイヤーの方向にエネミーが回転する（向く処理）

				// プレイヤーとエネミーのベクトルの計算処理
				XMFLOAT3 relVec3 = { player[0].pos.x - g_AlphaEnemy[i].pos.x, 0.0f, player[0].pos.z - g_AlphaEnemy[i].pos.z };

				// ベクトルとZアクシス間の角度の計算処理 (ラジアン)
				float angle2 = atan2f(relVec3.z * -1, relVec3.x) * (360.0f / (XM_PI * 2.0f)); // ベクトルとZアクシスの角度を求める
				angle2 += -90.0f;								// -90°オフセットする
				float angleRadian = angle2 * XM_PI / 180.0f;	// 角度をラジアンに変換する

				g_AlphaEnemy[i].rot.y = angleRadian;			// 角度の結果をエネミーのRotのyに入れ込む

			}
		}

	}


#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("透明エネミーからプレイヤーまでの距離:%f\n", g_AlphaEnemy[0].p_dist);
	PrintDebugProc("透明Enemy_rot:X:%f Y:%f Z:%f\n", g_AlphaEnemy[0].rot.x, g_AlphaEnemy[0].rot.y, g_AlphaEnemy[0].rot.z);
	//PrintDebugProc("chase_mode:%d\n", g_AlphaEnemy[0].chase_mode);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAlphaEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		if (g_AlphaEnemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_AlphaEnemy[i].scl.x, g_AlphaEnemy[i].scl.y, g_AlphaEnemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_AlphaEnemy[i].rot.x, g_AlphaEnemy[i].rot.y + XM_PI, g_AlphaEnemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_AlphaEnemy[i].pos.x, g_AlphaEnemy[i].pos.y, g_AlphaEnemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_AlphaEnemy[i].mtxWorld, mtxWorld);

		// このエネミーは透明なのでDrawModelをコメントアウトしている
		// モデル描画
		//DrawModel(&g_AlphaEnemy[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// エネミーの取得
//=============================================================================
ALPHA_ENEMY *GetAlphaEnemy()
{
	return &g_AlphaEnemy[0];
}

