//=============================================================================
//
// �`���[�g���A����ʏ��� [tutorial.cpp]
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
#include "tutorial.h"
#include "player.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "collision.h"
#include "debugproc.h"
#include "stamina_bar.h"
#include "ui.h"
#include "gate.h"
#include "item_key.h"
#include "time.h"
#include "shadow_renderer.h"
#include "Coord_Point.h"
#include "key_icon.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHitTutorial(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(void)
{

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), MESHFIELD_BlockX, MESHFIELD_BlockZ, MESHFIELD_WIDTH, MESHFIELD_HEIGHT); // 13

	// ���W�|�C���g�\���̏�����
	InitCoordPoint();

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �ǂ̏�����
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 12, 3, 80.0f, 80.0f);


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

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_TUTORIAL);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
{
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

	// ���̏I������
	UninitKey();

	// �ǂ̏I������
	UninitMeshWall();

	// ���W�|�C���g�\���̏I������
	UninitCoordPoint();

	// �n�ʂ̏I������
	UninitMeshField();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTutorial(void)
{

	if (g_bPause == FALSE)
		return;

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// ���W�|�C���g�\���̍X�V
	UpdateCoordPoint();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �Q�[�g�̍X�V����
	UpdateGate();

	// ���̍X�V����
	UpdateKey();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHitTutorial();

	// �^�C���̍X�V����
	UpdateTime();

	// UI�̍X�V����
	UpdateUi();

	// �X�^�~�i�Q�[�W�̍X�V����
	UpdateStaminaBar();

	// ���A�C�R���̍X�V����
	UpdateKeyIcon();

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial0(void)
{
	SetSMRenderer();  // �e�̕`��

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �����܂ł͉e�t��

	SetRenderer();	// �ʏ�`��


	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

#ifdef _DEBUG // �f�o�b�O�p
	// ���W�|�C���g�\���̍X�V����
	DrawCoordPoint();
#endif

	// �Q�[�g�̕`�揈��
	DrawGate();

	// ���̕`�揈��
	DrawKey();

	// �e�̕`�揈��
	DrawShadow();

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

void DrawTutorial(void)
{
	XMFLOAT3 pos;

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();
	DrawTutorial0();

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHitTutorial(void)
{
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	GATE *gate = GetGate();			// �Q�[�g�̃|�C���^�[��������
	KEY *key = GetKey();			// ���̃|�C���^�[��������
	UI *ui = GetUi();				// UI�̃|�C���^�[��������
	int *P_chip_num = GetPlayer_chip_num();	// ���݃v���C���[�����b�V���t�B�[���h�̉���(���Ԗ�)�ɂ��邩��Get����


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
			SetFade(FADE_OUT, MODE_TITLE);
		}
	}

	// ���b�V���t�B�[���h�̏��ԍ��œ����蔻����s��(���̏��̏�ɂ͏�Q���Ȃǂ��Z�b�g����)
	// �v�f�ԍ��̓v���C���[�����ݗ����Ă��郁�b�V���t�B�[���h�̏��ԍ�
	COORD_POINT *coord_point = GetCoordPoint();
	
	if (coord_point[*P_chip_num].move == -1)
	{
		player[0].pos = player[0].old_pos;		// 1�t���[���O�ɖ߂�
	}

}


