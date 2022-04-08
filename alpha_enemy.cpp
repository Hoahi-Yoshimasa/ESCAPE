//=============================================================================
//
// �����G�l�~�[���f������ [alpha_enemy.cpp]
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
#include "light.h"
#include "alpha_enemy.h"

#include "fade.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ALPHA_ENEMY			"data/MODEL/alpha_enemy.obj"	// �ǂݍ��ރ��f����

#define ALPHA_ENEMY_SHADOW_SIZE			(0.4f)					// �e�̑傫��
#define ALPHA_ENEMY_ANIME_SPEED			(0.05f)					// �G�l�~�[�̃A�j���[�V�������x
#define ALPHA_ENEMY_RISE_LIMIT			(30.0f)					// �G�l�~�[�̃A�j���[�V�����㏸�ő�l
#define ALPHA_ENEMY_DESCENDING_LIMIT	(25.0f)					// �G�l�~�[�̃A�j���[�V�������~�ő�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ALPHA_ENEMY		g_AlphaEnemy[MAX_ALPHA_ENEMY];				// �����̃G�l�~�[

static BOOL				g_Load = FALSE;


static float anime_speed;		// �G�l�~�[�̃A�j���[�V�������x�p�ϐ�

// ���U���g��ʗp�̐��`��ԃf�[�^
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-400.0f, ALPHA_ENEMY_OFFSET_Y,  425.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 }, // �J�n�n�_
	{ XMFLOAT3(-175.0f, ALPHA_ENEMY_OFFSET_Y,  425.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 },
	{ XMFLOAT3(-175.0f, ALPHA_ENEMY_OFFSET_Y,  275.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(125.0f,  ALPHA_ENEMY_OFFSET_Y,  275.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(125.0f,  ALPHA_ENEMY_OFFSET_Y, -175.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(-225.0f, ALPHA_ENEMY_OFFSET_Y, -175.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 10 },
	{ XMFLOAT3(-225.0f, ALPHA_ENEMY_OFFSET_Y,  225.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 },
	{ XMFLOAT3(-400.0f, ALPHA_ENEMY_OFFSET_Y,  225.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 }, 
	{ XMFLOAT3(-400.0f, ALPHA_ENEMY_OFFSET_Y,  425.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 6 }, // �J�n�n�_

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitAlphaEnemy(void)
{
	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		LoadModel(MODEL_ALPHA_ENEMY, &g_AlphaEnemy[i].model);
		g_AlphaEnemy[i].load = TRUE;

		g_AlphaEnemy[i].pos = XMFLOAT3(-375.0f, ALPHA_ENEMY_OFFSET_Y, 425.0f);
		g_AlphaEnemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_AlphaEnemy[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_AlphaEnemy[i].chase_mode = FALSE;			// TRUE�Ȃ�ǂ������郂�[�h�ɂȂ�

		g_AlphaEnemy[i].angle = 0.0f;
		g_AlphaEnemy[i].p_dist = 0.0f;				// �v���C���[�܂ł̋���


		anime_speed = ALPHA_ENEMY_ANIME_SPEED;		// �G�l�~�[�̃A�j���[�V�������x�p�ϐ�


		g_AlphaEnemy[i].spd = 0.0f;					// �ړ��X�s�[�h�N���A
		g_AlphaEnemy[i].size = ALPHA_ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_AlphaEnemy[0].model, &g_AlphaEnemy[0].diffuse[0]);

		XMFLOAT3 pos = g_AlphaEnemy[i].pos;
		pos.y -= (ALPHA_ENEMY_OFFSET_Y - 0.1f);
		g_AlphaEnemy[i].shadowIdx = CreateShadow(pos, ALPHA_ENEMY_SHADOW_SIZE, ALPHA_ENEMY_SHADOW_SIZE);


		g_AlphaEnemy[i].move_time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_AlphaEnemy[i].tbl_adr = NULL;				// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_AlphaEnemy[i].tbl_size = 0;				// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_AlphaEnemy[i].use = TRUE;					// TRUE:�����Ă�

	}

	// ���`��Ԃœ������Ă�
	if (GetMode() == MODE_GAME)
	{
		g_AlphaEnemy[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_AlphaEnemy[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_AlphaEnemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitAlphaEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		if (g_AlphaEnemy[i].load)
		{
			UnloadModel(&g_AlphaEnemy[i].model);
			g_AlphaEnemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateAlphaEnemy(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		if (g_AlphaEnemy[i].use == TRUE)				// ���̃G�l�~�[���g���Ă���H
		{												// Yes
			if (GetMode() == MODE_GAME)					// �Q�[�����[�h�̎��̓���
			{
				PrintDebugProc("����:move_time: %f\n", g_AlphaEnemy[i].move_time);

				if (g_AlphaEnemy[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
				{										// ���`��Ԃ̏���
					// �ړ�����
					int		index = (int)g_AlphaEnemy[i].move_time;
					float	time = g_AlphaEnemy[i].move_time - index;
					int		size = g_AlphaEnemy[i].tbl_size;

					float dt = 1.0f / g_AlphaEnemy[i].tbl_adr[index].frame;					// 1�t���[���Ői�߂鎞��
					g_AlphaEnemy[i].move_time += dt;										// �A�j���[�V�����̍��v���Ԃɑ���

					if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
					{
						g_AlphaEnemy[i].move_time = 0.0f;
						index = 0;
					}

					// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
					XMVECTOR p1 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
					XMVECTOR p0 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
					XMVECTOR vec = p1 - p0;
					XMStoreFloat3(&g_AlphaEnemy[i].pos, p0 + vec * time);

					// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
					XMVECTOR r1 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 1].rot);	// ���̊p�x
					XMVECTOR r0 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
					XMVECTOR rot = r1 - r0;
					XMStoreFloat3(&g_AlphaEnemy[i].rot, r0 + rot * time);

					// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
					XMVECTOR s1 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 1].scl);	// ����Scale
					XMVECTOR s0 = XMLoadFloat3(&g_AlphaEnemy[i].tbl_adr[index + 0].scl);	// ���݂�Scale
					XMVECTOR scl = s1 - s0;
					XMStoreFloat3(&g_AlphaEnemy[i].scl, s0 + scl * time);
				}


				PLAYER *player = GetPlayer();
				// �G�l�~�[�ƃv���C���[�̋���(�x�N�g��)�����߂�
				XMFLOAT3 dist;									// �����ۑ��p

				XMVECTOR p = XMLoadFloat3(&player[0].pos);		// �v�Z�p�Ɉ�xXMVECTOR�ɓ����
				XMVECTOR e = XMLoadFloat3(&g_AlphaEnemy[i].pos);
				XMVECTOR d = p - e;								// �������v�Z
				XMStoreFloat3(&dist, d);						// dist�ɖ߂�

				// �G�l�~�[�����Ă������
				XMFLOAT3 looking_direction = { sinf(g_AlphaEnemy[0].rot.y),0.0f,cosf(g_AlphaEnemy[0].rot.y) };//�G�l�~�[�̌��Ă������(�x�N�g��)

				// �G�l�~�[�ƃv���C���[�̃x�N�g���ƃG�l�~�[�����Ă�������̃x�N�g���̓��όv�Z
				float dot;										// ���ϕۑ��p

				XMVECTOR d1 = XMLoadFloat3(&dist);				// �G�l�~�[�ƃv���C���[�̃x�N�g�� 1
				XMVECTOR l = XMLoadFloat3(&looking_direction);	// �G�l�~�[�����Ă���x�N�g�� 2
				dot = dotProduct(&d, &l);						// ���όv�Z�������ʂ�ۑ�����

				float len = VectorMagnitude(&d1);				// �G�l�~�[�ƃv���C���[�̃x�N�g���̒�����ۑ�
				float len2 = VectorMagnitude(&l);				// �G�l�~�[�����Ă���x�N�g���̒�����ۑ�

				g_AlphaEnemy[i].p_dist = len;					// �G�l�~�[����v���C���[�܂ł̋�����ۑ�



				// ��L�Q�̃x�N�g���̊p�x�̌v�Z���s��
				float angle_cos = dot / (len*len2);				// �p�x��cos�����߂�

				// �����_���P�Ɓ|�P�̊ԂɌŒ肷��
				if (angle_cos > 1)
				{
					angle_cos = 1;
				}
				if (angle_cos < -1)
				{
					angle_cos = -1;
				}

				//�G�l�~�[��Y��]�� 6.28(-6.28)�𒴂�����0�ɖ߂�
				if (g_AlphaEnemy[i].rot.y > XM_2PI || g_AlphaEnemy[i].rot.y < -XM_2PI)
				{
					g_AlphaEnemy[i].rot.y = 0.0f;
				}


				// �v���C���[�̕����ɃG�l�~�[����]����i���������j

				// �v���C���[�ƃG�l�~�[�̃x�N�g���̌v�Z����
				XMFLOAT3 relVec3 = { player[0].pos.x - g_AlphaEnemy[i].pos.x, 0.0f, player[0].pos.z - g_AlphaEnemy[i].pos.z };

				// �x�N�g����Z�A�N�V�X�Ԃ̊p�x�̌v�Z���� (���W�A��)
				float angle2 = atan2f(relVec3.z * -1, relVec3.x) * (360.0f / (XM_PI * 2.0f)); // �x�N�g����Z�A�N�V�X�̊p�x�����߂�
				angle2 += -90.0f;								// -90���I�t�Z�b�g����
				float angleRadian = angle2 * XM_PI / 180.0f;	// �p�x�����W�A���ɕϊ�����

				g_AlphaEnemy[i].rot.y = angleRadian;			// �p�x�̌��ʂ��G�l�~�[��Rot��y�ɓ��ꍞ��

			}
		}

	}


#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("�����G�l�~�[����v���C���[�܂ł̋���:%f\n", g_AlphaEnemy[0].p_dist);
	PrintDebugProc("����Enemy_rot:X:%f Y:%f Z:%f\n", g_AlphaEnemy[0].rot.x, g_AlphaEnemy[0].rot.y, g_AlphaEnemy[0].rot.z);
	//PrintDebugProc("chase_mode:%d\n", g_AlphaEnemy[0].chase_mode);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawAlphaEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		if (g_AlphaEnemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_AlphaEnemy[i].scl.x, g_AlphaEnemy[i].scl.y, g_AlphaEnemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_AlphaEnemy[i].rot.x, g_AlphaEnemy[i].rot.y + XM_PI, g_AlphaEnemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_AlphaEnemy[i].pos.x, g_AlphaEnemy[i].pos.y, g_AlphaEnemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_AlphaEnemy[i].mtxWorld, mtxWorld);

		// ���̃G�l�~�[�͓����Ȃ̂�DrawModel���R�����g�A�E�g���Ă���
		// ���f���`��
		//DrawModel(&g_AlphaEnemy[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ALPHA_ENEMY *GetAlphaEnemy()
{
	return &g_AlphaEnemy[0];
}

