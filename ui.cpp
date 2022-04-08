//=============================================================================
//
// UI���� [ui.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "sprite.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(16)		// �T�C�Y
#define TEXTURE_HEIGHT				(32)		// 

#define DASH_TEXTURE_WIDTH			(60.0f)		// �_�b�V���A�C�R���̕\����
#define DASH_TEXTURE_HEIGHT			(40.0f)		// ����

#define KEY_UI_TEXTURE_WIDTH		(100.0f)	// �A�C�e���擾���\����
#define KEY_UI_TEXTURE_HEIGHT		(250.0f)	// ����

#define KEY_TEXTURE_WIDTH			(70.0f)		// �L�[�A�C�e���\����
#define KEY_TEXTURE_HEIGHT			(70.0f)		// ����

#define TEXT_FIELD_TEXTURE_WIDTH	(600.0f)	// �e�L�X�g�\�����̕\����
#define TEXT_FIELD_TEXTURE_HEIGHT	(150.0f)	// ����

#define TEXT_TEXTURE_WIDTH			(400.0f)	// �e�L�X�g�̕\����
#define TEXT_TEXTURE_HEIGHT			(50.0f)		// ����

#define TEXTURE_MAX					(19)		// ���e�N�X�`����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/UI/DASH.png",
	"data/TEXTURE/UI/UI.png",
	"data/TEXTURE/UI/key.png",
	"data/TEXTURE/Tutorial/Tutorial_Text1.png",
	"data/TEXTURE/Tutorial/Tutorial_Text2.png",
	"data/TEXTURE/Tutorial/Tutorial_Text3.png",
	"data/TEXTURE/Tutorial/Tutorial_Text4.png",
	"data/TEXTURE/Tutorial/Tutorial_Text5.png",
	"data/TEXTURE/Tutorial/Tutorial_Text6.png",
	"data/TEXTURE/Tutorial/Tutorial_Text7.png",
	"data/TEXTURE/Tutorial/Tutorial_Text8.png",
	"data/TEXTURE/Tutorial/Tutorial_Text9.png",
	"data/TEXTURE/Tutorial/Tutorial_Text10.png",
	"data/TEXTURE/Tutorial/Tutorial_Text11.png",
	"data/TEXTURE/Tutorial/Tutorial_Text12.png",
	"data/TEXTURE/Tutorial/Tutorial_Text13.png",
	"data/TEXTURE/Tutorial/Tutorial_Text14.png",
	"data/TEXTURE/Tutorial/Tutorial_Text15.png",
	"data/TEXTURE/Tutorial/Tutorial_Text16.png",
};


