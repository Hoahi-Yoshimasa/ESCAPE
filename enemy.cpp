//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "player.h"
#include "debugproc.h"
#include "collision.h"
#include "coord_point.h"
#include "light.h"

#include "fade.h"
#include "pathfind.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy.obj"		// �ǂݍ��ރ��f����

#define	SEARCH_MOVE_SPEED			(0.80f)				// �T�����[�h���̈ړ���
#define	CHASE_MOVE_SPEED			(1.0f)				// �`�F�C�X���[�h���̈ړ���

#define ENEMY_SHADOW_SIZE			(0.4f)				// �e�̑傫��
#define ENEMY_ANIME_SPEED			(0.05f)				// �G�l�~�[�̃A�j���[�V�������x
#define ENEMY_RISE_LIMIT			(30.0f)				// �G�l�~�[�̃A�j���[�V�����㏸�ő�l
#define ENEMY_DESCENDING_LIMIT		(25.0f)				// �G�l�~�[�̃A�j���[�V�������~�ő�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[

static BOOL				g_Load = FALSE;


static float g_Angle;			// �G�l�~�[�ƃv���C���[�Ƃ̃x�N�g���ƃG�l�~�[�����Ă���x�N�g���̊p�x�ۑ��p
static float anime_speed;		// �G�l�~�[�̃A�j���[�V�������x�p�ϐ�

static int g_NextPoint;			// �o�H�T���p ���̃|�C���g������ϐ�

static BOOL g_Enemy_Move;		// �����鎀�_�ړ��X�C�b�` �f�o�b�O�p

