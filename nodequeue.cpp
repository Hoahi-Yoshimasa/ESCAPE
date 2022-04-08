//=============================================================================
//
// キュー処理（First In, First Out）[nodequeue.cpp]
// Author : 星克昌
//
//=============================================================================
#include "main.h"
#include "nodequeue.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
QUEUE* CreateQueue(unsigned capacity)						// キューを作る為の関数
{
	QUEUE* queue = (QUEUE*)malloc(sizeof(QUEUE));
	queue->capacity = capacity;
	queue->first = queue->size = 0;
	queue->last = capacity - 1;
	queue->array = (int*)malloc(queue->capacity * sizeof(int));
	return queue;
}

BOOL isFull(QUEUE* queue)									// データが入れられるかチェックする関数 
{
	if (queue->size == queue->capacity)						//中身の数と配列の大きさが同じならTRUEを返す
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL isEmpty(QUEUE* queue)									// キューが空かどうかチェックする関数
{
	if (queue->size == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void Enqueue(QUEUE* queue, int node)						// データをキューに追加する関数
{
	if (isFull(queue))										// データが一杯ならリターンする
	{
		return;
	}
	queue->last = (queue->last + 1) % queue->capacity;		// lastの要素番号を移動させる
	queue->array[queue->last] = node;						// データが追加された場合それがlastになる
	queue->size = queue->size + 1;							// データのサイズを+1する
}

int Dequeue(QUEUE* queue)									// 最初に入れたデータを返す関数
{
	int node = queue->array[queue->first];					// firstの場所にあるデータを取り出す
	queue->first = (queue->first + 1) % queue->capacity;	// firstの位置を+1する firstの位置がcapacityMAXになったら0に戻るように
	queue->size = queue->size - 1;							// データのサイズを-1する
	return node;
}

int First(QUEUE* queue)
{
	return queue->array[queue->first];
}

int Last(QUEUE* queue)
{
	return queue->array[queue->last];
}