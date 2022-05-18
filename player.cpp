//=============================================================================
//
// ���f������ [player.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "sound.h"
#include "ui.h"
#include "alpha_enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player.obj"		// �ǂݍ��ރ��f����

#define	WALKING_SPEED		(0.9f)						// �����ړ���
#define	RUN_SPEED			(2.0f)						// ����ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define PLAYER_SHADOW_SIZE	(1.0f)						// �e�̑傫��
#define PLAYER_OFFSET_Y		(20.0f)						// �v���C���[�̑��������킹��

#define PLAYER_PARTS_MAX	(1)							// �v���C���[�̃p�[�c�̐�
#define PLAYER_STAMINA_RECOVERY	(0.4f)					// �v���C���[�̃X�^�~�i�񕜗�
#define PLAYER_STAMINA_RECOVERY_TIME	(120)			// �v���C���[�̃X�^�~�i�񕜎���
#define PLAYER_STAMINA_REDUNCTION		(0.6f)			// �v���C���[�̃X�^�~�i������ (�񕜗ʂ��傫���l������)

#define DEFAULT_HEARTBEAT				(120)			// �v���C���[�f�t�H���g�S����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static BOOL			g_Load = FALSE;

static int g_Walksound_count;			// �������ϓ��ȊԊu�Ŗ炷�p�̃J�E���g�ϐ�
static int g_StaminaRecovery_count;		// �X�^�~�i�񕜎��Ԓ���

static int g_Text_count;				// �`���[�g���A���e�L�X�g�p�J�E���g�ϐ�

