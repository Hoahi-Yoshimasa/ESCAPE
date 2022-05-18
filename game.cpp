//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "collision.h"
#include "debugproc.h"
#include "fence.h"
#include "stamina_bar.h"
#include "ui.h"
#include "gate.h"
#include "item_key.h"
#include "time.h"
#include "rock.h"
#include "hut.h"
#include "shadow_renderer.h"
#include "Coord_Point.h"
#include "effect.h"
#include "alpha_enemy.h"
#include "key_icon.h"

#include "nodequeue.h"
#include "pathfind.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{

	// �^�C�g����ʁE���U���g��ʁE�Q�[���I�[�o�[��ʂׂ̈̏���������
	if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		// �t�B�[���h�̏�����
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), MESHFIELD_BlockX, MESHFIELD_BlockZ, MESHFIELD_WIDTH, MESHFIELD_HEIGHT);

		// ���C�g��L����	// �e�̏���������
		InitShadow();

		// �G�l�~�[�̏�����
		InitEnemy();

		// �ǂ̏�����
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);

		// �����̏�����
		InitHut();

	}

	if (GetMode() == MODE_GAME)
	{
		// �t�B�[���h�̏�����
		InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), MESHFIELD_BlockX, MESHFIELD_BlockZ, MESHFIELD_WIDTH, MESHFIELD_HEIGHT); // 13

		// ���W�|�C���g�\���̏�����
		InitCoordPoint();

		// ���C�g��L����	// �e�̏���������
		InitShadow();

		// �v���C���[�̏�����
		InitPlayer();

		// �G�l�~�[�̏�����
		InitEnemy();

		// �o�H�T�������̏�����
		InitPathfind();

		// �����̃G�l�~�[�̏���������
		InitAlphaEnemy();

		// �ǂ̏�����
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
		InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);


		// �t�F���X�̏�����
		InitFence();

		// ��̏�����
		InitRock();

		// �����̏�����
		InitHut();

		// �Q�[�g�̏�����
		InitGate();

		// ���̏�����
		InitKey();

		// �^�C���̏�����
		InitTime();

		// �X�^�~�i�Q�[�W�̏�����
		InitStaminaBar();

		// UI�̏�����
		InitUi();

		// ���A�C�R���̏�����
		InitKeyIcon();

		// �G�t�F�N�g�̏�����
		InitEffect();

		// BGM�Đ�
		PlaySound(SOUND_LABEL_BGM_GAME);

	}

	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{

	// �G�t�F�N�g�̏I������
	UninitEffect();

	// ���A�C�R���̏I������
	UninitKeyIcon();

	// UI�̏I������
	UninitUi();

	// �X�^�~�i�Q�[�W�̏I������
	UninitStaminaBar();

	// �^�C���̏I������
	UninitTime();

	// �Q�[�g�̏I������
	UninitGate();

	// �����̏I������
	UninitHut();

	// ���̏I������
	UninitKey();

	// ��̏I������
	UninitRock();

	// �t�F���X�̏I������
	UninitFence();

	// �ǂ̏I������
	UninitMeshWall();

	// ���W�|�C���g�\���̏I������
	UninitCoordPoint();

	// �n�ʂ̏I������
	UninitMeshField();

	// �����̃G�l�~�[�̏I������
	UninitAlphaEnemy();

	// �o�H�T�������̏I������
	UninitPathfind();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{

	if (g_bPause == FALSE)
		return;

	if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		// �n�ʏ����̍X�V
		UpdateMeshField();

		// �G�l�~�[�̍X�V����
		UpdateEnemy();

		// �Ǐ����̍X�V
		UpdateMeshWall();

		// �����̍X�V����
		UpdateHut();

		// �e�̍X�V����
		UpdateShadow();

	}

	if (GetMode() == MODE_GAME)
	{
		// �v���C���[�̍X�V����
		UpdatePlayer();

		// �n�ʏ����̍X�V
		UpdateMeshField();

		// ���W�|�C���g�\���̍X�V
		UpdateCoordPoint();

		// �G�l�~�[�̍X�V����
		UpdateEnemy();

		// �����̃G�l�~�[�̍X�V����
		UpdateAlphaEnemy();

		// �Ǐ����̍X�V
		UpdateMeshWall();

		// �t�F���X�̍X�V����
		UpdateFence();

		// �����̍X�V����
		UpdateHut();

		// ��̍X�V����
		UpdateRock();

		// �Q�[�g�̍X�V����
		UpdateGate();

		// ���̍X�V����
		UpdateKey();

		// �e�̍X�V����
		UpdateShadow();

		// �����蔻�菈��
		CheckHit();

		// �^�C���̍X�V����
		UpdateTime();

		// UI�̍X�V����
		UpdateUi();

		// ���A�C�R���̍X�V����
		UpdateKeyIcon();

		// �X�^�~�i�Q�[�W�̍X�V����
		UpdateStaminaBar();

		// �G�t�F�N�g�̍X�V����
		UpdateEffect();
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// �^�C�g����ʁE���U���g��ʁE�Q�[���I�[�o�[��ʂׂ̈̕`�揈��
	if (GetMode() == MODE_TITLE || GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		SetSMRenderer();  // �e�̕`��

		// �G�l�~�[�̕`�揈��
		DrawEnemy();

		// �����̏�����
		DrawHut();

		// �����܂ł͉e�t��
		SetRenderer();	// �ʏ�`��

		// �G�l�~�[�̕`�揈��
		DrawEnemy();

		// �n�ʂ̕`�揈��
		DrawMeshField();

		// �����̏�����
		DrawHut();

		// �ǂ̕`�揈��
		DrawMeshWall();

		// �e�̕`�揈��
		DrawShadow();

	}

	if (GetMode() == MODE_GAME)
	{
		SetSMRenderer();  // �e�̕`��

		// �G�l�~�[�̕`�揈��
		DrawEnemy();

		// �v���C���[�̕`�揈��
		DrawPlayer();

		// ��̕`�揈��
		DrawRock();

		// �t�F���X�̕`�揈��
		DrawFence();

		// �����̏�����
		DrawHut();

		// �����܂ł͉e�t��

		SetRenderer();	// �ʏ�`��

		// 3D�̕���`�悷�鏈��
		// �n�ʂ̕`�揈��
		DrawMeshField();

#ifdef _DEBUG // �f�o�b�O�p
		// ���W�|�C���g�\���̍X�V����
		DrawCoordPoint();
#endif

		// �t�F���X�̕`�揈��
		DrawFence();

		// ��̕`�揈��
		DrawRock();

		// �����̏�����
		DrawHut();

		// �Q�[�g�̕`�揈��
		DrawGate();

		// ���̕`�揈��
		DrawKey();

		// �e�̕`�揈��
		DrawShadow();

		// �G�l�~�[�̕`�揈��
		DrawEnemy();

		// �����̃G�l�~�[�̕`�揈��
		DrawAlphaEnemy();

		// �v���C���[�̕`�揈��
		DrawPlayer();

		// �ǂ̕`�揈��
		DrawMeshWall();

		// ���A�C�R���̕`�揈��
		DrawKeyIcon();

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		// �G�t�F�N�g�̕`�揈��
		DrawEffect();

		// �^�C���̕`�揈��
		DrawTime();

		// UI�̕`�揈��
		DrawUi();

		// �X�^�~�i�Q�[�W�̕`�揈��
		DrawStaminaBar();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);

	}
}


