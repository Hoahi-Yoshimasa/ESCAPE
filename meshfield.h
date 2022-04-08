//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.h]
// Author : ������
//
//=============================================================================
#pragma once
#include "renderer.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define MESHFIELD_BlockX		(20)	// ���b�V���t�B�[���h�̉��̐�
#define MESHFIELD_BlockZ		(20)	// ���b�V���t�B�[���h�̏c�̐�

#define MESHFIELD_WIDTH			(50.0f)	// ���b�V���t�B�[���h�̉��̑傫��
#define MESHFIELD_HEIGHT		(50.0f)	// ���b�V���t�B�[���h�̏c�̑傫��

#define MAP_MESHFIELD_WIDTH			(MESHFIELD_BlockX * MESHFIELD_WIDTH)	// ���b�V���t�B�[���h�S�̂̉��̑傫��
#define MAP_MESHFIELD_HEIGHT		(MESHFIELD_BlockZ * MESHFIELD_HEIGHT)	// ���b�V���t�B�[���h�S�̂̏c�̑傫��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

XMFLOAT3 *GetCoord();

HRESULT MakeVertexPoint(void);

int *GetEnemy_chip_num();
int *GetPlayer_chip_num();
