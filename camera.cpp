//=============================================================================
//
// �J�������� [camera.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

// FPS�p
#define	POS_X_CAM			(0.0f)			// �J�����̏����ʒu(X���W)
#define	POS_Y_CAM			(33.0f)			// �J�����̏����ʒu(Y���W)
#define	POS_Z_CAM			(-70.0f)		// �J�����̏����ʒu(Z���W)


#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(10000.0f)										// �r���[���ʂ�FarZ�l
#define	PAN_UP_LIMIT	(50.0f)											// �J�����̏�����̏���l
#define	PAN_DOWN_LIMIT	(0.0f)											// �J�����̏�����̏���l

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAMERA			g_Camera;		// �J�����f�[�^

// �J�����̎��_
static INTERPOLATION_DATA cam_move_tbl[] = {	// pos, rot, �����_, frame
	{ XMFLOAT3(-50.0f, 10.0f, -250.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 5}, // �o���_
	{ XMFLOAT3(-50.0f, 10.0f, 60.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 20.0f, 60.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 30.0f, 80.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3}, //���[�v�J�n�n�_
	{ XMFLOAT3(-90.0f, 30.0f, 100.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-20.0f, 30.0f, 70.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 150.0f,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(115.0f, 150.0f, 70.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(160.0f, 150.0f,  200.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 200.0f,400.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-240.0f, 150.0f, 200.0f),XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 150.0f, 20.0f),  XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-40.0f, 33.0f, 200.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 20.0f, 60.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3},
	{ XMFLOAT3(-50.0f, 30.0f, 80.0f),   XMFLOAT3(0.0f, 0.0f, 0.0f),XMFLOAT3(-50.0f, 30.0f, 130.0f), 60 * 3}, // �I���_
};

//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	// FPS �p
	g_Camera.pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera.at = { 0.0f, 33.0f, 0.0f };
	g_Camera.up = { 0.0f, 1.0f, 0.0f };
	g_Camera.rot = { 0.0f, XM_PI, 0.0f };


	// �`���[�g���A���̏ꍇ�̐ݒ�
	if (GetMode() == MODE_TITLE)
	{
		g_Camera.at = { -40.0f, 33.0f, 200.0f };
	}

	// ���U���g��ʂƃQ�[���I�[�o�[��ʂ̃J�����ݒ�
	if (GetMode() == MODE_RESULT || GetMode() == MODE_GAMEOVER)
	{
		g_Camera.pos = { -50.0f, 40.0f, -30.0f };
		g_Camera.at = { -50.0f, 40.0f, 130.0f };
		g_Camera.up = { 0.0f, 1.0f, 0.0f };
		g_Camera.rot = { 0.0f, XM_PI, 0.0f };

	}

	// ���_�ƒ����_�̋������v�Z
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);

	// �J�������_
	g_Camera.move_time = 0.0f;				// ���`��ԗp�̃^�C�}�[���N���A
	g_Camera.tbl_adr = cam_move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Camera.tbl_size = sizeof(cam_move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

}


//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{
	PLAYER *player = GetPlayer();


	// �^�C�g����ʂł̓J�����̐��`��Ԃ��s��
	if (GetMode() == MODE_TITLE)
	{
		PrintDebugProc("Camera:move_time: %f\n", g_Camera.move_time);

		PrintDebugProc("Camera:ATx %f y;%f z;%f\n", g_Camera.at.x, g_Camera.at.y, g_Camera.at.z);
		PrintDebugProc("Camera:pos.x %f y;%f z;%f\n", g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z);

		if (g_Camera.tbl_adr != NULL)	// ���`��Ԃ����s����H
		{								// ���`��Ԃ̏���
			// �ړ�����
			int		index = (int)g_Camera.move_time;
			float	time = g_Camera.move_time - index;
			int		size = g_Camera.tbl_size;

			float dt = 1.0f / g_Camera.tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Camera.move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����Aindex3�֖߂�(���[�v�̂���)
			{
				g_Camera.move_time = 3.0f;
				index = 3;
			}

			// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			XMVECTOR p1 = XMLoadFloat3(&g_Camera.tbl_adr[index + 1].pos);	// ���̏ꏊ
			XMVECTOR p0 = XMLoadFloat3(&g_Camera.tbl_adr[index + 0].pos);	// ���݂̏ꏊ
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Camera.pos, p0 + vec * time);


			// �����_�����߂�	R = StartX + (EndX - StartX) * ���̎���
			XMVECTOR at1 = XMLoadFloat3(&g_Camera.tbl_adr[index + 1].scl);	// ���̊p�x
			XMVECTOR at0 = XMLoadFloat3(&g_Camera.tbl_adr[index + 0].scl);	// ���݂̊p�x
			XMVECTOR at = at1 - at0;
			XMStoreFloat3(&g_Camera.at, at0 + at * time);
		}

	}



	if (GetMode() == MODE_GAME || GetMode() == MODE_TUTORIAL)
	{
		// �v���C���[�������Ă��鎞�̂ݑ���\
		if (player[0].use == TRUE)
		{
			// FPS�p
			if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0, BUTTON_A) || IsButtonPressed(0, BUTTON_Y))
			{// �����_����u���v
				g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
				if (g_Camera.rot.y < -XM_PI)
				{
					g_Camera.rot.y += XM_PI * 2.0f;
				}
				g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
				g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
			}

			if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_C) || IsButtonPressed(0, BUTTON_Z))
			{// �����_����u�E�v
				g_Camera.rot.y += VALUE_ROTATE_CAMERA;
				if (g_Camera.rot.y > XM_PI)
				{
					g_Camera.rot.y -= XM_PI * 2.0f;
				}

				g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
				g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
			}

			if (GetKeyboardPress(DIK_UP) || IsButtonPressed(0, BUTTON_X))
			{// �����_�ړ��u��v
				g_Camera.at.y += VALUE_MOVE_CAMERA;
			}

			if (GetKeyboardPress(DIK_DOWN) || IsButtonPressed(0, BUTTON_B))
			{// �����_�ړ��u���v
				g_Camera.at.y -= VALUE_MOVE_CAMERA;
			}

			// �J����������l�ȏ�����Ȃ��悤�ɂ��鏈��
			if (g_Camera.at.y < PAN_DOWN_LIMIT)
			{
				g_Camera.at.y = PAN_DOWN_LIMIT;
			}
			if (g_Camera.at.y > PAN_UP_LIMIT)
			{
				g_Camera.at.y = PAN_UP_LIMIT;
			}

		}

		PrintDebugProc("Camera:AT.y %f\n", g_Camera.at.y);
		PrintDebugProc("Camera:pos.x %f y;%f z;%f\n", g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z);

#ifdef _DEBUG	// �f�o�b�O����\������
		PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif

	}
}


//=============================================================================
// �J�����̍X�V
//=============================================================================
void SetCamera(void)
{
	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA *GetCamera(void)
{
	return &g_Camera;
}


// �J�����̎��_�ƒ����_���Z�b�g
void SetCameraAT(XMFLOAT3 pos)
{
	if (GetMode() == MODE_GAME || GetMode() == MODE_TUTORIAL)
	{
		// FPS�p�J�����ݒ�
		g_Camera.pos = pos;					// �v���C���[��POS����J������POS�����킹��
		g_Camera.pos.y = POS_Y_CAM;			// �ڐ��̍����ɃI�t�Z�b�g����
		//g_Camera.pos.z += POS_OFFSET_Z_CAM; // �����J���������ɉ����Ă���(�t�F���X�ƏՓ˂������|���S���̌������������Ă��܂��̂�h������)

		// �J�����̎��_���J������Y����]�ɑΉ������Ă���
		g_Camera.at.x = g_Camera.pos.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}
}

