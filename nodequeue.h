//=============================================================================
//
// �L���[�����iFirst In, First Out�j[nodequeue.h]
// Author : ������
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct QUEUE
{
	int			first;		// �z��ŏ��̔ԍ�
	int			last;		// �z��Ō�̔ԍ�
	int			size;		// �z��̃f�[�^�̐�

	unsigned	capacity;	// �S�̂̔z��̑傫��
	int*		array;		// �z��
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
QUEUE* CreateQueue(unsigned capacity);
BOOL isFull(QUEUE* queue);
BOOL isEmpty(QUEUE* queue);
void Enqueue(QUEUE* queue, int node);
int Dequeue(QUEUE* queue);
int First(QUEUE* queue);
int Last(QUEUE* queue);
