//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "effect.h"
#include "sound.h"
#include "sprite.h"
#include "enemy.h"
#include "player.h"
#include "debugproc.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�

#define EFFECT_START_POS			(120.0f)		// �G�t�F�N�g���o�n�߂鋗��(�G�l�~�[�Ɣ�r�����Ƃ��̋���)

#define	FADE_RATE					(0.02f)			// �t�F�[�h�W��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/effect_red.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static XMFLOAT4					g_Color;					// �G�t�F�N�g�̃J���[�i���l�j

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
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


	// �G�t�F�N�g�̏�����
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	g_Color = { 1.0f, 0.0f, 0.0f, 1.0f };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
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
void UpdateEffect(void)
{
	// �G�l�~�[�߂Â��ɂ�ĐԂ��G�t�F�N�g����ʂɏo������

	ENEMY *enemy = GetEnemy();
	PLAYER *player = GetPlayer();					// �v���C���[�̃|�C���^�[��������

	g_Color.w = 1.0f - (enemy[0].p_dist / EFFECT_START_POS);	// �������������Ȃ�قǃG�l�~�[�ƃv���C���[�̋������߂�

	// �v���C���[�����񂾂��ʂ�Ԃ����鏈��
	if (player[0].use == FALSE)
	{
		g_Color.w = 1.0f;
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("�G�t�F�N�g:%f\n", g_Color.w);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(void)
{
	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// �G�t�F�N�g�摜��`��

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//SetVertex(0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f);
	SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH / 2, TEXTURE_WIDTH / 2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
		g_Color);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

}
