//=============================================================================
//
// ���A�C�R������ [key_icon.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "key_icon.h"
#include "player.h"
#include "debugproc.h"
#include "collision.h"
#include "item_key.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�

#define	KEYICON_WIDTH				(10.0f)			// ���_�T�C�Y
#define	KEYICON_HEIGHT				(10.0f)			// ���_�T�C�Y

#define	MAX_KEYICON					(3)				// �A�C�R���ő吔

#define KEYICON_RISE_LIMIT			(50.0f)			// �G�l�~�[�̃A�j���[�V�����㏸�ő�l
#define KEYICON_DESCENDING_LIMIT	(40.0f)			// �G�l�~�[�̃A�j���[�V�������~�ő�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexKeyIcon(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static KEYICON				g_KeyIcon[MAX_KEYICON];						// KEY_ICON
static int					g_TexNo;									// �e�N�X�`���ԍ�
static BOOL					g_bAlpaTest;								// �A���t�@�e�X�gON/OFF

static BOOL					g_Load = FALSE;

static float anime_speed;												// �G�l�~�[�̃A�j���[�V�������x�p�ϐ�


static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/UI/keyIcon.png",
};

// ���[�h�Q�[���p
static INTERPOLATION_DATA move_tbl_game_red[] = {		// �Ԃ��� pos, rot, scl, frame
	{ XMFLOAT3(293.0f, KEYICON_RISE_LIMIT,       -240.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // �J�n�n�_
	{ XMFLOAT3(293.0f, KEYICON_DESCENDING_LIMIT, -240.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(293.0f, KEYICON_RISE_LIMIT,       -240.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_game_blue[] = {		// ���� pos, rot, scl, frame
	{ XMFLOAT3(245.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // �J�n�n�_
	{ XMFLOAT3(245.0f, KEYICON_DESCENDING_LIMIT,  420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(245.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_game_yellow[] = {	// ���F���� pos, rot, scl, frame
	{ XMFLOAT3(-50.0f, KEYICON_RISE_LIMIT,        243.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // �J�n�n�_
	{ XMFLOAT3(-50.0f, KEYICON_DESCENDING_LIMIT,  243.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-50.0f, KEYICON_RISE_LIMIT,        243.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};



// �`���[�g���A���p
static INTERPOLATION_DATA move_tbl_tutorial_red[] = {		// �Ԃ��� pos, rot, scl, frame
	{ XMFLOAT3(-420.0f, KEYICON_RISE_LIMIT,       -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // �J�n�n�_
	{ XMFLOAT3(-420.0f, KEYICON_DESCENDING_LIMIT, -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(-420.0f, KEYICON_RISE_LIMIT,       -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_tutorial_blue[] = {		// ���� pos, rot, scl, frame
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // �J�n�n�_
	{ XMFLOAT3(420.0f, KEYICON_DESCENDING_LIMIT,  -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        -420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};
static INTERPOLATION_DATA move_tbl_tutorial_yellow[] = {	// ���F���� pos, rot, scl, frame
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 }, // �J�n�n�_
	{ XMFLOAT3(420.0f, KEYICON_DESCENDING_LIMIT,  420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
	{ XMFLOAT3(420.0f, KEYICON_RISE_LIMIT,        420.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1 },
};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitKeyIcon(void)
{
	MakeVertexKeyIcon();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// ���A�C�R���̏�����
	for (int i = 0; i < MAX_KEYICON; i++)
	{
		ZeroMemory(&g_KeyIcon[i].material, sizeof(g_KeyIcon[i].material));
		g_KeyIcon[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_KeyIcon[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_KeyIcon[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_KeyIcon[i].fWidth = KEYICON_WIDTH;
		g_KeyIcon[i].fHeight = KEYICON_HEIGHT;
		g_KeyIcon[i].bUse = FALSE;
		g_KeyIcon[i].p_dist = 0.0f;		// �v���C���[�܂ł̋���

		g_KeyIcon[i].move_time = 0.0f;	// ���`��ԗp�̃^�C�}�[���N���A
		g_KeyIcon[i].tbl_adr = NULL;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_KeyIcon[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	}

	g_bAlpaTest = TRUE;

	// ���A�C�R���̈ʒu�̐ݒ�
	SetKeyIcon(XMFLOAT3(293.0f, KEYICON_OFFSET_Y, -240.0f), 10.0f, 10.0f, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));	// �Ԃ����̃A�C�R��
	SetKeyIcon(XMFLOAT3(244.0f, KEYICON_OFFSET_Y, 420.0f), 10.0f, 10.0f, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));		// �����̃A�C�R��
	SetKeyIcon(XMFLOAT3(-50.0f, KEYICON_OFFSET_Y, 243.0f), 10.0f, 10.0f, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));		// ���F�����̃A�C�R��


	// ���`��Ԃœ������Ă�
	if (GetMode() == MODE_GAME)
	{
		// �Ԃ����A�C�R��
		g_KeyIcon[0].move_time = 0.0f;															// ���`��ԗp�̃^�C�}�[���N���A
		g_KeyIcon[0].tbl_adr = move_tbl_game_red;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_KeyIcon[0].tbl_size = sizeof(move_tbl_game_red) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �����A�C�R��
		g_KeyIcon[1].move_time = 0.0f;															// ���`��ԗp�̃^�C�}�[���N���A
		g_KeyIcon[1].tbl_adr = move_tbl_game_blue;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_KeyIcon[1].tbl_size = sizeof(move_tbl_game_blue) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// ���F�����A�C�R��
		g_KeyIcon[2].move_time = 0.0f;															// ���`��ԗp�̃^�C�}�[���N���A
		g_KeyIcon[2].tbl_adr = move_tbl_game_yellow;											// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_KeyIcon[2].tbl_size = sizeof(move_tbl_game_yellow) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	}
	else if (GetMode() == MODE_TUTORIAL)
	{
		// �Ԃ����A�C�R��
		g_KeyIcon[0].move_time = 0.0f;															// ���`��ԗp�̃^�C�}�[���N���A
		g_KeyIcon[0].tbl_adr = move_tbl_tutorial_red;											// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_KeyIcon[0].tbl_size = sizeof(move_tbl_tutorial_red) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �����A�C�R��
		g_KeyIcon[1].move_time = 0.0f;															// ���`��ԗp�̃^�C�}�[���N���A
		g_KeyIcon[1].tbl_adr = move_tbl_tutorial_blue;											// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_KeyIcon[1].tbl_size = sizeof(move_tbl_tutorial_blue) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// ���F�����A�C�R��
		g_KeyIcon[2].move_time = 0.0f;															// ���`��ԗp�̃^�C�}�[���N���A
		g_KeyIcon[2].tbl_adr = move_tbl_tutorial_yellow;										// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_KeyIcon[2].tbl_size = sizeof(move_tbl_tutorial_yellow) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitKeyIcon(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateKeyIcon(void)
{
	PLAYER *player = GetPlayer();						// �v���C���[�̃|�C���^�[��������
	KEY *key = GetKey();								// ���̃|�C���^�[��������

	for (int i = 0; i < MAX_KEYICON; i++)
	{
		if (key[i].use == TRUE)
		{
			if (g_KeyIcon[i].tbl_adr != NULL)				// ���`��Ԃ����s����H
			{												// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_KeyIcon[i].move_time;
				float	time = g_KeyIcon[i].move_time - index;
				int		size = g_KeyIcon[i].tbl_size;

				float dt = 1.0f / g_KeyIcon[i].tbl_adr[index].frame;				// 1�t���[���Ői�߂鎞��
				g_KeyIcon[i].move_time += dt;										// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))												// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_KeyIcon[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_KeyIcon[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_KeyIcon[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_KeyIcon[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_KeyIcon[i].scl, s0 + scl * time);

			}

			// ���A�C�R���ƃv���C���[�̋���(�x�N�g��)�����߂�
			float dist;												// �����ۑ��p
			XMFLOAT3 PlayerPos = XMFLOAT3(player[0].pos.x, player[0].pos.y - 20, player[0].pos.z);
			XMVECTOR p = XMLoadFloat3(&PlayerPos);					// �v�Z�p�Ɉ�xXMVECTOR�ɓ����
			XMVECTOR e = XMLoadFloat3(&g_KeyIcon[i].pos);
			XMVECTOR d = p - e;										// �������v�Z
			dist = VectorMagnitude(&d);								// �G�l�~�[�ƃv���C���[�̃x�N�g���̒�����ۑ�

			g_KeyIcon[i].p_dist = dist;

			PrintDebugProc("�v���C���[����J�M�܂ł̋���:%f\n", g_KeyIcon[i].p_dist);

			if (GetMode() == MODE_GAME)
			{
				// ���A�C�R���̕\���E��\������
				if (g_KeyIcon[i].p_dist < 170)	// ���̋����ȉ��Ŋe�J�M��use��TURE(���擾)�Ȃ�
				{
					g_KeyIcon[i].bUse = TRUE;	// �\������
				}
				else
				{
					g_KeyIcon[i].bUse = FALSE;	// ��\��
				}
			}
		}

	}

#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE : TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawKeyIcon(void)
{
	KEY *key = GetKey();	// ���̃|�C���^�[��������

	// ���e�X�g�ݒ�
	if (g_bAlpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_KEYICON; i++)
	{
		if (key[i].use == TRUE && g_KeyIcon[i].bUse == TRUE) // �L�[�����擾�ŕ\���͈͓��ɓ�������
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_KeyIcon[i].scl.x, g_KeyIcon[i].scl.y, g_KeyIcon[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_KeyIcon[i].pos.x, g_KeyIcon[i].pos.y, g_KeyIcon[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_KeyIcon[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexKeyIcon(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 10.0f;
	float fHeight = 10.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// ���A�C�R���̃p�����[�^���Z�b�g
//=============================================================================
int SetKeyIcon(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxKeyIcon = -1;

	for (int i = 0; i < MAX_KEYICON; i++)
	{
		if (!g_KeyIcon[i].bUse)
		{
			g_KeyIcon[i].pos = pos;
			g_KeyIcon[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_KeyIcon[i].fWidth = fWidth;
			g_KeyIcon[i].fHeight = fHeight;
			g_KeyIcon[i].bUse = TRUE;
			g_KeyIcon[i].material.Diffuse = col;

			nIdxKeyIcon = i;

			break;
		}
	}

	return nIdxKeyIcon;
}

//=============================================================================
// ���A�C�R���̎擾
//=============================================================================
KEYICON *GetKeyIcon()
{
	return &g_KeyIcon[0];
}
