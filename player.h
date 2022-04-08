//=============================================================================
//
// ���f������ [player.h]
// Author : ������
//
//=============================================================================
#pragma once

#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER				(1)			// �v���C���[�̐�

#define	PLAYER_SIZE				(10.0f)		// �����蔻��̑傫��

#define PLAYER_STAMINA			(100.0f)	// �v���C���[�̃X�^�~�i
#define PLAYER_STAMINA_MAX		(100.0f)	// �v���C���[�X�^�~�i�ő�l

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;				// �|���S���̈ʒu
	XMFLOAT3			old_pos;			// �����蔻��p�̃|���S���̈ʒu
											
	XMFLOAT3			rot;				// �|���S���̌���(��])
	XMFLOAT3			scl;				// �|���S���̑傫��(�X�P�[��)
											
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
											
	BOOL				load;				
	DX11_MODEL			model;				// ���f�����
											
	float				spd;				// �ړ��X�s�[�h
	float				dir;				// ����
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e��Index
	BOOL				use;
	float				stamina;			// �X�^�~�i
	float				staminaPercent;		// �X�^�~�i��������
	int					keyNumber;			// ���̎擾��
	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	INTERPOLATION_DATA	*tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;			// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			quaternion;			// �N�H�[�^�j�I��
	XMFLOAT3			upVector;			// �����������Ă��鏊


};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
int *GetStaminaRecovery_count(void);

