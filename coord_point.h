//=============================================================================
//
// ���W�|�C���g���� [Coord_Point.h]
// Author : ������
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	KEEP_OUT				(-1)			// �����蔻��ӏ�

//*****************************************************************************
// ���W�|�C���g�\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	BOOL		use;			// �g�p���Ă��邩�ǂ���
	int			move;			// KEEP_OUT(-1) �Ȃ瓮���Ȃ��ꏊ
} COORD_POINT;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitCoordPoint(void);
void UninitCoordPoint(void);
void UpdateCoordPoint(void);
void DrawCoordPoint(void);

XMFLOAT3 *GetCoord(void);

HRESULT MakeVertexCoordPoint(void);

int SetCoordPoint(XMFLOAT3 pos);
COORD_POINT *GetCoordPoint(void);

