//=============================================================================
//
// �o�H�T������[pathfind.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "pathfind.h"
#include "input.h"
#include "debugproc.h"
#include "nodequeue.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define COLLISION_POINT		(60)	// MAP�̓����蔻��|�C���g(�G�l�~�[���o�H�T�����Ȃ��ӏ��̂��Ƃ��w��)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static NODE		g_Node[MAP_RES];
static BOOL		g_Load = FALSE;

// �o�H�T���p�̔���ӏ����w�肷�� ���݂͓����蔻��Ɠ����ꏊ���w�肵�Ă��� (�I�u�W�F�N�g�z�u�ӏ����w��)
static int		g_Obstacle[COLLISION_POINT] = { 26,30,31,46,50,51,74,75,83,84,94,95,103,104,114,115,134,135,
											  150,151,162,163,166,170,171,182,183,186,202,203,214,215,222,223,
											  234,235,261,262,267,268,269,270,287,288,289,290,303,304,313,314,
											  317,318,323,324,333,334,353,354,373,374};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPathfind(void)							// �`�F�b�N�|�C���g�̏�����
{
	for (int i = 0; i < MAP_RES; i++)
	{
		g_Node[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Node[i].scl = XMFLOAT3(13.0f, 1.0f, 13.0f);

		g_Node[i].visited = FALSE;					// �����������ǂ����̃t���O������
		g_Node[i].cost = 0;							// �S�R�X�g0�Ə�����
		g_Node[i].prev = -1;						// �܂���������ĂȂ��̂�-1�ŏ�����
		g_Node[i].load = FALSE;
	}

	for (int j = 0; j < COLLISION_POINT; j++)		// �R�X�g�̎w��
	{
		g_Node[g_Obstacle[j]].cost = 1;				// ��Q���̉ӏ����ucost 1�v�ɂ��Ă���

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPathfind(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePathfind(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPathfind(void)
{

}

//=============================================================================
// �o�H�T�� (���D��T��) ����
//=============================================================================
int ShortestPath(int start, int goal)
{
	QUEUE* frontier = CreateQueue(MAP_RES);					// �L���[�̑傫����������(MAP�̑傫����)
	Enqueue(frontier, start);
	g_Node[start].visited = TRUE;							// �X�^�[�g�n�_��T�������Ƃ����t���O������

	while (!isEmpty(frontier))
	{
		int current = Dequeue(frontier);

		if (current == goal)								// �T���������ʁA�S�[���n�_�܂ŒT�����I�������烋�[�v�𔲂���
		{
			break;
		}

		if (current % MAP_RES_X > 0)						// ��ʊO�`�F�b�N
		{	// �����`�F�b�N
			if (!g_Node[current - 1].visited && g_Node[current - 1].cost == 0)	// ���T���ӏ� ���� ��Q������Ȃ�������
			{
				g_Node[current - 1].visited = TRUE;			// �����̃`�F�b�N�|�C���g���`�F�b�N�����Ƃ����t���O�𗧂Ă�
				g_Node[current - 1].prev = current;			// �����̃`�F�b�N�|�C���g�Ɍ��݂̃`�F�b�N�|�C���g�̔ԍ�������(�n��)
				Enqueue(frontier, current - 1);				// �����̃`�F�b�N�|�C���g���L���[�ɒǉ�
			}
		}
		if (current % MAP_RES_X < MAP_RES_X - 1)			// ��ʊO�`�F�b�N
		{	// �E���`�F�b�N
			if (!g_Node[current + 1].visited && g_Node[current + 1].cost == 0)
			{
				g_Node[current + 1].visited = TRUE;			// �E���̃`�F�b�N�|�C���g���`�F�b�N�����Ƃ����t���O�𗧂Ă�
				g_Node[current + 1].prev = current;			// �E���̃`�F�b�N�|�C���g�Ɍ��݂̃`�F�b�N�|�C���g�̔ԍ�������(�n��)
				Enqueue(frontier, current + 1);				// �E���̃`�F�b�N�|�C���g���L���[�ɒǉ�
			}
		}


		if (current / MAP_RES_X > 0)						// ��ʊO�`�F�b�N
		{	//���̏ꏊ���`�F�b�N����
			if (!g_Node[current - MAP_RES_X].visited && g_Node[current - MAP_RES_X].cost == 0)
			{
				g_Node[current - MAP_RES_X].visited = TRUE;	// �����̃`�F�b�N�|�C���g���`�F�b�N�����Ƃ����t���O�𗧂Ă�
				g_Node[current - MAP_RES_X].prev = current;	// �����̃`�F�b�N�|�C���g�Ɍ��݂̃`�F�b�N�|�C���g�̔ԍ�������(�n��)
				Enqueue(frontier, current - MAP_RES_X);		// �����̃`�F�b�N�|�C���g���L���[�ɒǉ�
			}
		}
		if (current / MAP_RES_X < MAP_RES_Y - 1)			// ��ʊO�`�F�b�N
		{	// ��̏ꏊ���`�F�b�N����
			if (!g_Node[current + MAP_RES_X].visited && g_Node[current + MAP_RES_X].cost == 0)
			{
				g_Node[current + MAP_RES_X].visited = TRUE;	// �㑤�̃`�F�b�N�|�C���g���`�F�b�N�����Ƃ����t���O�𗧂Ă�
				g_Node[current + MAP_RES_X].prev = current;	// �㑤�̃`�F�b�N�|�C���g�Ɍ��݂̃`�F�b�N�|�C���g�̔ԍ�������(�n��)
				Enqueue(frontier, current + MAP_RES_X);		// �㑤�̃`�F�b�N�|�C���g���L���[�ɒǉ�
			}
		}
	}

	int reverseCurrent = goal;								// ���݈ʒu���S�[���ɐݒ�
	int reversePrev = -1;									// �t�����̈�O�̃`�F�b�N�|�C���g��ۑ�

	for (int j = 0; j < MAP_RES; j++)
	{
		reversePrev = reverseCurrent;						// ���݈ʒu��reversePrev�ɕۑ�
		reverseCurrent = g_Node[reverseCurrent].prev;		// ���݂̏ꏊ(reverseCurrent)�̈�O�̃|�C���g�����݂̏ꏊ�ɂ���()
		if (reverseCurrent == start)						// reverseCurrent���X�^�[�g�n�_�Ȃ烋�[�v�𔲂���
		{
			break;
		}
	}

	for (int i = 0; i < MAP_RES; i++)
	{
		g_Node[i].visited = FALSE;							// �`�F�b�N�|�C���g�̃t���O��S���Z�b�g
		g_Node[i].prev = -1;								// ��O�̃`�F�b�N�|�C���g���S���Z�b�g
	}

	return reversePrev;										// �ŒZ�o�H�̎��̃|�C���g������Ԃ�
}


NODE *GetNode(void)
{
	return &g_Node[0];
}