void DrawGame(void)
{
	XMFLOAT3 pos;

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	//SetCameraAT(pos);
	SetCamera();
	DrawGame0();

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();						// �G�l�~�[�̃|�C���^�[��������
	ALPHA_ENEMY *alphaEnemy = GetAlphaEnemy();		// �����G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();					// �v���C���[�̃|�C���^�[��������
	GATE *gate = GetGate();							// �Q�[�g�̃|�C���^�[��������
	KEY *key = GetKey();							// ���̃|�C���^�[��������
	KEYICON *key_icon = GetKeyIcon();				// ���A�C�R���̃|�C���^�[��������
	UI *ui = GetUi();								// UI�̃|�C���^�[��������
	int *P_chip_num = GetPlayer_chip_num();			// ���݃v���C���[�����b�V���t�B�[���h�̉���(���Ԗ�)�ɂ��邩��Get����


	// ���_(�G�l�~�[)�ƃv���C���[�L����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// ���_(�G�l�~�[)�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE)
			continue;

		// BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size) && player[0].use == TRUE)
		{
			// �v���C���[�͓|�����
			player[0].use = FALSE;
			ReleaseShadow(player[0].shadowIdx);
			PlaySound(SOUND_LABEL_SE_HIT);		// �a���鉹(���ꉹ)
			SetFade(FADE_OUT, MODE_GAMEOVER);

		}
	}

	// �����̎��_(�G�l�~�[)�ƃv���C���[�L����
	for (int i = 0; i < MAX_ALPHA_ENEMY; i++)
	{
		// ���_(�G�l�~�[)�̗L���t���O���`�F�b�N����
		if (alphaEnemy[i].use == FALSE)
			continue;

		// BC�̓����蔻��
		if (CollisionBC(player->pos, alphaEnemy[i].pos, player->size, alphaEnemy[i].size) && player[0].use == TRUE)
		{
			// �v���C���[�͓|�����
			player[0].use = FALSE;
			ReleaseShadow(player[0].shadowIdx);
			PlaySound(SOUND_LABEL_SE_HIT);		// �a���鉹(���ꉹ)
			SetFade(FADE_OUT, MODE_GAMEOVER);
		}
	}


	// ���̎擾
	for (int i = 0; i < MAX_KEY; i++)
	{
		//�v���C���[�̗L���t���O���`�F�b�N����
		if (key[i].use == FALSE)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, key[i].pos, player->size, key[i].size))
		{
			PlaySound(SOUND_LABEL_SE_KEY_GET);	// �擾��
			player[0].keyNumber++;	// ���̎擾�����P���₷
			key[i].use = FALSE;
		}

		// �Ԃ�������肵����UI�̌��̃A�C�R����Z������
		if (key[0].use == FALSE)
		{
			ui[2].collected = 1.0f;
		}
		// ��������肵����UI�̌��̃A�C�R����Z������
		if (key[1].use == FALSE)
		{
			ui[3].collected = 1.0f;
		}
		// ���F��������肵����UI�̌��̃A�C�R����Z������
		if (key[2].use == FALSE)
		{
			ui[4].collected = 1.0f;
		}
	}

	// �v���C���[�ƒE�o�Q�[�g
	for (int i = 0; i < MAX_GATE; i++)
	{
		//�v���C���[�̗L���t���O���`�F�b�N����
		if (player[0].use == FALSE)
			continue;

		//BC�̓����蔻��
		// �����R�S�Ď�ɓ���Ă����烊�U���g�ɓ���
		if (CollisionBC(player->pos, gate[i].pos, player->size, gate[i].size) && player[0].keyNumber == MAX_KEY)
		{
			player[0].keyNumber = 0;				// ���̎擾����0�ɂ���
			PlaySound(SOUND_LABEL_SE_OPEN_DOOR);	// �����J���鉹
			SetFade(FADE_OUT, MODE_RESULT);
		}
	}

	// ���b�V���t�B�[���h�̏��ԍ��œ����蔻����s��(���̏��̏�ɂ͏�Q���Ȃǂ��Z�b�g����)
	// �v�f�ԍ��̓v���C���[�����ݗ����Ă��郁�b�V���t�B�[���h�̏��ԍ�
	COORD_POINT *coord_point = GetCoordPoint();
	
	if (coord_point[*P_chip_num].move == KEEP_OUT)
	{
		player[0].pos = player[0].old_pos;		// 1�t���[���O�ɖ߂�
	}

}


////=============================================================================
//// ���C���[�t���[�����[�h�̎擾
////=============================================================================
//int GetWireFrameMode(void)
//{
//	return g_WireFrameSwitch;
//}
