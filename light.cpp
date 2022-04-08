//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SHADOW_RANGE_WIDTH	(1000)			// �E�C���h�E�̕�
#define SHADOW_RANGE_HEIGHT	(1000)			// �E�C���h�E�̍���



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;


//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 100.0f, 100.0f, 100.0f );
		g_Light[i].Direction = XMFLOAT3( 1.0f, 1.0f, 1.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// ��������
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	g_Light[0].Position = XMFLOAT3(0.0f, 200.0f, 0.0f);
	g_Light[0].Direction = XMFLOAT3(0.0f, -1.0f, 0.3f);		// ���C�g�̊p�x  0.0 -1.0 0.3
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;				// ���s����
	g_Light[0].Enable = TRUE;								// ���̃��C�g��ON
	SetLight(0, &g_Light[0]);								// ����Őݒ肵�Ă���



	// �t�H�O�̏������i���̌��ʁj
	g_Fog.FogStart = 50.0f;									// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogEnd   = 210.0f;								// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	g_Fog.FogColor = XMFLOAT4( 0.1f, 0.1f, 0.1f, 1.0f );	// �t�H�O�̐F
	SetFog(&g_Fog);
	SetFogEnable(FALSE);		// ���̏ꏊ���`�F�b�N���� shadow

}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{
	//PLAYER *player = GetPlayer();

	//g_Light[0].Position.z = player[0].pos.z;
	//g_Light[0].Position.x = player[0].pos.x;

}

void SetLightViewProjection(void)	// ���C�g�̃|�W�V��������J�������ڂ��Ă�
{
	// ���C�g�r���[�s��ݒ�
	XMMATRIX mtxLightView;
	XMVECTOR pos = XMLoadFloat3(&g_Light[0].Position);
	XMVECTOR dir = XMLoadFloat3(&g_Light[0].Direction);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR vec = XMLoadFloat3(&up);
	mtxLightView = XMMatrixLookToLH(pos, dir, vec);
	SetLightViewMatrix(&mtxLightView);

	//���C�g���ˉe�s��ݒ�
	XMMATRIX mtxLightProjection;
	mtxLightProjection = XMMatrixOrthographicLH(SHADOW_RANGE_WIDTH, SHADOW_RANGE_HEIGHT, VIEW_NEAR_Z, VIEW_FAR_Z); // �e�͈̔�
	SetLightProjectionMatrix(&mtxLightProjection);
}

//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}