//static BOOL					g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
//static float					g_w, g_h;					// ���ƍ���
//static XMFLOAT3				g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static UI						g_Ui[MAX_UI];				// UI�̐�

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitUi(void)
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

	g_TexNo = 0;

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// UI�̏�����
	for (int i = 0; i < MAX_UI; i++)
	{
		g_Ui[i].pos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };				// �ʒu
		g_Ui[i].scl = XMFLOAT3{ 0.0f, 0.0f, 0.0f };				// �X�P�[��
		g_Ui[i].fWidth = TEXTURE_WIDTH;							// ��
		g_Ui[i].fHeight = TEXTURE_HEIGHT;;						// ����
		g_Ui[i].collected = 0.2f;								// �A�C�e��(��)�̎擾�� ����������0.2f(�������ŏ�����)
		g_Ui[i].use = TRUE;										// �g�p���Ă��邩�ǂ���
	}

	// DASH�Q�[�W�̏���������
	g_Ui[0].pos = XMFLOAT3{ 40.0f, 60.0f, 0.0f };				// �ʒu
	g_Ui[0].fWidth = DASH_TEXTURE_WIDTH;						// ��
	g_Ui[0].fHeight = DASH_TEXTURE_HEIGHT;						// ����

	//���̎擾�Q�[�W�̏���������
	g_Ui[1].pos = XMFLOAT3{ 910.0f, 280.0f, 0.0f };				// �ʒu
	g_Ui[1].fWidth = KEY_UI_TEXTURE_WIDTH;						// ��
	g_Ui[1].fHeight = KEY_UI_TEXTURE_HEIGHT;					// ����

	//�Ԃ����摜�̏���������
	g_Ui[2].pos = XMFLOAT3{ 910.0f, 200.0f, 0.0f };				// �ʒu
	g_Ui[2].fWidth = KEY_TEXTURE_WIDTH;							// ��
	g_Ui[2].fHeight = KEY_TEXTURE_HEIGHT;						// ����

	//�����摜�̏���������
	g_Ui[3].pos = XMFLOAT3{ 910.0f, 280.0f, 0.0f };				// �ʒu
	g_Ui[3].fWidth = KEY_TEXTURE_WIDTH;							// ��
	g_Ui[3].fHeight = KEY_TEXTURE_HEIGHT;						// ����

	//���F�̌��摜�̏���������
	g_Ui[4].pos = XMFLOAT3{ 910.0f, 360.0f, 0.0f };				// �ʒu
	g_Ui[4].fWidth = KEY_TEXTURE_WIDTH;							// ��
	g_Ui[4].fHeight = KEY_TEXTURE_HEIGHT;						// ����

	//�e�L�X�g���̏���������
	g_Ui[5].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[5].fWidth = TEXT_FIELD_TEXTURE_WIDTH;					// ��
	g_Ui[5].fHeight = TEXT_FIELD_TEXTURE_HEIGHT;				// ����

	//�e�L�X�g1�̏���������
	g_Ui[6].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[6].fWidth = TEXT_TEXTURE_WIDTH;						// ��
	g_Ui[6].fHeight = TEXT_TEXTURE_HEIGHT;						// ����
	g_Ui[6].use = TRUE;											// �g�p���Ă��邩�ǂ���

	//�e�L�X�g2�̏���������
	g_Ui[7].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[7].fWidth = TEXT_TEXTURE_WIDTH;						// ��
	g_Ui[7].fHeight = TEXT_TEXTURE_HEIGHT;						// ����
	g_Ui[7].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g3�̏���������
	g_Ui[8].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[8].fWidth = TEXT_TEXTURE_WIDTH;						// ��
	g_Ui[8].fHeight = TEXT_TEXTURE_HEIGHT * 2;					// ����
	g_Ui[8].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g4�̏���������
	g_Ui[9].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[9].fWidth = TEXT_TEXTURE_WIDTH;						// ��
	g_Ui[9].fHeight = TEXT_TEXTURE_HEIGHT;						// ����
	g_Ui[9].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g5�̏���������
	g_Ui[10].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[10].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// ��
	g_Ui[10].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// ����
	g_Ui[10].use = FALSE;										// �g�p���Ă��邩�ǂ���


	//�e�L�X�g6�̏���������
	g_Ui[11].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[11].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// ��
	g_Ui[11].fHeight = TEXT_TEXTURE_HEIGHT * 2.2f;				// ����
	g_Ui[11].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g7�̏���������
	g_Ui[12].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[12].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// ��
	g_Ui[12].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// ����
	g_Ui[12].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g8�̏���������
	g_Ui[13].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[13].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// ��
	g_Ui[13].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// ����
	g_Ui[13].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g9�̏���������
	g_Ui[14].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[14].fWidth = TEXT_TEXTURE_WIDTH * 1.2f;				// ��
	g_Ui[14].fHeight = TEXT_TEXTURE_HEIGHT * 1.2f;				// ����
	g_Ui[14].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g10�̏���������
	g_Ui[15].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[15].fWidth = TEXT_TEXTURE_WIDTH * 1.4f;				// ��
	g_Ui[15].fHeight = TEXT_TEXTURE_HEIGHT * 1.7f;				// ����
	g_Ui[15].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g11�̏���������
	g_Ui[16].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[16].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// ��
	g_Ui[16].fHeight = TEXT_TEXTURE_HEIGHT * 1.7f;				// ����
	g_Ui[16].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g12�̏���������
	g_Ui[17].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[17].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// ��
	g_Ui[17].fHeight = TEXT_TEXTURE_HEIGHT * 1.7f;				// ����
	g_Ui[17].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g13�̏���������
	g_Ui[18].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[18].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// ��
	g_Ui[18].fHeight = TEXT_TEXTURE_HEIGHT * 3;					// ����
	g_Ui[18].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g14�̏���������
	g_Ui[19].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[19].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// ��
	g_Ui[19].fHeight = TEXT_TEXTURE_HEIGHT * 3;					// ����
	g_Ui[19].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g15�̏���������
	g_Ui[20].pos = XMFLOAT3{ SCREEN_WIDTH / 2, 450.0f, 0.0f };	// �ʒu
	g_Ui[20].fWidth = TEXT_TEXTURE_WIDTH * 1.5f;				// ��
	g_Ui[20].fHeight = TEXT_TEXTURE_HEIGHT * 2.0f;				// ����
	g_Ui[20].use = FALSE;										// �g�p���Ă��邩�ǂ���

	//�e�L�X�g16�̏���������
	g_Ui[21].pos = XMFLOAT3{ SCREEN_WIDTH / 1.5f, 510.0f, 0.0f };	// �ʒu
	g_Ui[21].fWidth = TEXT_TEXTURE_WIDTH * 0.7f;				// ��
	g_Ui[21].fHeight = TEXT_TEXTURE_HEIGHT * 0.7f;				// ����
	g_Ui[21].use = TRUE;										// �g�p���Ă��邩�ǂ���

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUi(void)
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
void UpdateUi(void)
{
	// �Ō�̃e�L�X�g���\�����ꂽ�� Next BUTTON �̕\������������
	if (g_Ui[19].use == TRUE)
	{
		g_Ui[20].use = FALSE;
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUi(void)
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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// �X�^�~�i�Q�[�W�̃A�C�R���\��

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// �ʒu��e�N�X�`���[���W�𔽉f
	float px = g_Ui[0].pos.x;			// �\���ʒuX
	float py = g_Ui[0].pos.y;			// �\���ʒuY
	float pw = g_Ui[0].fWidth;			// �\����
	float ph = g_Ui[0].fHeight;			// �\������

	float tw = 1.0f;					// �e�N�X�`���̕�
	float th = 1.0f;					// �e�N�X�`���̍���
	float tx = 0.0f;					// �e�N�X�`���̍���X���W
	float ty = 0.0f;					// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
		XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	// KEY�̎擾���̕\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	// �ʒu��e�N�X�`���[���W�𔽉f
	float px1 = g_Ui[1].pos.x;			// �\���ʒuX
	float py1 = g_Ui[1].pos.y;			// �\���ʒuY
	float pw1 = g_Ui[1].fWidth;			// �\����
	float ph1 = g_Ui[1].fHeight;		// �\������

	float tw1 = 1.0f;					// �e�N�X�`���̕�
	float th1 = 1.0f;					// �e�N�X�`���̍���
	float tx1 = 0.0f;					// �e�N�X�`���̍���X���W
	float ty1 = 0.0f;					// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
	SetSpriteColor(g_VertexBuffer, px1, py1, pw1, ph1, tx1, ty1, tw1, th1,
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);


	// �Ԃ����̕\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// �ʒu��e�N�X�`���[���W�𔽉f
	float px2 = g_Ui[2].pos.x;			// �\���ʒuX
	float py2 = g_Ui[2].pos.y;			// �\���ʒuY
	float pw2 = g_Ui[2].fWidth;			// �\����
	float ph2 = g_Ui[2].fHeight;		// �\������

	float tw2 = 1.0f;					// �e�N�X�`���̕�
	float th2 = 1.0f;					// �e�N�X�`���̍���
	float tx2 = 0.0f;					// �e�N�X�`���̍���X���W
	float ty2 = 0.0f;					// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
	SetSpriteColor(g_VertexBuffer, px2, py2, pw2, ph2, tx2, ty2, tw2, th2,
		XMFLOAT4(1.0f, 0.0f, 0.0f, g_Ui[2].collected));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// �����̕\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// �ʒu��e�N�X�`���[���W�𔽉f
	float px3 = g_Ui[3].pos.x;			// �\���ʒuX
	float py3 = g_Ui[3].pos.y;			// �\���ʒuY
	float pw3 = g_Ui[3].fWidth;			// �\����
	float ph3 = g_Ui[3].fHeight;		// �\������

	float tw3 = 1.0f;					// �e�N�X�`���̕�
	float th3 = 1.0f;					// �e�N�X�`���̍���
	float tx3 = 0.0f;					// �e�N�X�`���̍���X���W
	float ty3 = 0.0f;					// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
	SetSpriteColor(g_VertexBuffer, px3, py3, pw3, ph3, tx3, ty3, tw3, th3,
		XMFLOAT4(0.0f, 0.0f, 1.0f, g_Ui[3].collected));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	// ���F�����̕\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

	// �ʒu��e�N�X�`���[���W�𔽉f
	float px4 = g_Ui[4].pos.x;			// �\���ʒuX
	float py4 = g_Ui[4].pos.y;			// �\���ʒuY
	float pw4 = g_Ui[4].fWidth;			// �\����
	float ph4 = g_Ui[4].fHeight;		// �\������

	float tw4 = 1.0f;					// �e�N�X�`���̕�
	float th4 = 1.0f;					// �e�N�X�`���̍���
	float tx4 = 0.0f;					// �e�N�X�`���̍���X���W
	float ty4 = 0.0f;					// �e�N�X�`���̍���Y���W

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
	SetSpriteColor(g_VertexBuffer, px4, py4, pw4, ph4, tx4, ty4, tw4, th4,
		XMFLOAT4(1.0f, 1.0f, 0.0f, g_Ui[4].collected));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);

	if (GetMode() == MODE_TUTORIAL)
	{
		// �e�L�X�g���̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px5 = g_Ui[5].pos.x;			// �\���ʒuX
		float py5 = g_Ui[5].pos.y;			// �\���ʒuY
		float pw5 = g_Ui[5].fWidth;			// �\����
		float ph5 = g_Ui[5].fHeight;		// �\������

		float tw5 = 1.0f;					// �e�N�X�`���̕�
		float th5 = 1.0f;					// �e�N�X�`���̍���
		float tx5 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty5 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px5, py5, pw5, ph5, tx5, ty5, tw5, th5,
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�L�X�g1�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px6 = g_Ui[6].pos.x;			// �\���ʒuX
		float py6 = g_Ui[6].pos.y;			// �\���ʒuY
		float pw6 = g_Ui[6].fWidth;			// �\����
		float ph6 = g_Ui[6].fHeight;		// �\������

		float tw6 = 1.0f;					// �e�N�X�`���̕�
		float th6 = 1.0f;					// �e�N�X�`���̍���
		float tx6 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty6 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px6, py6, pw6, ph6, tx6, ty6, tw6, th6,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[6].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g2�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px7 = g_Ui[7].pos.x;			// �\���ʒuX
		float py7 = g_Ui[7].pos.y;			// �\���ʒuY
		float pw7 = g_Ui[7].fWidth;			// �\����
		float ph7 = g_Ui[7].fHeight;		// �\������

		float tw7 = 1.0f;					// �e�N�X�`���̕�
		float th7 = 1.0f;					// �e�N�X�`���̍���
		float tx7 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty7 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px7, py7, pw7, ph7, tx7, ty7, tw7, th7,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[7].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g3�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px8 = g_Ui[8].pos.x;			// �\���ʒuX
		float py8 = g_Ui[8].pos.y;			// �\���ʒuY
		float pw8 = g_Ui[8].fWidth;			// �\����
		float ph8 = g_Ui[8].fHeight;		// �\������

		float tw8 = 1.0f;					// �e�N�X�`���̕�
		float th8 = 1.0f;					// �e�N�X�`���̍���
		float tx8 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty8 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px8, py8, pw8, ph8, tx8, ty8, tw8, th8,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[8].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g4�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px9 = g_Ui[9].pos.x;			// �\���ʒuX
		float py9 = g_Ui[9].pos.y;			// �\���ʒuY
		float pw9 = g_Ui[9].fWidth;			// �\����
		float ph9 = g_Ui[9].fHeight;		// �\������

		float tw9 = 1.0f;					// �e�N�X�`���̕�
		float th9 = 1.0f;					// �e�N�X�`���̍���
		float tx9 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty9 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px9, py9, pw9, ph9, tx9, ty9, tw9, th9,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[9].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�L�X�g5�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px10 = g_Ui[10].pos.x;			// �\���ʒuX
		float py10 = g_Ui[10].pos.y;			// �\���ʒuY
		float pw10 = g_Ui[10].fWidth;			// �\����
		float ph10 = g_Ui[10].fHeight;			// �\������

		float tw10 = 1.0f;					// �e�N�X�`���̕�
		float th10 = 1.0f;					// �e�N�X�`���̍���
		float tx10 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty10 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px10, py10, pw10, ph10, tx10, ty10, tw10, th10,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[10].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g6�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px11 = g_Ui[11].pos.x;			// �\���ʒuX
		float py11 = g_Ui[11].pos.y;			// �\���ʒuY
		float pw11 = g_Ui[11].fWidth;			// �\����
		float ph11 = g_Ui[11].fHeight;			// �\������

		float tw11 = 1.0f;					// �e�N�X�`���̕�
		float th11 = 1.0f;					// �e�N�X�`���̍���
		float tx11 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty11 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px11, py11, pw11, ph11, tx11, ty11, tw11, th11,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[11].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�L�X�g7�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px12 = g_Ui[12].pos.x;			// �\���ʒuX
		float py12 = g_Ui[12].pos.y;			// �\���ʒuY
		float pw12 = g_Ui[12].fWidth;			// �\����
		float ph12 = g_Ui[12].fHeight;			// �\������

		float tw12 = 1.0f;					// �e�N�X�`���̕�
		float th12 = 1.0f;					// �e�N�X�`���̍���
		float tx12 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty12 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px12, py12, pw12, ph12, tx12, ty12, tw12, th12,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[12].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g8�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px13 = g_Ui[13].pos.x;			// �\���ʒuX
		float py13 = g_Ui[13].pos.y;			// �\���ʒuY
		float pw13 = g_Ui[13].fWidth;			// �\����
		float ph13 = g_Ui[13].fHeight;			// �\������

		float tw13 = 1.0f;					// �e�N�X�`���̕�
		float th13 = 1.0f;					// �e�N�X�`���̍���
		float tx13 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty13 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px13, py13, pw13, ph13, tx13, ty13, tw13, th13,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[13].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�L�X�g9�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px14 = g_Ui[14].pos.x;			// �\���ʒuX
		float py14 = g_Ui[14].pos.y;			// �\���ʒuY
		float pw14 = g_Ui[14].fWidth;			// �\����
		float ph14 = g_Ui[14].fHeight;			// �\������

		float tw14 = 1.0f;					// �e�N�X�`���̕�
		float th14 = 1.0f;					// �e�N�X�`���̍���
		float tx14 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty14 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px14, py14, pw14, ph14, tx14, ty14, tw14, th14,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[14].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g10�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[12]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px15 = g_Ui[15].pos.x;			// �\���ʒuX
		float py15 = g_Ui[15].pos.y;			// �\���ʒuY
		float pw15 = g_Ui[15].fWidth;			// �\����
		float ph15 = g_Ui[15].fHeight;			// �\������

		float tw15 = 1.0f;					// �e�N�X�`���̕�
		float th15 = 1.0f;					// �e�N�X�`���̍���
		float tx15 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty15 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px15, py15, pw15, ph15, tx15, ty15, tw15, th15,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[15].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�L�X�g11�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px16 = g_Ui[16].pos.x;			// �\���ʒuX
		float py16 = g_Ui[16].pos.y;			// �\���ʒuY
		float pw16 = g_Ui[16].fWidth;			// �\����
		float ph16 = g_Ui[16].fHeight;			// �\������

		float tw16 = 1.0f;					// �e�N�X�`���̕�
		float th16 = 1.0f;					// �e�N�X�`���̍���
		float tx16 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty16 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px16, py16, pw16, ph16, tx16, ty16, tw16, th16,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[16].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g12�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[14]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px17 = g_Ui[17].pos.x;			// �\���ʒuX
		float py17 = g_Ui[17].pos.y;			// �\���ʒuY
		float pw17 = g_Ui[17].fWidth;			// �\����
		float ph17 = g_Ui[17].fHeight;			// �\������

		float tw17 = 1.0f;					// �e�N�X�`���̕�
		float th17 = 1.0f;					// �e�N�X�`���̍���
		float tx17 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty17 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px17, py17, pw17, ph17, tx17, ty17, tw17, th17,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[17].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�L�X�g13�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px18 = g_Ui[18].pos.x;			// �\���ʒuX
		float py18 = g_Ui[18].pos.y;			// �\���ʒuY
		float pw18 = g_Ui[18].fWidth;			// �\����
		float ph18 = g_Ui[18].fHeight;			// �\������

		float tw18 = 1.0f;					// �e�N�X�`���̕�
		float th18 = 1.0f;					// �e�N�X�`���̍���
		float tx18 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty18 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px18, py18, pw18, ph18, tx18, ty18, tw18, th18,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[18].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�L�X�g14�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[16]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px19 = g_Ui[19].pos.x;			// �\���ʒuX
		float py19 = g_Ui[19].pos.y;			// �\���ʒuY
		float pw19 = g_Ui[19].fWidth;			// �\����
		float ph19 = g_Ui[19].fHeight;			// �\������

		float tw19 = 1.0f;					// �e�N�X�`���̕�
		float th19 = 1.0f;					// �e�N�X�`���̍���
		float tx19 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty19 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px19, py19, pw19, ph19, tx19, ty19, tw19, th19,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[19].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


		// �e�L�X�g15�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[17]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px20 = g_Ui[20].pos.x;			// �\���ʒuX
		float py20 = g_Ui[20].pos.y;			// �\���ʒuY
		float pw20 = g_Ui[20].fWidth;			// �\����
		float ph20 = g_Ui[20].fHeight;			// �\������

		float tw20 = 1.0f;					// �e�N�X�`���̕�
		float th20 = 1.0f;					// �e�N�X�`���̍���
		float tx20 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty20 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px20, py20, pw20, ph20, tx20, ty20, tw20, th20,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[20].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// Next�摜�̕\��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[18]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px21 = g_Ui[21].pos.x;			// �\���ʒuX
		float py21 = g_Ui[21].pos.y;			// �\���ʒuY
		float pw21 = g_Ui[21].fWidth;			// �\����
		float ph21 = g_Ui[21].fHeight;			// �\������

		float tw21 = 1.0f;					// �e�N�X�`���̕�
		float th21 = 1.0f;					// �e�N�X�`���̍���
		float tx21 = 0.0f;					// �e�N�X�`���̍���X���W
		float ty21 = 0.0f;					// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W�ƐF�̐ݒ�
		SetSpriteColor(g_VertexBuffer, px21, py21, pw21, ph21, tx21, ty21, tw21, th21,
			XMFLOAT4(1.0f, 1.0f, 1.0f, (float)(g_Ui[21].use)));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);


	}

}


//=============================================================================
// UI�����擾
//=============================================================================

UI *GetUi(void)
{
	return g_Ui;
}

