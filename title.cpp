//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�


#define TEXTURE_WIDTH_TITLE_LOGO			(850)		// �^�C�g�����S�T�C�Y ��
#define TEXTURE_HEIGHT_TITLE_LOGO			(180)		// ����

#define TEXTURE_WIDTH_PRESS_LOGO			(480)		// PRESS ANY BUTTON���S�T�C�Y
#define TEXTURE_HEIGHT_PRESS_LOGO			(80)		// ����

#define TEXTURE_WIDTH_MODE_CHOICE_LOGO		(400)		// ���[�h�I�����S�T�C�Y
#define TEXTURE_HEIGHT_MODE_CHOICE_LOGO		(60)		// ����

#define TEXTURE_WIDTH_CURSOR_LOGO			(150)		// �J�[�\�����S�T�C�Y ��
#define TEXTURE_HEIGHT_CURSOR_LOGO			(50)		// ����

#define COORDINATE_HEIGHT_GAME_LOGO			(380)		// �Q�[���X�^�[�g���S�̍��W����
#define COORDINATE_HEIGHT_TUTORIAL_LOGO		(460)		// �`���[�g���A�����S�̍��W����
#define COORDINATE_HEIGHT_TITLE_LOGO		(170)		// �^�C�g���e�N�X�`���̍��W����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/Title/Title1.png",		// �^�C�g���摜
	"data/TEXTURE/Title/Title2.png",		// �Q�[���X�^�[�g�摜
	"data/TEXTURE/Title/Title3.png",		// �`���[�g���A���摜
	"data/TEXTURE/Title/Skeleton_hand.png",	// �J�[�\���摜
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static XMFLOAT4					g_CursorColourRed;			// �J�[�\���p�ԐF�̐ݒ�
static XMFLOAT4					g_CursorColourWhite;		// �J�[�\���p���F�̐ݒ�

static XMFLOAT4					CursorColourGame;			// �Q�[�����S�̐F�ς��p�ϐ�
static XMFLOAT4					CursorColourTutorial;		// �`���[�g���A�����S�̐F�ς��p�ϐ�

static float					g_AlphaValue;				// �^�C�g�����S�̃��l�p�ϐ�
static int						g_Count;					// �J�E���g�ϐ�

static BOOL						g_Load = FALSE;

static int						g_Cursor;					// �I���J�[�\���p�ϐ�
//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;
	g_Cursor = 0;										// �I���J�[�\���p�ϐ�
	g_AlphaValue = 0.0f;								// �ŏ��̓^�C�g����ʂ𓧖��ɂ������̂�0�ŏ�����
	g_Count = 540;										// 9�b��Ƀ^�C�g�����S���o�������̂�540�t���[���ŏ�����(���̃Q�[����60FPS)

	g_CursorColourRed = { 1.0f, 0.0f, 0.0f, 1.0f };		// �J�[�\���p�ԐF�̐ݒ�
	g_CursorColourWhite = { 1.0f, 1.0f, 1.0f, 1.0f };	// �J�[�\���p���F�̐ݒ�
	CursorColourGame = g_CursorColourWhite;				// �Q�[�����S�̐F�ς��p�ϐ� (���F�ŏ�����)
	CursorColourTutorial = g_CursorColourRed;			// �`���[�g���A�����S�̐F�ς��p�ϐ� (�ԐF�ŏ�����)

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_OP);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTitle(void)
{
	PrintDebugProc("�J�[�\��:%d %n", g_Cursor);

	// �L�[�{�[�h����
	if (GetKeyboardTrigger(DIK_UP))
	{
		g_Cursor++;
	}
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		g_Cursor++;
	}

	// �R���g���[���[����
	if (IsButtonTriggered(0, BUTTON_UP))
	{
		g_Cursor++;
	}
	if (IsButtonTriggered(0, BUTTON_DOWN))
	{
		g_Cursor++;
	}

	g_Cursor = g_Cursor % 2;

	if (g_Cursor == 0)
	{
		CursorColourGame = g_CursorColourWhite;				// �Q�[�����S�̐F�ς��p�ϐ� (���F�ŏ�����)
		CursorColourTutorial = g_CursorColourRed;			// �`���[�g���A�����S�̐F�ς��p�ϐ� (�ԐF�ŏ�����)
	}
	else
	{
		CursorColourGame = g_CursorColourRed;				// �Q�[�����S�̐F�ς��p�ϐ� (���F�ŏ�����)
		CursorColourTutorial = g_CursorColourWhite;			// �`���[�g���A�����S�̐F�ς��p�ϐ� (�ԐF�ŏ�����)
	}

	// �V�[���J�ڂ��鏈��
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_C) || IsButtonTriggered(0, BUTTON_M))
	{
		switch (g_Cursor)
		{
		case 0:
			SetFade(FADE_OUT, MODE_GAME);
			break;

		case 1:
			SetFade(FADE_OUT, MODE_TUTORIAL);
			break;

		default:
			break;
		}
	}

	// �^�C���J�E���g�����炷����
	if (g_Count <= 0)				// �J�E���g��0�ɂȂ�����(�P�b�o�߂�����)
	{
		if (g_AlphaValue < 1.0f)
		{
			g_AlphaValue += 0.01f;	// �^�C�g�����S�̃��l�𑝂₷ 5�t���[����0.01��������
		}
		else
		{
			g_AlphaValue = 1.0f;	// ���l�Œ肳����
		}
		g_Count = 5;				// ��x�J�E���g��0�ɂȂ�����5��������
	}
	g_Count--;						// ���t���[���J�E���g��1���炷


	PrintDebugProc("�^�C�g���J�E���g:%d\n", g_Count);
	PrintDebugProc("�^�C�g���A���t�@�l:%f\n", g_AlphaValue);
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_AlphaValue);	// �^�C�g�����S�����Ԃɂ���ďo��������
	SetMaterial(material);

	// �^�C�g���̃��S��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_TITLE_LOGO, TEXTURE_WIDTH_TITLE_LOGO, TEXTURE_HEIGHT_TITLE_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �}�e���A���ݒ�
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �Q�[���X�^�[�g�̃��S�`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_GAME_LOGO, TEXTURE_WIDTH_MODE_CHOICE_LOGO, TEXTURE_HEIGHT_MODE_CHOICE_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
			CursorColourGame);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �`���[�g���A���̃��S�`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x, COORDINATE_HEIGHT_TUTORIAL_LOGO, TEXTURE_WIDTH_MODE_CHOICE_LOGO, TEXTURE_HEIGHT_MODE_CHOICE_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
			CursorColourTutorial);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �J�[�\���̃��S�`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Pos.x / 2.5f + 50 * g_Cursor, 380.0f + 90.0f *g_Cursor, TEXTURE_WIDTH_CURSOR_LOGO, TEXTURE_HEIGHT_CURSOR_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
			g_CursorColourWhite);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}





