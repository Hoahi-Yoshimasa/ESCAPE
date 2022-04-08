//=============================================================================
//
// チュートリアル画面処理 [tutorial.cpp]
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
#include "tutorial.h"
#include "player.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "collision.h"
#include "debugproc.h"
#include "stamina_bar.h"
#include "ui.h"
#include "gate.h"
#include "item_key.h"
#include "time.h"
#include "shadow_renderer.h"
#include "Coord_Point.h"
#include "key_icon.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHitTutorial(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************

static BOOL	g_bPause = TRUE;	// ポーズON/OFF


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), MESHFIELD_BlockX, MESHFIELD_BlockZ, MESHFIELD_WIDTH, MESHFIELD_HEIGHT); // 13

	// 座標ポイント表示の初期化
	InitCoordPoint();

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);


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

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_TUTORIAL);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
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

	// 鍵の終了処理
	UninitKey();

	// 壁の終了処理
	UninitMeshWall();

	// 座標ポイント表示の終了処理
	UninitCoordPoint();

	// 地面の終了処理
	UninitMeshField();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{

	if (g_bPause == FALSE)
		return;

	// プレイヤーの更新処理
	UpdatePlayer();

	// 地面処理の更新
	UpdateMeshField();

	// 座標ポイント表示の更新
	UpdateCoordPoint();

	// 壁処理の更新
	UpdateMeshWall();

	// ゲートの更新処理
	UpdateGate();

	// 鍵の更新処理
	UpdateKey();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHitTutorial();

	// タイムの更新処理
	UpdateTime();

	// UIの更新処理
	UpdateUi();

	// スタミナゲージの更新処理
	UpdateStaminaBar();

	// 鍵アイコンの更新処理
	UpdateKeyIcon();

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial0(void)
{
	SetSMRenderer();  // 影の描画

	// プレイヤーの描画処理
	DrawPlayer();

	// ここまでは影付き

	SetRenderer();	// 通常描画


	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

#ifdef _DEBUG // デバッグ用
	// 座標ポイント表示の更新処理
	DrawCoordPoint();
#endif

	// ゲートの描画処理
	DrawGate();

	// 鍵の描画処理
	DrawKey();

	// 影の描画処理
	DrawShadow();

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

void DrawTutorial(void)
{
	XMFLOAT3 pos;

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();
	DrawTutorial0();

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHitTutorial(void)
{
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	GATE *gate = GetGate();			// ゲートのポインターを初期化
	KEY *key = GetKey();			// 鍵のポインターを初期化
	UI *ui = GetUi();				// UIのポインターを初期化
	int *P_chip_num = GetPlayer_chip_num();	// 現在プレイヤーがメッシュフィールドの何処(何番目)にいるかをGetする


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
			SetFade(FADE_OUT, MODE_TITLE);
		}
	}

	// メッシュフィールドの床番号で当たり判定を行う(その床の上には障害物などをセットする)
	// 要素番号はプレイヤーが現在立っているメッシュフィールドの床番号
	COORD_POINT *coord_point = GetCoordPoint();
	
	if (coord_point[*P_chip_num].move == -1)
	{
		player[0].pos = player[0].old_pos;		// 1フレーム前に戻す
	}

}