static int g_Heartbeat_count;			// �S�����̃J�E���g�ϐ�

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;
	g_Walksound_count = 0;									// �������ϓ��ȊԊu�Ŗ炷�p�̃J�E���g�ϐ�
	g_Heartbeat_count = DEFAULT_HEARTBEAT;					// �S�����̃J�E���g�ϐ� ������

	g_Player.pos = { -375.0f, PLAYER_OFFSET_Y, -375.0f };
	g_Player.old_pos = { 0.0f, 0.0f, 0.0f };				// 1�t���[���O�̍��W��ۑ����Ă����p
	g_Player.rot = { 0.0f, XM_PI, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.stamina = PLAYER_STAMINA;						// �X�^�~�i�̏�����
	g_Player.staminaPercent = 1.0f;							// HP��100���ŏ�����
	g_StaminaRecovery_count = 0;							// �X�^�~�i�񕜎��Ԓ���

	g_Player.spd = 0.0f;									// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;							// �����蔻��̑傫��
	g_Player.keyNumber = 0;									// ���̎擾�� 0�ŏ���������

	g_Player.use = TRUE;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�

	g_Text_count = 6;	// �`���[�g���A���̃e�L�X�g�\���J�n�ԍ���6�Ȃ̂�6�ŏ��������Ă���

	// �`���[�g���A�����̃v���C���[���W
	if (GetMode() == MODE_TUTORIAL)
	{
		g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, -400.0f };
	}
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();
	UI * ui = GetUi();

	g_Player.old_pos = g_Player.pos;		// �����蔻��p��1�t���[���O�̍��W��ۑ����Ă���

	// �����Ă��鎞�̂ݑ���\
	if (g_Player.use == TRUE)
	{
		// �L�[�{�[�h����
		if (GetKeyboardPress(DIK_A))
		{	// ���ֈړ�
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.x += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (GetKeyboardPress(DIK_D))
		{	// �E�ֈړ�
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z += sinf(g_Player.rot.y) * (g_Player.spd);
			g_Player.pos.x -= cosf(g_Player.rot.y) * (g_Player.spd);
		}

		if (GetKeyboardPress(DIK_W))
		{	// ��ֈړ�
			g_Player.spd = WALKING_SPEED;
			// W�L�[��������L_SHIFT����������ړ��ʕω�(�_�b�V��)����
			if (GetKeyboardPress(DIK_LSHIFT) && g_Player.stamina > 0 && g_StaminaRecovery_count == 0)
			{
				g_Player.spd = RUN_SPEED;
				g_Player.stamina -= PLAYER_STAMINA_REDUNCTION;			// �X�^�~�i������
			}
			g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
		}


		if (GetKeyboardPress(DIK_S))
		{	// ���ֈړ�
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.x += sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (GetKeyboardPress(DIK_RIGHT))
		{// ���_����u�E�v
			g_Player.rot.y += VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y > XM_PI)
			{
				g_Player.rot.y -= XM_PI * 2.0f;
			}
		}

		if (GetKeyboardPress(DIK_LEFT))
		{// ���_����u���v
			g_Player.rot.y -= VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y < -XM_PI)
			{
				g_Player.rot.y += XM_PI * 2.0f;
			}
		}


		// �R���g���[���[����
		if (IsButtonPressed(0, BUTTON_LEFT))
		{	// ���ֈړ�
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.x += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (IsButtonPressed(0, BUTTON_RIGHT))
		{	// �E�ֈړ�
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.z += sinf(g_Player.rot.y) * (g_Player.spd);
			g_Player.pos.x -= cosf(g_Player.rot.y) * (g_Player.spd);
		}

		if (IsButtonPressed(0, BUTTON_UP))
		{	// ��ֈړ�
			g_Player.spd = WALKING_SPEED;
			// W�L�[��������L_SHIFT����������ړ��ʕω�(�_�b�V��)����
			if (IsButtonPressed(0, BUTTON_R) && g_Player.stamina > 0 && g_StaminaRecovery_count == 0)
			{
				g_Player.spd = RUN_SPEED;
				g_Player.stamina -= PLAYER_STAMINA_REDUNCTION;			// �X�^�~�i������
			}
			g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
		}


		if (IsButtonPressed(0, BUTTON_DOWN))
		{	// ���ֈړ�
			g_Player.spd = WALKING_SPEED;
			g_Player.pos.x += sinf(g_Player.rot.y) * g_Player.spd;
			g_Player.pos.z += cosf(g_Player.rot.y) * g_Player.spd;
		}

		if (IsButtonPressed(0, BUTTON_C) || IsButtonPressed(0, BUTTON_Z))
		{// ���_����u�E�v
			g_Player.rot.y += VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y > XM_PI)
			{
				g_Player.rot.y -= XM_PI * 2.0f;
			}
		}

		if (IsButtonPressed(0, BUTTON_A) || IsButtonPressed(0, BUTTON_Y))
		{// ���_����u���v
			g_Player.rot.y -= VALUE_ROTATE_CAMERA;
			if (g_Player.rot.y < -XM_PI)
			{
				g_Player.rot.y += XM_PI * 2.0f;
			}
		}


		PrintDebugProc("Player.rot.y:%f\n", g_Player.rot.y);


		// �`���[�g���A���̃e�L�X�g��\�������鏈��
		if (GetMode() == MODE_TUTORIAL)
		{
			if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_M))
			{
				g_Text_count++;
				if (g_Text_count > MAX_UI - 2)
				{
					g_Text_count = MAX_UI - 2;		// �`���[�g���A�������̍Ō�̉摜�ŌŒ肳����
				}
				ui[g_Text_count - 1].use = FALSE;	// ��O�̃e�L�X�g�摜������
				ui[g_Text_count].use = TRUE;		// ���݂̃e�L�X�g�摜��\������
			}
		}

		// �X�^�~�i���O�ɂȂ����瑖��Ȃ��悤�ɂ���J�E���g����
		if (g_Player.stamina < 0)
		{
			g_StaminaRecovery_count = PLAYER_STAMINA_RECOVERY_TIME;	// �񕜎���
		}
		g_StaminaRecovery_count--;

		if (g_StaminaRecovery_count < 0)
		{
			g_StaminaRecovery_count = 0;
		}

		PrintDebugProc("�X�^�~�i:%f\n", g_Player.stamina);

		// �����~�܂��Ă���Ƃ��́A�v���C���[�̕��s�X�s�[�h��WALKING_SPEED�ɖ߂�
		if (g_Player.old_pos.x == g_Player.pos.x && g_Player.old_pos.z == g_Player.pos.z)
		{
			g_Player.spd = WALKING_SPEED;
		}

		// �X�^�~�i�Q�[�W�̉񕜏���
		if (g_Player.spd == WALKING_SPEED)
		{
			if (g_StaminaRecovery_count > 0) // �X�^�~�i���g���؂�Ɖ񕜗ʂ��x���Ȃ�
			{
				g_Player.stamina += PLAYER_STAMINA_RECOVERY / 2.0f;	// �X�^�~�i�񕜗�
			}
			else
			{
				g_Player.stamina += PLAYER_STAMINA_RECOVERY;		// �X�^�~�i�񕜗�
				// �ő�l�܂ŉ񕜂����炻��ȏ�񕜂��Ȃ��悤�ɂ���
				if (g_Player.stamina > PLAYER_STAMINA_MAX)
				{
					g_Player.stamina = PLAYER_STAMINA_MAX;
				}
			}
		}

		PrintDebugProc("�X�^�~�i�J�E���g:%d\n", g_StaminaRecovery_count);
		PrintDebugProc("���̎擾��:%d\n", g_Player.keyNumber);

		// �ړ��������͂��������瑫����炷����
		if (g_Player.pos.x != g_Player.old_pos.x || g_Player.pos.z != g_Player.old_pos.z)
		{
			if (g_Walksound_count <= 0)
			{
				if (g_Player.spd == RUN_SPEED)
				{ // �����Ă���T�E���h��炷
					PlaySound(SOUND_LABEL_SE_RUN);
					g_Walksound_count = 35;
				}
				else if (g_Player.spd == WALKING_SPEED)
				{ // �����Ă���T�E���h��炷
					PlaySound(SOUND_LABEL_SE_Footsteps);
					g_Walksound_count = 35;
				}
			}
			g_Walksound_count--;
		}

		PrintDebugProc("�X�s�[�h:%f\n", g_Player.spd);
		PrintDebugProc("�X�s�[�h�J�E���g:%d\n", g_Walksound_count);

		// �X�^�~�i�o�[�̏���
		g_Player.staminaPercent = g_Player.stamina / PLAYER_STAMINA_MAX;		// �X�^�~�i�o�[�ׂ̈̏���


		// �v���C���[�ƃG�l�~�[�ɋ����ɂ���ĐS������(����������E�x��������)�ύX������

		ALPHA_ENEMY *alphaEnemy = GetAlphaEnemy();

		if (GetMode() == MODE_GAME)
		{

			// �����̃G�l�~�[�ƃv���C���[�̋�����350�ȏ�̎��A���Ԋu�ŐS������炷
			if (alphaEnemy[0].p_dist > 350)
			{
				if (g_Heartbeat_count <= 0)
				{
					PlaySound(SOUND_LABEL_SE_HEARTBEAT);
					g_Heartbeat_count = DEFAULT_HEARTBEAT;
				}
			}

			// �����̃G�l�~�[�ƃv���C���[�̋�����350�`200�̎��A60�t���[���Ɉ��S������炷
			if (alphaEnemy[0].p_dist < 350 && alphaEnemy[0].p_dist > 200)
			{
				if (g_Heartbeat_count > 60)		// �S�����������ɂ���Đ؂�ւ�鎞�Ƀ��O���o�Ȃ��悤��60�J�E���g�ȏ�Ȃ�60�J�E���g�ɂ���
				{
					g_Heartbeat_count = 60;
				}
				if (g_Heartbeat_count <= 0)
				{
					PlaySound(SOUND_LABEL_SE_HEARTBEAT);
					g_Heartbeat_count = 60;
				}
			}

			// �����̃G�l�~�[�ƃv���C���[�̋�����200�ȉ��̎��A30�t���[���Ɉ��S������炷
			if (alphaEnemy[0].p_dist < 200)	
			{
				if (g_Heartbeat_count > 30)		// �S�����������ɂ���Đ؂�ւ�鎞�Ƀ��O���o�Ȃ��悤��30�J�E���g�ȏ�Ȃ�30�J�E���g�ɂ���
				{
					g_Heartbeat_count = 30;
				}
				if (g_Heartbeat_count <= 0)
				{
					PlaySound(SOUND_LABEL_SE_HEARTBEAT);
					g_Heartbeat_count = 30;
				}
			}
		}


		// �`���[�g���A�����͈��̌ۓ���炷
		if (GetMode() == MODE_TUTORIAL)
		{
			if (g_Heartbeat_count <= 0)
			{
				PlaySound(SOUND_LABEL_SE_HEARTBEAT);
				g_Heartbeat_count = DEFAULT_HEARTBEAT;
			}
		}

		PrintDebugProc("�S�����̃J�E���g:%d\n", g_Heartbeat_count);

		g_Heartbeat_count--;	// �S�������Ȃ炷�J�E���g�p�ϐ��𖈃t���[������


		// ���C�L���X�g���đ����̍��������߂�
		XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// �Ԃ������|���S���̖@���x�N�g���i�����j
		XMFLOAT3 hitPosition;								// ��_
		hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// �O�ꂽ���p�ɏ��������Ă���
		bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
		g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;

		{	// �|�C���g���C�g
			LIGHT *light = GetLightData(1);
			XMFLOAT3 pos = g_Player.pos;
			pos.y += 20.0f;

			light->Position = pos;
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			light->Type = LIGHT_TYPE_POINT;
			light->Enable = TRUE;
			SetLightData(1, light);
		}


#ifdef _DEBUG	// �f�o�b�O����\������
		PrintDebugProc("Player:�� �� �� ���@Space\n");
		PrintDebugProc("Player:X:%f Y:%f Z:%f rot.y:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, g_Player.rot.y);
#endif

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O�L��
	SetCullingMode(CULL_MODE_BACK);

	// ���C���[�t���[���X�C�b�`�L��
	SelectWireFrameMode();

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �N�H�[�^�j�I���𔽉f
	XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	SetFuchi(0);

	// ���f���`��
	DrawModel(&g_Player.model);


	// �p�[�c�̊K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ��
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
		if (g_Parts[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ���f���`��
		DrawModel(&g_Parts[i].model);
	}

	SetFuchi(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

	// ���C���[�t���[���ݒ��߂�
	SetWireFrameMode(WIRE_FRAME_MODE_NONE);

}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

//=============================================================================
// �v���C���[�̃X�^�~�i�񕜎��Ԃ̏����擾
//=============================================================================
int *GetStaminaRecovery_count()
{
	return &g_StaminaRecovery_count;
}