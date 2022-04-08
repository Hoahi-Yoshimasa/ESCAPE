//=============================================================================
//
// 経路探索処理[pathfind.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "pathfind.h"
#include "input.h"
#include "debugproc.h"
#include "nodequeue.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define COLLISION_POINT		(60)	// MAPの当たり判定ポイント(エネミーが経路探索しない箇所のことを指す)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static NODE		g_Node[MAP_RES];
static BOOL		g_Load = FALSE;

// 経路探索用の判定箇所を指定する 現在は当たり判定と同じ場所を指定している (オブジェクト配置箇所を指定)
static int		g_Obstacle[COLLISION_POINT] = { 26,30,31,46,50,51,74,75,83,84,94,95,103,104,114,115,134,135,
											  150,151,162,163,166,170,171,182,183,186,202,203,214,215,222,223,
											  234,235,261,262,267,268,269,270,287,288,289,290,303,304,313,314,
											  317,318,323,324,333,334,353,354,373,374};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPathfind(void)							// チェックポイントの初期化
{
	for (int i = 0; i < MAP_RES; i++)
	{
		g_Node[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Node[i].scl = XMFLOAT3(13.0f, 1.0f, 13.0f);

		g_Node[i].visited = FALSE;					// 処理したかどうかのフラグ初期化
		g_Node[i].cost = 0;							// 全コスト0と初期化
		g_Node[i].prev = -1;						// まだ何も入れてないので-1で初期化
		g_Node[i].load = FALSE;
	}

	for (int j = 0; j < COLLISION_POINT; j++)		// コストの指定
	{
		g_Node[g_Obstacle[j]].cost = 1;				// 障害物の箇所を「cost 1」にしている

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPathfind(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePathfind(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPathfind(void)
{

}

//=============================================================================
// 経路探索 (幅優先探索) 処理
//=============================================================================
int ShortestPath(int start, int goal)
{
	QUEUE* frontier = CreateQueue(MAP_RES);					// キューの大きさを初期化(MAPの大きさで)
	Enqueue(frontier, start);
	g_Node[start].visited = TRUE;							// スタート地点を探索したというフラグを入れる

	while (!isEmpty(frontier))
	{
		int current = Dequeue(frontier);

		if (current == goal)								// 探索した結果、ゴール地点まで探索が終了したらループを抜ける
		{
			break;
		}

		if (current % MAP_RES_X > 0)						// 画面外チェック
		{	// 左側チェック
			if (!g_Node[current - 1].visited && g_Node[current - 1].cost == 0)	// 未探索箇所 かつ 障害物じゃなかったら
			{
				g_Node[current - 1].visited = TRUE;			// 左側のチェックポイントをチェックしたというフラグを立てる
				g_Node[current - 1].prev = current;			// 左側のチェックポイントに現在のチェックポイントの番号を入れる(渡す)
				Enqueue(frontier, current - 1);				// 左側のチェックポイントをキューに追加
			}
		}
		if (current % MAP_RES_X < MAP_RES_X - 1)			// 画面外チェック
		{	// 右側チェック
			if (!g_Node[current + 1].visited && g_Node[current + 1].cost == 0)
			{
				g_Node[current + 1].visited = TRUE;			// 右側のチェックポイントをチェックしたというフラグを立てる
				g_Node[current + 1].prev = current;			// 右側のチェックポイントに現在のチェックポイントの番号を入れる(渡す)
				Enqueue(frontier, current + 1);				// 右側のチェックポイントをキューに追加
			}
		}


		if (current / MAP_RES_X > 0)						// 画面外チェック
		{	//下の場所をチェックする
			if (!g_Node[current - MAP_RES_X].visited && g_Node[current - MAP_RES_X].cost == 0)
			{
				g_Node[current - MAP_RES_X].visited = TRUE;	// 下側のチェックポイントをチェックしたというフラグを立てる
				g_Node[current - MAP_RES_X].prev = current;	// 下側のチェックポイントに現在のチェックポイントの番号を入れる(渡す)
				Enqueue(frontier, current - MAP_RES_X);		// 下側のチェックポイントをキューに追加
			}
		}
		if (current / MAP_RES_X < MAP_RES_Y - 1)			// 画面外チェック
		{	// 上の場所をチェックする
			if (!g_Node[current + MAP_RES_X].visited && g_Node[current + MAP_RES_X].cost == 0)
			{
				g_Node[current + MAP_RES_X].visited = TRUE;	// 上側のチェックポイントをチェックしたというフラグを立てる
				g_Node[current + MAP_RES_X].prev = current;	// 上側のチェックポイントに現在のチェックポイントの番号を入れる(渡す)
				Enqueue(frontier, current + MAP_RES_X);		// 上側のチェックポイントをキューに追加
			}
		}
	}

	int reverseCurrent = goal;								// 現在位置をゴールに設定
	int reversePrev = -1;									// 逆方向の一つ前のチェックポイントを保存

	for (int j = 0; j < MAP_RES; j++)
	{
		reversePrev = reverseCurrent;						// 現在位置をreversePrevに保存
		reverseCurrent = g_Node[reverseCurrent].prev;		// 現在の場所(reverseCurrent)の一つ前のポイントを現在の場所にする()
		if (reverseCurrent == start)						// reverseCurrentがスタート地点ならループを抜ける
		{
			break;
		}
	}

	for (int i = 0; i < MAP_RES; i++)
	{
		g_Node[i].visited = FALSE;							// チェックポイントのフラグを全リセット
		g_Node[i].prev = -1;								// 一つ前のチェックポイントも全リセット
	}

	return reversePrev;										// 最短経路の次のポイントだけを返す
}


NODE *GetNode(void)
{
	return &g_Node[0];
}
