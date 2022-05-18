//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "collision.h"
#include "debugproc.h"
#include "fence.h"
#include "stamina_bar.h"
#include "ui.h"
#include "gate.h"
#include "item_key.h"
#include "time.h"
#include "rock.h"
#include "hut.h"
#include "shadow_renderer.h"
#include "Coord_Point.h"
#include "effect.h"
#include "alpha_enemy.h"
#include "key_icon.h"

#include "nodequeue.h"
#include "pathfind.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************

static BOOL	g_bPause = TRUE;	// ポーズON/OFF

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{

	// タイトル画面・リザルト画面・ゲームオーバー画面の為の初期化処理
	if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		// フィールドの初期化
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), MESHFIELD_BlockX, MESHFIELD_BlockZ, MESHFIELD_WIDTH, MESHFIELD_HEIGHT);

		// ライトを有効化	// 影の初期化処理
		InitShadow();

		// エネミーの初期化
		InitEnemy();

		// 壁の初期化
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);

		// 小屋の初期化
		InitHut();

	}

	if (GetMode() == MODE_GAME)
	{
		// フィールドの初期化
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), MESHFIELD_BlockX, MESHFIELD_BlockZ, MESHFIELD_WIDTH, MESHFIELD_HEIGHT); // 13

		// 座標ポイント表示の初期化
		InitCoordPoint();

		// ライトを有効化	// 影の初期化処理
		InitShadow();

		// プレイヤーの初期化
		InitPlayer();

		// エネミーの初期化
		InitEnemy();

		// 経路探索処理の初期化
		InitPathfind();

		// 透明のエネミーの初期化処理
		InitAlphaEnemy();

		// 壁の初期化
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);


		// フェンスの初期化
		InitFence();

		// 岩の初期化
		InitRock();

		// 小屋の初期化
		InitHut();

		// ゲートの初期化
		InitGate();

		// 鍵の初期化
		InitKey();

		// タイムの初期化
		InitTime();

		// スタミナゲージの初期化
		InitStaminaBar();

		// UIの初期化
		InitUi();

		// 鍵アイコンの初期化
		InitKeyIcon();

		// エフェクトの初期化
		InitEffect();

		// BGM再生
		PlaySound(SOUND_LABEL_BGM_GAME);

	}

	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{

	// エフェクトの終了処理
	UninitEffect();

	// 鍵アイコンの終了処理
	UninitKeyIcon();

	// UIの終了処理
	UninitUi();

	// スタミナゲージの終了処理
	UninitStaminaBar();

	// タイムの終了処理
	UninitTime();

	// ゲートの終了処理
	UninitGate();

	// 小屋の終了処理
	UninitHut();

	// 鍵の終了処理
	UninitKey();

	// 岩の終了処理
	UninitRock();

	// フェンスの終了処理
	UninitFence();

	// 壁の終了処理
	UninitMeshWall();

	// 座標ポイント表示の終了処理
	UninitCoordPoint();

	// 地面の終了処理
	UninitMeshField();

	// 透明のエネミーの終了処理
	UninitAlphaEnemy();

	// 経路探索処理の終了処理
	UninitPathfind();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{

	if (g_bPause == FALSE)
		return;

	if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		// 地面処理の更新
		UpdateMeshField();

		// エネミーの更新処理
		UpdateEnemy();

		// 壁処理の更新
		UpdateMeshWall();

		// 小屋の更新処理
		UpdateHut();

		// 影の更新処理
		UpdateShadow();

	}

	if (GetMode() == MODE_GAME)
	{
		// プレイヤーの更新処理
		UpdatePlayer();

		// 地面処理の更新
		UpdateMeshField();

		// 座標ポイント表示の更新
		UpdateCoordPoint();

		// エネミーの更新処理
		UpdateEnemy();

		// 透明のエネミーの更新処理
		UpdateAlphaEnemy();

		// 壁処理の更新
		UpdateMeshWall();

		// フェンスの更新処理
		UpdateFence();

		// 小屋の更新処理
		UpdateHut();

		// 岩の更新処理
		UpdateRock();

		// ゲートの更新処理
		UpdateGate();

		// 鍵の更新処理
		UpdateKey();

		// 影の更新処理
		UpdateShadow();

		// 当たり判定処理
		CheckHit();

		// タイムの更新処理
		UpdateTime();

		// UIの更新処理
		UpdateUi();

		// 鍵アイコンの更新処理
		UpdateKeyIcon();

		// スタミナゲージの更新処理
		UpdateStaminaBar();

		// エフェクトの更新処理
		UpdateEffect();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// タイトル画面・リザルト画面・ゲームオーバー画面の為の描画処理
	if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		SetSMRenderer();  // 影の描画

		// エネミーの描画処理
		DrawEnemy();

		// 小屋の初期化
		DrawHut();

		// ここまでは影付き
		SetRenderer();	// 通常描画

		// エネミーの描画処理
		DrawEnemy();

		// 地面の描画処理
		DrawMeshField();

		// 小屋の初期化
		DrawHut();

		// 壁の描画処理
		DrawMeshWall();

		// 影の描画処理
		DrawShadow();

	}

	if (GetMode() == MODE_GAME)
	{
		SetSMRenderer();  // 影の描画

		// エネミーの描画処理
		DrawEnemy();

		// プレイヤーの描画処理
		DrawPlayer();

		// 岩の描画処理
		DrawRock();

		// フェンスの描画処理
		DrawFence();

		// 小屋の初期化
		DrawHut();

		// ここまでは影付き

		SetRenderer();	// 通常描画

		// 3Dの物を描画する処理
		// 地面の描画処理
		DrawMeshField();

#ifdef _DEBUG // デバッグ用
		// 座標ポイント表示の更新処理
		DrawCoordPoint();
#endif

		// フェンスの描画処理
		DrawFence();

		// 岩の描画処理
		DrawRock();

		// 小屋の初期化
		DrawHut();

		// ゲートの描画処理
		DrawGate();

		// 鍵の描画処理
		DrawKey();

		// 影の描画処理
		DrawShadow();

		// エネミーの描画処理
		DrawEnemy();

		// 透明のエネミーの描画処理
		DrawAlphaEnemy();

		// プレイヤーの描画処理
		DrawPlayer();

		// 壁の描画処理
		DrawMeshWall();

		// 鍵アイコンの描画処理
		DrawKeyIcon();

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		// エフェクトの描画処理
		DrawEffect();

		// タイムの描画処理
		DrawTime();

		// UIの描画処理
		DrawUi();

		// スタミナゲージの描画処理
		DrawStaminaBar();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);

	}
}


