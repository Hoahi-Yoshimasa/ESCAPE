//=============================================================================
//
// キュー処理（First In, First Out）[nodequeue.h]
// Author : 星克昌
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct QUEUE
{
	int			first;		// 配列最初の番号
	int			last;		// 配列最後の番号
	int			size;		// 配列のデータの数

	unsigned	capacity;	// 全体の配列の大きさ
	int*		array;		// 配列
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
QUEUE* CreateQueue(unsigned capacity);
BOOL isFull(QUEUE* queue);
BOOL isEmpty(QUEUE* queue);
void Enqueue(QUEUE* queue, int node);
int Dequeue(QUEUE* queue);
int First(QUEUE* queue);
int Last(QUEUE* queue);
