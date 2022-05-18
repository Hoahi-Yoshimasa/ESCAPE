//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "sound.h"
#include "ui.h"
#include "alpha_enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player.obj"		// 読み込むモデル名

#define	WALKING_SPEED		(0.9f)						// 歩く移動量
#define	RUN_SPEED			(2.0f)						// 走る移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)						// 影の大きさ
#define PLAYER_OFFSET_Y		(20.0f)						// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(1)							// プレイヤーのパーツの数
#define PLAYER_STAMINA_RECOVERY	(0.4f)					// プレイヤーのスタミナ回復量
#define PLAYER_STAMINA_RECOVERY_TIME	(120)			// プレイヤーのスタミナ回復時間
#define PLAYER_STAMINA_REDUNCTION		(0.6f)			// プレイヤーのスタミナ減少量 (回復量より大きい値を入れる)

#define DEFAULT_HEARTBEAT				(120)			// プレイヤーデフォルト心拍音

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static BOOL			g_Load = FALSE;

static int g_Walksound_count;			// 足音を均等な間隔で鳴らす用のカウント変数
static int g_StaminaRecovery_count;		// スタミナ回復時間調整

static int g_Text_count;				// チュートリアルテキスト用カウント変数

static int g_Heartbeat_count;			// 心拍音のカウント変数

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;
	g_Walksound_count = 0;									// 足音を均等な間隔で鳴らす用のカウント変数
	g_Heartbeat_count = DEFAULT_HEARTBEAT;					// 心拍音のカウント変数 初期化

	g_Player.pos = { -375.0f, PLAYER_OFFSET_Y, -375.0f };
	g_Player.old_pos = { 0.0f, 0.0f, 0.0f };				// 1フレーム前の座標を保存しておく用
	g_Player.rot = { 0.0f, XM_PI, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.stamina = PLAYER_STAMINA;						// スタミナの初期化
	g_Player.staminaPercent = 1.0f;							// HPを100％で初期化
	g_StaminaRecovery_count = 0;							// スタミナ回復時間調整

	g_Player.spd = 0.0f;									// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;							// 当たり判定の大きさ
	g_Player.keyNumber = 0;									// 鍵の取得数 0で初期化する

	g_Player.use = TRUE;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	g_Text_count = 6;	// チュートリアルのテキスト表示開始番号が6なので6で初期化している

	// チュートリアル時のプレイヤー座標
	if (GetMode() == MODE_TUTORIAL)
	{
		g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, -400.0f };
	}
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	UI * ui = GetUi();

	g_Player.old_pos = g_Player.pos;		// 当たり判定用で1フレーム前の座標を保存しておく

	// 生きている時のみ操作可能
	if (g_Player.use == TRUE)
	{
		// キーボード操作
		if (GetKeyboardPress(DIK_A))
		{	// 左へ移動
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.x += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (GetKeyboardPress(DIK_D))
		{	// 右へ移動
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z += sinf(g_Player.rot.y) * (g_Player.spd);
			g_Player.pos.x -= cosf(g_Player.rot.y) * (g_Player.spd);
		}

		if (GetKeyboardPress(DIK_W))
		{	// 上へ移動
			g_Player.spd = WALKING_SPEED;
			// Wキーを押しつつL_SHIFTを押したら移動量変化(ダッシュ)する
			if (GetKeyboardPress(DIK_LSHIFT) && g_Player.stamina > 0 && g_StaminaRecovery_count == 0)
			{
				g_Player.spd = RUN_SPEED;
				g_Player.stamina -= PLAYER_STAMINA_REDUNCTION;			// スタミナ減少量
			}
			g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
		}


		if (GetKeyboardPress(DIK_S))
		{	// 下へ移動
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.x += sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (GetKeyboardPress(DIK_RIGHT))
		{// 視点旋回「右」
			g_Player.rot.y += VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y > XM_PI)
			{
				g_Player.rot.y -= XM_PI * 2.0f;
			}
		}

		if (GetKeyboardPress(DIK_LEFT))
		{// 視点旋回「左」
			g_Player.rot.y -= VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y < -XM_PI)
			{
				g_Player.rot.y += XM_PI * 2.0f;
			}
		}


		// コントローラー操作
		if (IsButtonPressed(0, BUTTON_LEFT))
		{	// 左へ移動
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.x += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (IsButtonPressed(0, BUTTON_RIGHT))
		{	// 右へ移動
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z += sinf(g_Player.rot.y) * (g_Player.spd);
			g_Player.pos.x -= cosf(g_Player.rot.y) * (g_Player.spd);
		}

		if (IsButtonPressed(0, BUTTON_UP))
		{	// 上へ移動
			g_Player.spd = WALKING_SPEED;
			// Wキーを押しつつL_SHIFTを押したら移動量変化(ダッシュ)する
			if (IsButtonPressed(0, BUTTON_R) && g_Player.stamina > 0 && g_StaminaRecovery_count == 0)
			{
				g_Player.spd = RUN_SPEED;
				g_Player.stamina -= PLAYER_STAMINA_REDUNCTION;			// スタミナ減少量
			}
			g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
		}


		if (IsButtonPressed(0, BUTTON_DOWN))
		{	// 下へ移動
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.x += sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (IsButtonPressed(0, BUTTON_C) || IsButtonPressed(0, BUTTON_Z))
		{// 視点旋回「右」
			g_Player.rot.y += VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y > XM_PI)
			{
				g_Player.rot.y -= XM_PI * 2.0f;
			}
		}

		if (IsButtonPressed(0, BUTTON_A) || IsButtonPressed(0, BUTTON_Y))
		{// 視点旋回「左」
			g_Player.rot.y -= VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y < -XM_PI)
			{
				g_Player.rot.y += XM_PI * 2.0f;
			}
		}


		PrintDebugProc("Player.rot.y:%f\n", g_Player.rot.y);


		// チュートリアルのテキストを表示させる処理
		if (GetMode() == MODE_TUTORIAL)
		{
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_M))
			{
				g_Text_count++;
				if (g_Text_count > MAX_UI - 2)
				{
					g_Text_count = MAX_UI - 2;		// チュートリアル説明の最後の画像で固定させる
				}
				ui[g_Text_count - 1].use = FALSE;	// 一つ前のテキスト画像を消す
				ui[g_Text_count].use = TRUE;		// 現在のテキスト画像を表示する
			}
		}

		// スタミナが０になったら走れないようにするカウント処理
		if (g_Player.stamina < 0)
		{
			g_StaminaRecovery_count = PLAYER_STAMINA_RECOVERY_TIME;	// 回復時間
		}
		g_StaminaRecovery_count--;

		if (g_StaminaRecovery_count < 0)
		{
			g_StaminaRecovery_count = 0;
		}

		PrintDebugProc("スタミナ:%f\n", g_Player.stamina);

		// 立ち止まっているときは、プレイヤーの歩行スピードをWALKING_SPEEDに戻す
		if (g_Player.old_pos.x == g_Player.pos.x && g_Player.old_pos.z == g_Player.pos.z)
		{
			g_Player.spd = WALKING_SPEED;
		}

		// スタミナゲージの回復処理
		if (g_Player.spd == WALKING_SPEED)
		{
			if (g_StaminaRecovery_count > 0) // スタミナを使い切ると回復量が遅くなる
			{
				g_Player.stamina += PLAYER_STAMINA_RECOVERY / 2.0f;	// スタミナ回復量
			}
			else
			{
				g_Player.stamina += PLAYER_STAMINA_RECOVERY;		// スタミナ回復量
				// 最大値まで回復したらそれ以上回復しないようにする
				if (g_Player.stamina > PLAYER_STAMINA_MAX)
				{
					g_Player.stamina = PLAYER_STAMINA_MAX;
				}
			}
		}

		PrintDebugProc("スタミナカウント:%d\n", g_StaminaRecovery_count);
		PrintDebugProc("鍵の取得数:%d\n", g_Player.keyNumber);

		// 移動処理入力があったら足音を鳴らす処理
		if (g_Player.pos.x != g_Player.old_pos.x || g_Player.pos.z != g_Player.old_pos.z)
		{
			if (g_Walksound_count <= 0)
			{
				if (g_Player.spd == RUN_SPEED)
				{ // 走っているサウンドを鳴らす
					PlaySound(SOUND_LABEL_SE_RUN);
					g_Walksound_count = 35;
				}
				else if (g_Player.spd == WALKING_SPEED)
				{ // 歩いているサウンドを鳴らす
					PlaySound(SOUND_LABEL_SE_Footsteps);
					g_Walksound_count = 35;
				}
			}
			g_Walksound_count--;
		}

		PrintDebugProc("スピード:%f\n", g_Player.spd);
		PrintDebugProc("スピードカウント:%d\n", g_Walksound_count);

		// スタミナバーの処理
		g_Player.staminaPercent = g_Player.stamina / PLAYER_STAMINA_MAX;		// スタミナバーの為の処理


		// プレイヤーとエネミーに距離によって心拍音を(早くしたり・遅くしたり)変更させる

		ALPHA_ENEMY *alphaEnemy = GetAlphaEnemy();

		if (GetMode() == MODE_GAME)
		{

			// 透明のエネミーとプレイヤーの距離が350以上の時、一定間隔で心拍数を鳴らす
			if (alphaEnemy[0].p_dist > 350)
			{
				if (g_Heartbeat_count <= 0)
				{
					PlaySound(SOUND_LABEL_SE_HEARTBEAT);
					g_Heartbeat_count = DEFAULT_HEARTBEAT;
				}
			}

			// 透明のエネミーとプレイヤーの距離が350～200の時、60フレームに一回心拍音を鳴らす
			if (alphaEnemy[0].p_dist < 350 && alphaEnemy[0].p_dist > 200)
			{
				if (g_Heartbeat_count > 60)		// 心拍音が距離によって切り替わる時にラグが出ないように60カウント以上なら60カウントにする
				{
					g_Heartbeat_count = 60;
				}
				if (g_Heartbeat_count <= 0)
				{
					PlaySound(SOUND_LABEL_SE_HEARTBEAT);
					g_Heartbeat_count = 60;
				}
			}

			// 透明のエネミーとプレイヤーの距離が200以下の時、30フレームに一回心拍音を鳴らす
			if (alphaEnemy[0].p_dist < 200)	
			{
				if (g_Heartbeat_count > 30)		// 心拍音が距離によって切り替わる時にラグが出ないように30カウント以上なら30カウントにする
				{
					g_Heartbeat_count = 30;
				}
				if (g_Heartbeat_count <= 0)
				{
					PlaySound(SOUND_LABEL_SE_HEARTBEAT);
					g_Heartbeat_count = 30;
				}
			}
		}


		// チュートリアル時は一定の鼓動を鳴らす
		if (GetMode() == MODE_TUTORIAL)
		{
			if (g_Heartbeat_count <= 0)
			{
				PlaySound(SOUND_LABEL_SE_HEARTBEAT);
				g_Heartbeat_count = DEFAULT_HEARTBEAT;
			}
		}

		PrintDebugProc("心拍音のカウント:%d\n", g_Heartbeat_count);

		g_Heartbeat_count--;	// 心拍音をならすカウント用変数を毎フレーム引く


		// レイキャストして足元の高さを求める
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
		XMFLOAT3 hitPosition;								// 交点
		hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// 外れた時用に初期化しておく
		bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
		g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;

		{	// ポイントライト
			LIGHT *light = GetLightData(1);
			XMFLOAT3 pos = g_Player.pos;
			pos.y += 20.0f;

			light->Position = pos;
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Type = LIGHT_TYPE_POINT;
			light->Enable = TRUE;
			SetLightData(1, light);
		}


#ifdef _DEBUG	// デバッグ情報を表示する
		PrintDebugProc("Player:↑ → ↓ ←　Space\n");
		PrintDebugProc("Player:X:%f Y:%f Z:%f rot.y:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, g_Player.rot.y);
#endif

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング有効
	SetCullingMode(CULL_MODE_BACK);

	// ワイヤーフレームスイッチ有効
	SelectWireFrameMode();

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// クォータニオンを反映
	XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	SetFuchi(0);

	// モデル描画
	DrawModel(&g_Player.model);


	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// モデル描画
		DrawModel(&g_Parts[i].model);
	}

	SetFuchi(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	// ワイヤーフレーム設定を戻す
	SetWireFrameMode(WIRE_FRAME_MODE_NONE);

}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

//=============================================================================
// プレイヤーのスタミナ回復時間の情報を取得
//=============================================================================
int *GetStaminaRecovery_count()
{
	return &g_StaminaRecovery_count;
}