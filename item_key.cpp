//=============================================================================
//
// ���̃��f������ [key.cpp]
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
#include "item_key.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_KEY		"data/MODEL/key.obj"		// �ǂݍ��ރ��f����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static KEY			g_Key[MAX_KEY];						// ��

static BOOL			g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitKey(void)
{
	for (int i = 0; i < MAX_KEY; i++)
	{
		LoadModel(MODEL_KEY, &g_Key[i].model);

		g_Key[i].load = TRUE;

		g_Key[i].pos = XMFLOAT3(0.0f, KEY_OFFSET_Y, -20.0f);
		g_Key[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Key[i].scl = XMFLOAT3(0.8f, 0.8f, 0.8f);
		g_Key[i].size = KEY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Key[0].model, &g_Key[0].diffuse[0]);

		XMFLOAT3 pos = g_Key[i].pos;
		pos.y -= (KEY_OFFSET_Y - 0.1f);
		g_Key[i].use = TRUE;			// TRUE:�g���Ă���

	}

	// �e���̐F�w��
	// �Ԃ���
	for (int j = 0; j < g_Key[RED_KEY].model.SubsetNum; j++)
	{
		SetModelDiffuse(&g_Key[RED_KEY].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.99f)); // ���l���P�Ȃ�V���h�E��`�悷��悤�ɂ��Ă���� 0.99�ɂ��Ă���
	}
	// ����
	for (int k = 0; k < g_Key[BLUE_KEY].model.SubsetNum; k++)
	{
		SetModelDiffuse(&g_Key[BLUE_KEY].model, k, XMFLOAT4(0.0f, 0.0f, 1.0f, 0.99f));
	}
	// ���F����
	for (int l = 0; l < g_Key[YELLOW_KEY].model.SubsetNum; l++)
	{
		SetModelDiffuse(&g_Key[YELLOW_KEY].model, l, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.99f));
	}

	// ���C���Q�[�����̌���POS�w��
	if (GetMode() == MODE_GAME)
	{
		g_Key[RED_KEY].pos = XMFLOAT3(290.0f, KEY_OFFSET_Y, -240.0f);
		g_Key[RED_KEY].rot = XMFLOAT3(0.0f, -XM_PIDIV2, 0.0f);

		g_Key[BLUE_KEY].pos = XMFLOAT3(241.0f, KEY_OFFSET_Y, 420.0f);
		g_Key[BLUE_KEY].rot = XMFLOAT3(0.0f, -XM_PIDIV2, 0.0f);

		g_Key[YELLOW_KEY].pos = XMFLOAT3(-50.0f, KEY_OFFSET_Y, 240.0f);
		g_Key[YELLOW_KEY].rot = XMFLOAT3(0.0f, XM_PI, 0.0f);

	}

	// �`���[�g���A�����̌���POS�w��
	if (GetMode() == MODE_TUTORIAL)
	{
		g_Key[RED_KEY].pos = XMFLOAT3(-420.0f, KEY_OFFSET_Y, -420.0f);
		g_Key[RED_KEY].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

		g_Key[BLUE_KEY].pos = XMFLOAT3(420.0f, KEY_OFFSET_Y, -420.0f);
		g_Key[BLUE_KEY].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);

		g_Key[YELLOW_KEY].pos = XMFLOAT3(420.0f, KEY_OFFSET_Y, 420.0f);
		g_Key[YELLOW_KEY].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitKey(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_KEY; i++)
	{
		if (g_Key[i].load)
		{
			UnloadModel(&g_Key[i].model);
			g_Key[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateKey(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawKey(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O�L��
	SetCullingMode(CULL_MODE_BACK);

	// ���C���[�t���[���X�C�b�`�L��
	SelectWireFrameMode();

	for (int i = 0; i < MAX_KEY; i++)
	{
		if (g_Key[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Key[i].scl.x, g_Key[i].scl.y, g_Key[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Key[i].rot.x, g_Key[i].rot.y + XM_PI, g_Key[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Key[i].pos.x, g_Key[i].pos.y, g_Key[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Key[i].mtxWorld, mtxWorld);

		// �t�`�����点�Ă���
		SetFuchi(1);

		// ���f���`��
		DrawModel(&g_Key[i].model);

		SetFuchi(0);

	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

	// ���C���[�t���[���ݒ��߂�
	SetWireFrameMode(WIRE_FRAME_MODE_NONE);
}



//=============================================================================
// �Q�[�g�̎擾
//=============================================================================
KEY *GetKey()
{
	return &g_Key[0];
}

