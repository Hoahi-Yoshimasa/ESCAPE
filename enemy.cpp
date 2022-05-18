//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"
#include "debugproc.h"
#include "collision.h"
#include "coord_point.h"
#include "light.h"

#include "fade.h"
#include "pathfind.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy.obj"		// 読み込むモデル名

#define	SEARCH_MOVE_SPEED			(0.80f)				// 探索モード時の移動量
#define	CHASE_MOVE_SPEED			(1.0f)				// チェイスモード時の移動量

#define ENEMY_SHADOW_SIZE			(0.4f)				// 影の大きさ
#define ENEMY_ANIME_SPEED			(0.05f)				// エネミーのアニメーション速度
#define ENEMY_RISE_LIMIT			(30.0f)				// エネミーのアニメーション上昇最大値
#define ENEMY_DESCENDING_LIMIT		(25.0f)				// エネミーのアニメーション下降最大値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー

static BOOL				g_Load = FALSE;


static float g_Angle;			// エネミーとプレイヤーとのベクトルとエネミーが見ているベクトルの角度保存用
static float anime_speed;		// エネミーのアニメーション速度用変数

static int g_NextPoint;			// 経路探索用 次のポイントを入れる変数

static BOOL g_Enemy_Move;		// 見える死神移動スイッチ デバッグ用

