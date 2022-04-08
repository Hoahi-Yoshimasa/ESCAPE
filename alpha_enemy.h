//=============================================================================
//
// 透明エネミーモデル処理 [alpha_enemy.h]
// Author : 星克昌
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ALPHA_ENEMY			(1)			// エネミーの数

#define	ALPHA_ENEMY_SIZE		(15.0f)		// 当たり判定の大きさ
#define ALPHA_ENEMY_OFFSET_Y	(25.0f)		// エネミーの足元をあわせる

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ALPHA_ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	BOOL				chase_mode;			//チェイスモード TRUEなら追いかけるモード

	float				angle;				// 角度
	float				p_dist;				// プレイヤーまでの距離

	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号
	
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitAlphaEnemy(void);
void UninitAlphaEnemy(void);
void UpdateAlphaEnemy(void);
void DrawAlphaEnemy(void);

ALPHA_ENEMY *GetAlphaEnemy(void);

