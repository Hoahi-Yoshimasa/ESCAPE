//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// FPS用
#define	POS_X_CAM			(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM			(33.0f)			// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-70.0f)		// カメラの初期位置(Z座標)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(10000.0f)										// ビュー平面のFarZ値
#define	PAN_UP_LIMIT	(50.0f)											// カメラの上方向の上限値
#define	PAN_DOWN_LIMIT	(0.0f)											// カメラの上方向の上限値

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_Camera;		// カメラデータ

// カメラの視点
static INTERPOLATION_DATA cam_move_tbl[] = {	// pos, rot, 注視点, frame
	{ XMFLOAT3(-50.0f, 10.0f, -250.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 5}, // 出発点
	{ XMFLOAT3(-50.0f, 10.0f, 60.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 20.0f, 60.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 30.0f, 80.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3}, //ループ開始地点
	{ XMFLOAT3(-90.0f, 30.0f, 100.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-20.0f, 30.0f, 70.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 150.0f,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(115.0f, 150.0f, 70.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(160.0f, 150.0f,  200.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 200.0f,400.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-240.0f, 150.0f, 200.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 150.0f, 20.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 20.0f, 60.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 30.0f, 80.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3}, // 終着点
};

//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	// FPS 用
	g_Camera.pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera.at = { 0.0f, 33.0f, 0.0f };
	g_Camera.up = { 0.0f, 1.0f, 0.0f };
	g_Camera.rot = { 0.0f, XM_PI, 0.0f };


	// チュートリアルの場合の設定
	if (GetMode() == MODE_TITLE)
	{
		g_Camera.at = { -40.0f, 33.0f, 200.0f };
	}

	// リザルト画面とゲームオーバー画面のカメラ設定
	if (GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		g_Camera.pos = { -50.0f, 40.0f, -30.0f };
		g_Camera.at = { -50.0f, 40.0f, 130.0f };
		g_Camera.up = { 0.0f, 1.0f, 0.0f };
		g_Camera.rot = { 0.0f, XM_PI, 0.0f };

	}

	// 視点と注視点の距離を計算
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);

	// カメラ視点
	g_Camera.move_time = 0.0f;				// 線形補間用のタイマーをクリア
	g_Camera.tbl_adr = cam_move_tbl;		// 再生するアニメデータの先頭アドレスをセット
	g_Camera.tbl_size = sizeof(cam_move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	PLAYER *player = GetPlayer();


	// タイトル画面ではカメラの線形補間を行う
	if (GetMode() == MODE_TITLE)
	{
		PrintDebugProc("Camera:move_time: %f\n", g_Camera.move_time);

		PrintDebugProc("Camera:ATx %f y;%f z;%f\n", g_Camera.at.x, g_Camera.at.y, g_Camera.at.z);
		PrintDebugProc("Camera:pos.x %f y;%f z;%f\n", g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z);

		if (g_Camera.tbl_adr != NULL)	// 線形補間を実行する？
		{								// 線形補間の処理
			// 移動処理
			int		index = (int)g_Camera.move_time;
			float	time = g_Camera.move_time - index;
			int		size = g_Camera.tbl_size;

			float dt = 1.0f / g_Camera.tbl_adr[index].frame;	// 1フレームで進める時間
			g_Camera.move_time += dt;							// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、index3へ戻す(ループのため)
			{
				g_Camera.move_time = 3.0f;
				index = 3;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			XMVECTOR p1 = XMLoadFloat3(&g_Camera.tbl_adr[index + 1].pos);	// 次の場所
			XMVECTOR p0 = XMLoadFloat3(&g_Camera.tbl_adr[index + 0].pos);	// 現在の場所
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Camera.pos, p0 + vec * time);


			// 注視点を求める	R = StartX + (EndX - StartX) * 今の時間
			XMVECTOR at1 = XMLoadFloat3(&g_Camera.tbl_adr[index + 1].scl);	// 次の角度
			XMVECTOR at0 = XMLoadFloat3(&g_Camera.tbl_adr[index + 0].scl);	// 現在の角度
			XMVECTOR at = at1 - at0;
			XMStoreFloat3(&g_Camera.at, at0 + at * time);
		}

	}



	if (GetMode() == MODE_GAME || GetMode() == MODE_TUTORIAL)
	{
		// プレイヤーが生きている時のみ操作可能
		if (player[0].use == TRUE)
		{
			// FPS用
			if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_A) || IsButtonPressed(0, BUTTON_Y))
			{// 注視点旋回「左」
				g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
				if (g_Camera.rot.y < -XM_PI)
				{
					g_Camera.rot.y += XM_PI * 2.0f;
				}
				g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
				g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
			}

			if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_C) || IsButtonPressed(0, BUTTON_Z))
			{// 注視点旋回「右」
				g_Camera.rot.y += VALUE_ROTATE_CAMERA;
				if (g_Camera.rot.y > XM_PI)
				{
					g_Camera.rot.y -= XM_PI * 2.0f;
				}

				g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
				g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
			}

			if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_X))
			{// 注視点移動「上」
				g_Camera.at.y += VALUE_MOVE_CAMERA;
			}

			if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_B))
			{// 注視点移動「下」
				g_Camera.at.y -= VALUE_MOVE_CAMERA;
			}

			// カメラが上限値以上向かないようにする処理
			if (g_Camera.at.y < PAN_DOWN_LIMIT)
			{
				g_Camera.at.y = PAN_DOWN_LIMIT;
			}
			if (g_Camera.at.y > PAN_UP_LIMIT)
			{
				g_Camera.at.y = PAN_UP_LIMIT;
			}

		}

		PrintDebugProc("Camera:AT.y %f\n", g_Camera.at.y);
		PrintDebugProc("Camera:pos.x %f y;%f z;%f\n", g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z);

#ifdef _DEBUG	// デバッグ情報を表示する
		PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif

	}
}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void)
{
	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void)
{
	return &g_Camera;
}


// カメラの視点と注視点をセット
void SetCameraAT(XMFLOAT3 pos)
{
	if (GetMode() == MODE_GAME || GetMode() == MODE_TUTORIAL)
	{
		// FPS用カメラ設定
		g_Camera.pos = pos;					// プレイヤーのPOSからカメラのPOSを合わせる
		g_Camera.pos.y = POS_Y_CAM;			// 目線の高さにオフセットする
		//g_Camera.pos.z += POS_OFFSET_Z_CAM; // 少しカメラを後ろに下げている(フェンスと衝突した時ポリゴンの向こうが見えてしまうのを防ぐため)

		// カメラの視点をカメラのY軸回転に対応させている
		g_Camera.at.x = g_Camera.pos.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}
}

