//=============================================================================
//
// �X�^�~�i�Q�[�W���� [stamina_bar.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "stamina_bar.h"
#include "sprite.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(300.0f)	// ��
#define TEXTURE_HEIGHT				(23.0f)		// ����
#define TEXTURE_MAX					(1)			// �e�N�X�`���̐�

#define STAMINA_BAR_MAX				(2)			// �X�^�~�i�o�[�̑���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/stamina.png",
};

static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL			g_Load = FALSE;		// ���������s�������̃t���O
static STAMINA_BAR	g_StaminaBar[STAMINA_BAR_MAX];


//=============================================================================
// ����������
//=============================================================================
HRESULT InitStaminaBar(void)
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


	// �X�^�~�i�o�[�̍\���̂̏�����
	for (int i = 0; i < STAMINA_BAR_MAX; i++)
	{
		g_StaminaBar[i].w = TEXTURE_WIDTH;
		g_StaminaBar[i].h = TEXTURE_HEIGHT;
		g_StaminaBar[i].pos = XMFLOAT3(230.0f, 60.0f, 0.0f);	// ���W
		g_StaminaBar[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_StaminaBar[i].texNo = 0;
	}
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitStaminaBar(void)
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
void UpdateStaminaBar(void)
{
	SetStaminaBar();

	// �v���C���[�p
	g_StaminaBar[0].pos.x = 230 - (TEXTURE_WIDTH / 2 - g_StaminaBar[0].w / 2);


#ifdef _DEBUG	// �f�o�b�O����\������

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawStaminaBar(void)
{
	PLAYER *player = GetPlayer();
	int *P_StaminaRecovery_count = GetStaminaRecovery_count();

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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	// �X�^�~�i�o�[�̍ő�l�`��@�v���C���[�p

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_StaminaBar[0].texNo]);

	//�X�^�~�i�o�[�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = 230.0f;						// �X�^�~�i�o�[�̕\���ʒuX
	float py = g_StaminaBar[0].pos.y;		// �X�^�~�i�o�[�̕\���ʒuY
	float pw = TEXTURE_WIDTH;				// �X�^�~�i�o�[�̕\����
	float ph = g_StaminaBar[0].h;			// �X�^�~�i�o�[�̕\������

	float tw = 1.0f;	// �e�N�X�`���̕�
	float th = 1.0f;	// �e�N�X�`���̍���
	float tx = 0.0f;	// �e�N�X�`���̍���X���W
	float ty = 0.0f;	// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteColorRotation(g_VertexBuffer,
		px, py, pw, ph,
		tx, ty, tw, th,
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		g_StaminaBar[0].rot.z);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);



	// �X�^�~�i�o�[�̕`�揈��

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_StaminaBar[0].texNo]);

	//�X�^�~�i�o�[�̈ʒu��e�N�X�`���[���W�𔽉f
	px = g_StaminaBar[0].pos.x;		// �X�^�~�i�o�[�̕\���ʒuX
	py = g_StaminaBar[0].pos.y;		// �X�^�~�i�o�[�̕\���ʒuY
	pw = g_StaminaBar[0].w;			// �X�^�~�i�o�[�̕\����
	ph = g_StaminaBar[0].h;			// �X�^�~�i�o�[�̕\������

	tw = 1.0f;	// �e�N�X�`���̕�
	th = 1.0f;	// �e�N�X�`���̍���
	tx = 0.0f;	// �e�N�X�`���̍���X���W
	ty = 0.0f;	// �e�N�X�`���̍���Y���W

	// �X�^�~�i�Q�[�W���g���؂�����񕜎��Ԓ��Ԃ����鏈��
	if (*P_StaminaRecovery_count == 0)
	{	// �ʏ펞(���F)
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 0.84f, 0.0f, 1.0f),
			g_StaminaBar[1].rot.z);
	}
	else
	{	// �g���؂����̂ŉ񕜎��Ԓ��͐Ԃ�����
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
			g_StaminaBar[1].rot.z);

	}


	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}


void SetStaminaBar(void)
{
	PLAYER *player = GetPlayer();
	g_StaminaBar[0].w = TEXTURE_WIDTH * player[0].staminaPercent;

}