// リザルト画面用の線形補間データ
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame

	{ XMFLOAT3(180.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 5 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 3 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(XM_PIDIV4, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y, 200.0f),XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
};

// ゲームオーバー画面用の線形補間データ
static INTERPOLATION_DATA move_tbl2[] = {	// pos, rot, scl, frame

	{ XMFLOAT3(-50.0f, 0.0f, 0.0f),             XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 5 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 0.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 0.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 0.0f),   XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 60.0f),  XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // 回転
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y, 200.0f),XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(25.0f, ENEMY_OFFSET_Y, 425.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_Enemy[i].chase_mode = FALSE;	// TRUEなら追いかけるモードになる

		g_Enemy[i].angle = 0.0f;
		g_Enemy[i].p_dist = 0.0f;		// プレイヤーまでの距離


		anime_speed = ENEMY_ANIME_SPEED;	// エネミーのアニメーション速度用変数


		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);


		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].use = TRUE;			// TRUE:生きてる

		// タイトル画面・リザルト画面・ゲームオーバー画面のPOS
		if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
		{
			g_Enemy[i].pos = XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f);
		}
	}

	// 線形補間で動かしてる
	if (GetMode() == MODE_RESULT)	// リザルト用
	{
		g_Enemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
		g_Enemy[0].tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	}

	if (GetMode() == MODE_GAMEOVER)	// ゲームオーバー画面用
	{
		g_Enemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
		g_Enemy[0].tbl_adr = move_tbl2;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[0].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	}

	g_Enemy_Move = FALSE;

	g_Angle = 0;
	g_NextPoint = 0;			// 経路探索用 次のポイントを入れる変数 0で初期化

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			if (GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)	// リザルト画面とゲームオーバー画面時のみ線形補間
			{
				PrintDebugProc("enemy:move_time: %f\n", g_Enemy[i].move_time);

				if (g_Enemy[i].tbl_adr != NULL)	// 線形補間を実行する？
				{								// 線形補間の処理
					// 移動処理
					int		index = (int)g_Enemy[i].move_time;
					float	time = g_Enemy[i].move_time - index;
					int		size = g_Enemy[i].tbl_size;

					float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1フレームで進める時間
					g_Enemy[i].move_time += dt;							// アニメーションの合計時間に足す

					if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
					{
						g_Enemy[i].move_time = 0.0f;
						index = 0;
					}

					// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
					XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// 次の場所
					XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// 現在の場所
					XMVECTOR vec = p1 - p0;
					XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

					// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
					XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// 次の角度
					XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// 現在の角度
					XMVECTOR rot = r1 - r0;
					XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

					// scaleを求める S = StartX + (EndX - StartX) * 今の時間
					XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// 次のScale
					XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// 現在のScale
					XMVECTOR scl = s1 - s0;
					XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

				}

			}

			PLAYER *player = GetPlayer();

			if (GetMode() == MODE_GAME)							// ゲームモードの時の動作
			{
				// エネミーとプレイヤーの距離(ベクトル)を求める
				XMFLOAT3 dist;									// 距離保存用

				XMVECTOR p = XMLoadFloat3(&player[0].pos);		// 計算用に一度XMVECTORに入れる
				XMVECTOR e = XMLoadFloat3(&g_Enemy[i].pos);
				XMVECTOR d = p - e;								// 距離を計算
				XMStoreFloat3(&dist, d);						// distに戻す

				// エネミーが見ている方向
				XMFLOAT3 looking_direction = { sinf(g_Enemy[0].rot.y),0.0f,cosf(g_Enemy[0].rot.y) };//エネミーの見ている方向(ベクトル)


				// エネミーとプレイヤーのベクトルとエネミーが見ている方向のベクトルの内積計算
				float dot;										// 内積保存用

				XMVECTOR d1 = XMLoadFloat3(&dist);				// エネミーとプレイヤーのベクトル 1
				XMVECTOR l = XMLoadFloat3(&looking_direction);	// エネミーが見ているベクトル 2
				dot = dotProduct(&d, &l);						// 内積計算した結果を保存する

				float len = VectorMagnitude(&d1);				// エネミーとプレイヤーのベクトルの長さを保存
				float len2 = VectorMagnitude(&l);				// エネミーが見ているベクトルの長さを保存

				g_Enemy[i].p_dist = len;						// エネミーからプレイヤーまでの距離を保存


				// 上記２つのベクトルの角度の計算を行う

				float angle_cos = dot / (len*len2); // 角度のcosを求める

				// 小数点を１と－１の間に固定する
				if (angle_cos > 1)
				{
					angle_cos = 1;
				}
				if (angle_cos < -1)
				{
					angle_cos = -1;
				}

				g_Angle = acosf(angle_cos); // 角度の計算を行う


				float radian = XMConvertToRadians(30);			// エネミーのサーチ範囲30°をラジアンに変換

				if (g_Angle > XM_PI - radian && len < 200.0f)	// エネミーが見ている方向を基準に左右角度30度以内に入ったら かつ距離が200以内なら
				{
					g_Enemy[i].chase_mode = TRUE;				// チェイスモードがON(TRUE)になる
					g_Enemy[i].spd = CHASE_MOVE_SPEED;			// チェイスモード用のスピード設定
				}
				else
				{
					g_Enemy[i].chase_mode = FALSE;				// チェイスモードがOFF(FALSE)になる
					g_Enemy[i].spd = SEARCH_MOVE_SPEED;			// 通常モード用のスピード設定
				}


				//エネミーのY回転が 6.28(-6.28)を超えたら0に戻す
				if (g_Enemy[i].rot.y > XM_2PI || g_Enemy[i].rot.y < -XM_2PI)
				{
					g_Enemy[i].rot.y = 0.0f;
				}

#ifdef _DEBUG
				// デバッグ用エネミー移動スイッチ
				g_Enemy_Move = g_Enemy_Move % 2;

				if (GetKeyboardTrigger(DIK_1))
				{
					g_Enemy_Move++;
				}
				PrintDebugProc("エネミーMOVE:%d\n", g_Enemy_Move);
#endif

				int *P_chip_num = GetPlayer_chip_num();			// 現在プレイヤーがメッシュフィールドの何処(何番目)にいるかをGetする
				int *E_chip_num = GetEnemy_chip_num();

				if (g_Enemy_Move == FALSE)							// エネミー移動スイッチONなら
				{
					if (g_Enemy[i].chase_mode == TRUE)				// チェイスモードならプレイヤーを直接追いかける
					{
						// プレイヤーの方向にエネミーが回転する（向く処理）

						// プレイヤーとエネミーのベクトルの計算処理
						XMFLOAT3 relVec3 = { player[0].pos.x - g_Enemy[i].pos.x, 0.0f, player[0].pos.z - g_Enemy[i].pos.z };

						// ベクトルとZアクシス間の角度の計算処理 (ラジアン)

						float angle2 = atan2f(relVec3.z * -1, relVec3.x) * (360.0f / (XM_PI * 2.0f));	// ベクトルとZアクシスの角度を求める
						angle2 += -90.0f;																// -90°オフセットする
						float angleRadian = angle2 * XM_PI / 180.0f;									// 角度をラジアンに変換する
						g_Enemy[i].rot.y = angleRadian;													// 角度の結果をエネミーのRotのyに入れ込む


						// プレイヤーのところに移動する処理
						XMFLOAT3 difference;		// 差分計算用
						XMVECTOR P_pos = XMLoadFloat3(&player[0].pos);									// 一度計算用で変換する
						XMVECTOR E_pos = XMLoadFloat3(&g_Enemy[i].pos);
						XMVECTOR difference1 = XMLoadFloat3(&difference);

						difference1 = P_pos - E_pos;													// 自分とプレイヤーとの差分を求めている
						XMStoreFloat3(&difference, difference1);										// 結果を一度保存する(戻す)
						float angle1 = atan2f(difference.z, difference.x);								// その差分を使って角度を求めている

						g_Enemy[i].pos.x += cosf(angle1) * g_Enemy[i].spd;								// angleの方向へ移動
						g_Enemy[i].pos.z += sinf(angle1) * g_Enemy[i].spd;								// angleの方向へ移動
					}
					else																				// チェイスモード以外の場合 (経路探索をする)
					{
						// 最短経路探索処理
						COORD_POINT *coord_point = GetCoordPoint();										// 各メッシュフィールドの中心座標座標を取得
						g_NextPoint = ShortestPath(*E_chip_num, *P_chip_num);							// 最短経路 次の場所を返す


						// NextPos方向にエネミーが回転する（向く処理）

						// NextPosとエネミーのベクトルの計算処理
						XMFLOAT3 relVec3 = { coord_point[g_NextPoint].pos.x - coord_point[*E_chip_num].pos.x, 0.0f, coord_point[g_NextPoint].pos.z - coord_point[*E_chip_num].pos.z };

						// ベクトルとZアクシス間の角度の計算処理 (ラジアン)
						float angle2 = atan2f(relVec3.z * -1, relVec3.x) * (360.0f / (XM_PI * 2.0f));	// ベクトルとZアクシスの角度を求める
						angle2 += -90.0f;																// -90°オフセットする
						float angleRadian = angle2 * XM_PI / 180.0f;									// 角度をラジアンに変換する
						g_Enemy[i].rot.y = angleRadian;													// 角度の結果をエネミーのRotのyに入れ込む


						// NextPointに移動する処理
						XMFLOAT3 difference;															// 差分計算用
						XMVECTOR Next_pos = XMLoadFloat3(&coord_point[g_NextPoint].pos);				// 一度計算用で変換する
						XMVECTOR E_pos = XMLoadFloat3(&g_Enemy[i].pos);
						XMVECTOR difference1 = XMLoadFloat3(&difference);

						difference1 = Next_pos - E_pos;													// エネミーとNextポイントとの差分を求めている
						XMStoreFloat3(&difference, difference1);										// 結果を一度保存する(戻す)

						float angle1 = atan2f(difference.z, difference.x);								// その差分を使って角度を求めている

						g_Enemy[i].pos.x += cosf(angle1) * g_Enemy[i].spd;								// angleの方向へ移動
						g_Enemy[i].pos.z += sinf(angle1) * g_Enemy[i].spd;								// angleの方向へ移動
					}
				}
			}

			// エネミーが上下にアニメーションする処理
			g_Enemy[i].pos.y += anime_speed;
			if (g_Enemy[i].pos.y > ENEMY_RISE_LIMIT || g_Enemy[i].pos.y < ENEMY_DESCENDING_LIMIT)
			{
				anime_speed *= -1;	// 上下反転処理
			}

			{	// ポイントライト
				LIGHT *light = GetLightData(1);
				XMFLOAT3 pos = g_Enemy[i].pos;
				pos.y += 0.0f;

				light->Position = pos;
				light->Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
				light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Type = LIGHT_TYPE_POINT;
				light->Enable = TRUE;
				SetLightData(1, light);
			}

		}

	}


#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("PとEの角度:%f\n", g_Angle);
	PrintDebugProc("Enemy:X:%f Y:%f Z:%f rot.y:%f\n", g_Enemy[0].pos.x, g_Enemy[0].pos.y, g_Enemy[0].pos.z, g_Enemy[0].rot);
	PrintDebugProc("エネミーからプレイヤーまでの距離:%f\n", g_Enemy[0].p_dist);
	PrintDebugProc("Enemy_rot:X:%f Y:%f Z:%f\n", g_Enemy[0].rot.x, g_Enemy[0].rot.y, g_Enemy[0].rot.z);
	PrintDebugProc("chase_mode:%d\n", g_Enemy[0].chase_mode);
	PrintDebugProc("最短経路 NEXTポイント:%d\n", g_NextPoint);


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワイヤーフレームスイッチ有効
	SelectWireFrameMode();

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	// ワイヤーフレーム設定を戻す
	SetWireFrameMode(WIRE_FRAME_MODE_NONE);

}


//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

