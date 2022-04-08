//=============================================================================
//
// ���̃��f������ [gate.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "debugproc.h"
#include "collision.h"
#include "gate.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_GATE			"data/MODEL/gate.obj"		// �ǂݍ��ރ��f����


#define GATE_SHADOW_SIZE	(0.4f)						// �e�̑傫��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static GATE			g_Gate[MAX_GATE];					// �Q�[�g

static BOOL				g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGate(void)
{
	for (int i = 0; i < MAX_GATE; i++)
	{
		LoadModel(MODEL_GATE, &g_Gate[i].model);
		g_Gate[i].load = TRUE;

		g_Gate[i].pos = XMFLOAT3(-400.0f, GATE_OFFSET_Y, 470.0f);
		g_Gate[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Gate[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
		g_Gate[i].size = GATE_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Gate[0].model, &g_Gate[0].diffuse[0]);

		XMFLOAT3 pos = g_Gate[i].pos;
		pos.y -= (GATE_OFFSET_Y - 0.1f);
		g_Gate[i].shadowIdx = CreateShadow(pos, GATE_SHADOW_SIZE, GATE_SHADOW_SIZE);



		g_Gate[i].use = TRUE;			// TRUE:�g���Ă���

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGate(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_GATE; i++)
	{
		if (g_Gate[i].load)
		{
			UnloadModel(&g_Gate[i].model);
			g_Gate[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGate(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGate(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_GATE; i++)
	{
		if (g_Gate[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Gate[i].scl.x, g_Gate[i].scl.y, g_Gate[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Gate[i].rot.x, g_Gate[i].rot.y + XM_PI, g_Gate[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Gate[i].pos.x, g_Gate[i].pos.y, g_Gate[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Gate[i].mtxWorld, mtxWorld);

		DrawModel(&g_Gate[i].model);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}



//=============================================================================
// �Q�[�g�̎擾
//=============================================================================
GATE *GetGate()
{
	return &g_Gate[0];
}