void DrawGame(void)
{
	XMFLOAT3 pos;

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	//SetCameraAT(pos);
	SetCamera();
	DrawGame0();

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();						// エネミーのポインターを初期化
	ALPHA_ENEMY *alphaEnemy = GetAlphaEnemy();		// 透明エネミーのポインターを初期化
	PLAYER *player = GetPlayer();					// プレイヤーのポインターを初期化
	GATE *gate = GetGate();							// ゲートのポインターを初期化
	KEY *key = GetKey();							// 鍵のポインターを初期化
	KEYICON *key_icon = GetKeyIcon();				// 鍵アイコンのポインターを初期化
	UI *ui = GetUi();								// UIのポインターを初期化
	int *P_chip_num = GetPlayer_chip_num();			// 現在プレイヤーがメッシュフィールドの何処(何番目)にいるかをGetする


	// 死神(エネミー)とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// 死神(エネミー)の有効フラグをチェックする
		if (enemy[i].use == FALSE)
			continue;

		// BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size) && player[0].use == TRUE)
		{
			// プレイヤーは倒される
			player[0].use = FALSE;
			ReleaseShadow(player[0].shadowIdx);
			PlaySound(SOUND_LABEL_SE_HIT);		// 斬られる音(やられ音)
			SetFade(FADE_OUT, MODE_GAMEOVER);

		}
	}

	// 透明の死神(エネミー)とプレイヤーキャラ
	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		// 死神(エネミー)の有効フラグをチェックする
		if (alphaEnemy[i].use == FALSE)
			continue;

		// BCの当たり判定
		if (CollisionBC(player->pos, alphaEnemy[i].pos, player->size, alphaEnemy[i].size) && player[0].use == TRUE)
		{
			// プレイヤーは倒される
			player[0].use = FALSE;
			ReleaseShadow(player[0].shadowIdx);
			PlaySound(SOUND_LABEL_SE_HIT);		// 斬られる音(やられ音)
			SetFade(FADE_OUT, MODE_GAMEOVER);
		}
	}


	// 鍵の取得
	for (int i = 0; i < MAX_KEY; i++)
	{
		//プレイヤーの有効フラグをチェックする
		if (key[i].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, key[i].pos, player->size, key[i].size))
		{
			PlaySound(SOUND_LABEL_SE_KEY_GET);	// 取得音
			player[0].keyNumber++;	// 鍵の取得数を１増やす
			key[i].use = FALSE;
		}

		// 赤い鍵を入手したらUIの鍵のアイコンを濃くする
		if (key[0].use == FALSE)
		{
			ui[2].collected = 1.0f;
		}
		// 青い鍵を入手したらUIの鍵のアイコンを濃くする
		if (key[1].use == FALSE)
		{
			ui[3].collected = 1.0f;
		}
		// 黄色い鍵を入手したらUIの鍵のアイコンを濃くする
		if (key[2].use == FALSE)
		{
			ui[4].collected = 1.0f;
		}
	}

	// プレイヤーと脱出ゲート
	for (int i = 0; i < MAX_GATE; i++)
	{
		//プレイヤーの有効フラグをチェックする
		if (player[0].use == FALSE)
			continue;

		//BCの当たり判定
		// 鍵を３つ全て手に入れていたらリザルトに入る
		if (CollisionBC(player->pos, gate[i].pos, player->size, gate[i].size) && player[0].keyNumber == MAX_KEY)
		{
			player[0].keyNumber = 0;				// 鍵の取得数を0にする
			PlaySound(SOUND_LABEL_SE_OPEN_DOOR);	// 扉を開ける音
			SetFade(FADE_OUT, MODE_RESULT);
		}
	}

	// メッシュフィールドの床番号で当たり判定を行う(その床の上には障害物などをセットする)
	// 要素番号はプレイヤーが現在立っているメッシュフィールドの床番号
	COORD_POINT *coord_point = GetCoordPoint();
	
	if (coord_point[*P_chip_num].move == KEEP_OUT)
	{
		player[0].pos = player[0].old_pos;		// 1フレーム前に戻す
	}

}


////=============================================================================
//// ワイヤーフレームモードの取得
////=============================================================================
//int GetWireFrameMode(void)
//{
//	return g_WireFrameSwitch;
//}
