//=============================================================================
//
// �����̃��f������ [hut.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"
#include "debugproc.h"
#include "collision.h"
#include "coord_point.h"
#include "hut.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_HUT			"data/MODEL/hut.obj"		// �ǂݍ��ރ��f����


#define HUT_SHADOW_SIZE	(0.4f)							// �e�̑傫��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static HUT				g_Hut[MAX_HUT];					// ����
static BOOL				g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitHut(void)
{
	for (int i = 0; i < MAX_HUT; i++)
	{
		LoadModel(MODEL_HUT, &g_Hut[i].model);
		g_Hut[i].load = TRUE;

		g_Hut[i].pos = XMFLOAT3(0.0f, HUT_OFFSET_Y, 12.0f);
		g_Hut[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Hut[i].scl = XMFLOAT3(0.9f, 1.2f, 0.8f);
		g_Hut[i].size = HUT_SIZE;		// �����蔻��̑傫��

		g_Hut[i].use = TRUE;			// TRUE:�g���Ă���

	}

	// �e�������ƂɌʂŏ���������
	g_Hut[0].pos = XMFLOAT3(-40.0f, HUT_OFFSET_Y, 195.0f);
	g_Hut[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Hut[1].pos = XMFLOAT3(245.0f, HUT_OFFSET_Y, -250.0f);
	g_Hut[1].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

	g_Hut[2].pos = XMFLOAT3(-355.0f, HUT_OFFSET_Y, -5.0f);
	g_Hut[2].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHut(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_HUT; i++)
	{
		if (g_Hut[i].load)
		{
			UnloadModel(&g_Hut[i].model);
			g_Hut[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateHut(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHut(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O�L��
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_HUT; i++)
	{
		if (g_Hut[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Hut[i].scl.x, g_Hut[i].scl.y, g_Hut[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Hut[i].rot.x, g_Hut[i].rot.y + XM_PI, g_Hut[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Hut[i].pos.x, g_Hut[i].pos.y, g_Hut[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Hut[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Hut[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}



//=============================================================================
// �����̏��擾
//=============================================================================
HUT *GetHut()
{
	return &g_Hut[0];
}

