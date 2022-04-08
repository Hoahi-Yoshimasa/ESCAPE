//=============================================================================
//
// ��̃��f������ [rock.cpp]
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
#include "rock.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ROCK			"data/MODEL/rock.obj"		// �ǂݍ��ރ��f����


#define ROCK_SHADOW_SIZE	(0.4f)						// �e�̑傫��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ROCK			g_Rock[MAX_ROCK];					// ��

static BOOL			g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRock(void)
{
	for (int i = 0; i < MAX_ROCK; i++)
	{
		LoadModel(MODEL_ROCK, &g_Rock[i].model);
		g_Rock[i].load = TRUE;

		g_Rock[i].pos = XMFLOAT3(0.0f, ROCK_OFFSET_Y, 12.0f);
		g_Rock[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rock[i].scl = XMFLOAT3(0.9f, 0.7f, 0.5f);
		g_Rock[i].size = ROCK_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Rock[0].model, &g_Rock[0].diffuse[0]);

		XMFLOAT3 pos = g_Rock[i].pos;
		pos.y -= (ROCK_OFFSET_Y - 0.1f);
		g_Rock[i].shadowIdx = CreateShadow(pos, ROCK_SHADOW_SIZE, ROCK_SHADOW_SIZE);

		g_Rock[i].use = TRUE;			// TRUE:�g���Ă���

	}

	// �e�₲�ƂɌʂŏ���������
	g_Rock[0].pos = XMFLOAT3(-400.0f, ROCK_OFFSET_Y, 175.0f);
	g_Rock[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Rock[0].scl = XMFLOAT3(1.0f, 0.7f, 0.5f);

	g_Rock[1].pos = XMFLOAT3(-175.0f, ROCK_OFFSET_Y, -405.0f);
	g_Rock[1].rot = XMFLOAT3(0.0f, XM_PI + XM_PIDIV2, 0.0f);
	g_Rock[1].scl = XMFLOAT3(1.0f, 0.7f, 0.5f);

	g_Rock[2].pos = XMFLOAT3(-175.0f, ROCK_OFFSET_Y, -53.0f);
	g_Rock[2].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Rock[2].scl = XMFLOAT3(0.8f, 0.7f, 0.5f);

	g_Rock[3].pos = XMFLOAT3(400.0f, ROCK_OFFSET_Y, 275.0f);
	g_Rock[3].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);
	g_Rock[3].scl = XMFLOAT3(1.0f, 0.7f, 0.5f);


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRock(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].load)
		{
			UnloadModel(&g_Rock[i].model);
			g_Rock[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRock(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRock(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O�L��
	SetCullingMode(CULL_MODE_BACK);

	for (int i = 0; i < MAX_ROCK; i++)
	{
		if (g_Rock[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Rock[i].scl.x, g_Rock[i].scl.y, g_Rock[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Rock[i].rot.x, g_Rock[i].rot.y + XM_PI, g_Rock[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Rock[i].pos.x, g_Rock[i].pos.y, g_Rock[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Rock[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Rock[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}



//=============================================================================
// ��̏��擾
//=============================================================================
ROCK *GetRock()
{
	return &g_Rock[0];
}

