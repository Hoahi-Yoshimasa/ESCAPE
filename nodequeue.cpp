//=============================================================================
//
// �L���[�����iFirst In, First Out�j[nodequeue.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "nodequeue.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
QUEUE* CreateQueue(unsigned capacity)						// �L���[�����ׂ̊֐�
{
	QUEUE* queue = (QUEUE*)malloc(sizeof(QUEUE));
	queue->capacity = capacity;
	queue->first = queue->size = 0;
	queue->last = capacity - 1;
	queue->array = (int*)malloc(queue->capacity * sizeof(int));
	return queue;
}

BOOL isFull(QUEUE* queue)									// �f�[�^��������邩�`�F�b�N����֐� 
{
	if (queue->size == queue->capacity)						//���g�̐��Ɣz��̑傫���������Ȃ�TRUE��Ԃ�
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL isEmpty(QUEUE* queue)									// �L���[���󂩂ǂ����`�F�b�N����֐�
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

void Enqueue(QUEUE* queue, int node)						// �f�[�^���L���[�ɒǉ�����֐�
{
	if (isFull(queue))										// �f�[�^����t�Ȃ烊�^�[������
	{
		return;
	}
	queue->last = (queue->last + 1) % queue->capacity;		// last�̗v�f�ԍ����ړ�������
	queue->array[queue->last] = node;						// �f�[�^���ǉ����ꂽ�ꍇ���ꂪlast�ɂȂ�
	queue->size = queue->size + 1;							// �f�[�^�̃T�C�Y��+1����
}

int Dequeue(QUEUE* queue)									// �ŏ��ɓ��ꂽ�f�[�^��Ԃ��֐�
{
	int node = queue->array[queue->first];					// first�̏ꏊ�ɂ���f�[�^�����o��
	queue->first = (queue->first + 1) % queue->capacity;	// first�̈ʒu��+1���� first�̈ʒu��capacityMAX�ɂȂ�����0�ɖ߂�悤��
	queue->size = queue->size - 1;							// �f�[�^�̃T�C�Y��-1����
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