// ���U���g��ʗp�̐��`��ԃf�[�^
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame

	{ XMFLOAT3(180.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 5 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 3 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(XM_PIDIV4, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y, 200.0f),XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
};

// �Q�[���I�[�o�[��ʗp�̐��`��ԃf�[�^
static INTERPOLATION_DATA move_tbl2[] = {	// pos, rot, scl, frame

	{ XMFLOAT3(-50.0f, 0.0f, 0.0f),             XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 5 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 0.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 0.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 0.0f),   XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 60.0f),  XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PIDIV2, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y,130.0f), XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 }, // ��]
	{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y, 200.0f),XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(0.3f, 0.3f, 0.3f), 60 * 2 },
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(25.0f, ENEMY_OFFSET_Y, 425.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(0.3f, 0.3f, 0.3f);

		g_Enemy[i].chase_mode = FALSE;	// TRUE�Ȃ�ǂ������郂�[�h�ɂȂ�

		g_Enemy[i].angle = 0.0f;
		g_Enemy[i].p_dist = 0.0f;		// �v���C���[�܂ł̋���


		anime_speed = ENEMY_ANIME_SPEED;	// �G�l�~�[�̃A�j���[�V�������x�p�ϐ�


		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);


		g_Enemy[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Enemy[i].use = TRUE;			// TRUE:�����Ă�

		// �^�C�g����ʁE���U���g��ʁE�Q�[���I�[�o�[��ʂ�POS
		if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
		{
			g_Enemy[i].pos = XMFLOAT3(-50.0f, ENEMY_OFFSET_Y, 130.0f);
		}
	}

	// ���`��Ԃœ������Ă�
	if (GetMode() == MODE_RESULT)	// ���U���g�p
	{
		g_Enemy[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	}

	if (GetMode() == MODE_GAMEOVER)	// �Q�[���I�[�o�[��ʗp
	{
		g_Enemy[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[0].tbl_adr = move_tbl2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Enemy[0].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	}

	g_Enemy_Move = FALSE;

	g_Angle = 0;
	g_NextPoint = 0;			// �o�H�T���p ���̃|�C���g������ϐ� 0�ŏ�����

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)	// ���U���g��ʂƃQ�[���I�[�o�[��ʎ��̂ݐ��`���
			{
				PrintDebugProc("enemy:move_time: %f\n", g_Enemy[i].move_time);

				if (g_Enemy[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
				{								// ���`��Ԃ̏���
					// �ړ�����
					int		index = (int)g_Enemy[i].move_time;
					float	time = g_Enemy[i].move_time - index;
					int		size = g_Enemy[i].tbl_size;

					float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
					g_Enemy[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

					if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
					{
						g_Enemy[i].move_time = 0.0f;
						index = 0;
					}

					// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
					XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
					XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
					XMVECTOR vec = p1 - p0;
					XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

					// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
					XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// ���̊p�x
					XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
					XMVECTOR rot = r1 - r0;
					XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

					// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
					XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// ����Scale
					XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// ���݂�Scale
					XMVECTOR scl = s1 - s0;
					XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

				}

			}

			PLAYER *player = GetPlayer();

			if (GetMode() == MODE_GAME)							// �Q�[�����[�h�̎��̓���
			{
				// �G�l�~�[�ƃv���C���[�̋���(�x�N�g��)�����߂�
				XMFLOAT3 dist;									// �����ۑ��p

				XMVECTOR p = XMLoadFloat3(&player[0].pos);		// �v�Z�p�Ɉ�xXMVECTOR�ɓ����
				XMVECTOR e = XMLoadFloat3(&g_Enemy[i].pos);
				XMVECTOR d = p - e;								// �������v�Z
				XMStoreFloat3(&dist, d);						// dist�ɖ߂�

				// �G�l�~�[�����Ă������
				XMFLOAT3 looking_direction = { sinf(g_Enemy[0].rot.y),0.0f,cosf(g_Enemy[0].rot.y) };//�G�l�~�[�̌��Ă������(�x�N�g��)


				// �G�l�~�[�ƃv���C���[�̃x�N�g���ƃG�l�~�[�����Ă�������̃x�N�g���̓��όv�Z
				float dot;										// ���ϕۑ��p

				XMVECTOR d1 = XMLoadFloat3(&dist);				// �G�l�~�[�ƃv���C���[�̃x�N�g�� 1
				XMVECTOR l = XMLoadFloat3(&looking_direction);	// �G�l�~�[�����Ă���x�N�g�� 2
				dot = dotProduct(&d, &l);						// ���όv�Z�������ʂ�ۑ�����

				float len = VectorMagnitude(&d1);				// �G�l�~�[�ƃv���C���[�̃x�N�g���̒�����ۑ�
				float len2 = VectorMagnitude(&l);				// �G�l�~�[�����Ă���x�N�g���̒�����ۑ�

				g_Enemy[i].p_dist = len;						// �G�l�~�[����v���C���[�܂ł̋�����ۑ�


				// ��L�Q�̃x�N�g���̊p�x�̌v�Z���s��

				float angle_cos = dot / (len*len2); // �p�x��cos�����߂�

				// �����_���P�Ɓ|�P�̊ԂɌŒ肷��
				if (angle_cos > 1)
				{
					angle_cos = 1;
				}
				if (angle_cos < -1)
				{
					angle_cos = -1;
				}

				g_Angle = acosf(angle_cos); // �p�x�̌v�Z���s��


				float radian = XMConvertToRadians(30);			// �G�l�~�[�̃T�[�`�͈�30�������W�A���ɕϊ�

				if (g_Angle > XM_PI - radian && len < 200.0f)	// �G�l�~�[�����Ă����������ɍ��E�p�x30�x�ȓ��ɓ������� ��������200�ȓ��Ȃ�
				{
					g_Enemy[i].chase_mode = TRUE;				// �`�F�C�X���[�h��ON(TRUE)�ɂȂ�
					g_Enemy[i].spd = CHASE_MOVE_SPEED;			// �`�F�C�X���[�h�p�̃X�s�[�h�ݒ�
				}
				else
				{
					g_Enemy[i].chase_mode = FALSE;				// �`�F�C�X���[�h��OFF(FALSE)�ɂȂ�
					g_Enemy[i].spd = SEARCH_MOVE_SPEED;			// �ʏ탂�[�h�p�̃X�s�[�h�ݒ�
				}


				//�G�l�~�[��Y��]�� 6.28(-6.28)�𒴂�����0�ɖ߂�
				if (g_Enemy[i].rot.y > XM_2PI || g_Enemy[i].rot.y < -XM_2PI)
				{
					g_Enemy[i].rot.y = 0.0f;
				}

#ifdef _DEBUG
				// �f�o�b�O�p�G�l�~�[�ړ��X�C�b�`
				g_Enemy_Move = g_Enemy_Move % 2;

				if (GetKeyboardTrigger(DIK_1))
				{
					g_Enemy_Move++;
				}
				PrintDebugProc("�G�l�~�[MOVE:%d\n", g_Enemy_Move);
#endif

				int *P_chip_num = GetPlayer_chip_num();			// ���݃v���C���[�����b�V���t�B�[���h�̉���(���Ԗ�)�ɂ��邩��Get����
				int *E_chip_num = GetEnemy_chip_num();

				if (g_Enemy_Move == FALSE)							// �G�l�~�[�ړ��X�C�b�`ON�Ȃ�
				{
					if (g_Enemy[i].chase_mode == TRUE)				// �`�F�C�X���[�h�Ȃ�v���C���[�𒼐ڒǂ�������
					{
						// �v���C���[�̕����ɃG�l�~�[����]����i���������j

						// �v���C���[�ƃG�l�~�[�̃x�N�g���̌v�Z����
						XMFLOAT3 relVec3 = { player[0].pos.x - g_Enemy[i].pos.x, 0.0f, player[0].pos.z - g_Enemy[i].pos.z };

						// �x�N�g����Z�A�N�V�X�Ԃ̊p�x�̌v�Z���� (���W�A��)

						float angle2 = atan2f(relVec3.z * -1, relVec3.x) * (360.0f / (XM_PI * 2.0f));	// �x�N�g����Z�A�N�V�X�̊p�x�����߂�
						angle2 += -90.0f;																// -90���I�t�Z�b�g����
						float angleRadian = angle2 * XM_PI / 180.0f;									// �p�x�����W�A���ɕϊ�����
						g_Enemy[i].rot.y = angleRadian;													// �p�x�̌��ʂ��G�l�~�[��Rot��y�ɓ��ꍞ��


						// �v���C���[�̂Ƃ���Ɉړ����鏈��
						XMFLOAT3 difference;		// �����v�Z�p
						XMVECTOR P_pos = XMLoadFloat3(&player[0].pos);									// ��x�v�Z�p�ŕϊ�����
						XMVECTOR E_pos = XMLoadFloat3(&g_Enemy[i].pos);
						XMVECTOR difference1 = XMLoadFloat3(&difference);

						difference1 = P_pos - E_pos;													// �����ƃv���C���[�Ƃ̍��������߂Ă���
						XMStoreFloat3(&difference, difference1);										// ���ʂ���x�ۑ�����(�߂�)
						float angle1 = atan2f(difference.z, difference.x);								// ���̍������g���Ċp�x�����߂Ă���

						g_Enemy[i].pos.x += cosf(angle1) * g_Enemy[i].spd;								// angle�̕����ֈړ�
						g_Enemy[i].pos.z += sinf(angle1) * g_Enemy[i].spd;								// angle�̕����ֈړ�
					}
					else																				// �`�F�C�X���[�h�ȊO�̏ꍇ (�o�H�T��������)
					{
						// �ŒZ�o�H�T������
						COORD_POINT *coord_point = GetCoordPoint();										// �e���b�V���t�B�[���h�̒��S���W���W���擾
						g_NextPoint = ShortestPath(*E_chip_num, *P_chip_num);							// �ŒZ�o�H ���̏ꏊ��Ԃ�


						// NextPos�����ɃG�l�~�[����]����i���������j

						// NextPos�ƃG�l�~�[�̃x�N�g���̌v�Z����
						XMFLOAT3 relVec3 = { coord_point[g_NextPoint].pos.x - coord_point[*E_chip_num].pos.x, 0.0f, coord_point[g_NextPoint].pos.z - coord_point[*E_chip_num].pos.z };

						// �x�N�g����Z�A�N�V�X�Ԃ̊p�x�̌v�Z���� (���W�A��)
						float angle2 = atan2f(relVec3.z * -1, relVec3.x) * (360.0f / (XM_PI * 2.0f));	// �x�N�g����Z�A�N�V�X�̊p�x�����߂�
						angle2 += -90.0f;																// -90���I�t�Z�b�g����
						float angleRadian = angle2 * XM_PI / 180.0f;									// �p�x�����W�A���ɕϊ�����
						g_Enemy[i].rot.y = angleRadian;													// �p�x�̌��ʂ��G�l�~�[��Rot��y�ɓ��ꍞ��


						// NextPoint�Ɉړ����鏈��
						XMFLOAT3 difference;															// �����v�Z�p
						XMVECTOR Next_pos = XMLoadFloat3(&coord_point[g_NextPoint].pos);				// ��x�v�Z�p�ŕϊ�����
						XMVECTOR E_pos = XMLoadFloat3(&g_Enemy[i].pos);
						XMVECTOR difference1 = XMLoadFloat3(&difference);

						difference1 = Next_pos - E_pos;													// �G�l�~�[��Next�|�C���g�Ƃ̍��������߂Ă���
						XMStoreFloat3(&difference, difference1);										// ���ʂ���x�ۑ�����(�߂�)

						float angle1 = atan2f(difference.z, difference.x);								// ���̍������g���Ċp�x�����߂Ă���

						g_Enemy[i].pos.x += cosf(angle1) * g_Enemy[i].spd;								// angle�̕����ֈړ�
						g_Enemy[i].pos.z += sinf(angle1) * g_Enemy[i].spd;								// angle�̕����ֈړ�
					}
				}
			}

			// �G�l�~�[���㉺�ɃA�j���[�V�������鏈��
			g_Enemy[i].pos.y += anime_speed;
			if (g_Enemy[i].pos.y > ENEMY_RISE_LIMIT || g_Enemy[i].pos.y < ENEMY_DESCENDING_LIMIT)
			{
				anime_speed *= -1;	// �㉺���]����
			}

			{	// �|�C���g���C�g
				LIGHT *light = GetLightData(1);
				XMFLOAT3 pos = g_Enemy[i].pos;
				pos.y += 0.0f;

				light->Position = pos;
				light->Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
				light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				light->Type = LIGHT_TYPE_POINT;
				light->Enable = TRUE;
				SetLightData(1, light);
			}

		}

	}


#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("P��E�̊p�x:%f\n", g_Angle);
	PrintDebugProc("Enemy:X:%f Y:%f Z:%f rot.y:%f\n", g_Enemy[0].pos.x, g_Enemy[0].pos.y, g_Enemy[0].pos.z, g_Enemy[0].rot);
	PrintDebugProc("�G�l�~�[����v���C���[�܂ł̋���:%f\n", g_Enemy[0].p_dist);
	PrintDebugProc("Enemy_rot:X:%f Y:%f Z:%f\n", g_Enemy[0].rot.x, g_Enemy[0].rot.y, g_Enemy[0].rot.z);
	PrintDebugProc("chase_mode:%d\n", g_Enemy[0].chase_mode);
	PrintDebugProc("�ŒZ�o�H NEXT�|�C���g:%d\n", g_NextPoint);


#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���C���[�t���[���X�C�b�`�L��
	SelectWireFrameMode();

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Enemy[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

	// ���C���[�t���[���ݒ��߂�
	SetWireFrameMode(WIRE_FRAME_MODE_NONE);

}


//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}

