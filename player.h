//=============================================================================
//
// モデル処理 [player.h]
// Author : 星克昌
//
//=============================================================================
#pragma once

#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER				(1)			// プレイヤーの数

#define	PLAYER_SIZE				(10.0f)		// 当たり判定の大きさ

#define PLAYER_STAMINA			(100.0f)	// プレイヤーのスタミナ
#define PLAYER_STAMINA_MAX		(100.0f)	// プレイヤースタミナ最大値

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;				// ポリゴンの位置
	XMFLOAT3			old_pos;			// 当たり判定用のポリゴンの位置
											
	XMFLOAT3			rot;				// ポリゴンの向き(回転)
	XMFLOAT3			scl;				// ポリゴンの大きさ(スケール)
											
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
											
	BOOL				load;				
	DX11_MODEL			model;				// モデル情報
											
	float				spd;				// 移動スピード
	float				dir;				// 向き
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のIndex
	BOOL				use;
	float				stamina;			// スタミナ
	float				staminaPercent;		// スタミナが何％か
	int					keyNumber;			// 鍵の取得数
	// 階層アニメーション用のメンバー変数
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			quaternion;			// クォータニオン
	XMFLOAT3			upVector;			// 自分が立っている所


};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
int *GetStaminaRecovery_count(void);

