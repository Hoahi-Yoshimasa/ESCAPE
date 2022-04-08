//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.cpp]
// Author : ������
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"
#include "player.h"
#include "enemy.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX		(2)				// �e�N�X�`���̐�

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11Buffer					*g_IndexBuffer = NULL;	// �C���f�b�N�X�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;				// �e�N�X�`���ԍ�

static XMFLOAT3		g_posField;								// �|���S���\���ʒu�̒��S���W
static XMFLOAT3		g_rotField;								// �|���S���̉�]�p

static int			g_nNumBlockXField, g_nNumBlockZField;	// �u���b�N��
static int			g_nNumVertexField;						// �����_��	
static int			g_nNumVertexIndexField;					// ���C���f�b�N�X��
static int			g_nNumPolygonField;						// ���|���S����
static float		g_fBlockSizeXField, g_fBlockSizeZField;	// �u���b�N�T�C�Y


static char* g_TextureName[] = {
	"data/TEXTURE/field_test.jpg",
	"data/TEXTURE/field.png",
};


// �g�̂悤�ɓ���������

static VERTEX_3D	*g_Vertex = NULL;

// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
static XMFLOAT3		g_Center;					// �g�̔����ꏊ
static float		g_Time = 0.0f;				// �g�̌o�ߎ���
static float		g_wave_frequency = 2.0f;	// �g�̎��g��
static float		g_wave_correction = 0.02f;	// �g�̋����␳
static float		g_wave_amplitude = 20.0f;	// �g�̐U��


static 	XMFLOAT3 g_Coord[MESHFIELD_BlockZ][MESHFIELD_BlockX]; // �S���b�V���t�B�[���h���̂Q�����z��

static BOOL			g_Load = FALSE;

// �Q�����z��̃��b�V���t�B�[���h�̉����ɂ��邩�𒲂ׂ�ϐ�
static int g_Enemy_chip_num;
static int g_Player_chip_num;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	MakeVertexPoint();

	// �|���S���\���ʒu�̒��S���W��ݒ�
	g_posField = pos;

	g_rotField = rot;

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

	// �u���b�N���̐ݒ�
	g_nNumBlockXField = nNumBlockX;
	g_nNumBlockZField = nNumBlockZ;


	// �e���b�V���t�B�[���h���S���W��ۑ����� ���S���W���~�����̂� �o�H�T���̈�
	for (int i = 0; i < g_nNumBlockZField; i++) // �c�̃u���b�N��
	{
		for (int j = 0; j < g_nNumBlockXField; j++) // ���̃u���b�N��
		{
			//2�����z��Ɋe���b�V���t�B�[���h��POS��������
			g_Coord[i][j] = XMFLOAT3(j * fBlockSizeX - g_nNumBlockXField * fBlockSizeX/2 + fBlockSizeX/2, 0, i * fBlockSizeZ - g_nNumBlockZField * fBlockSizeZ / 2 + fBlockSizeZ/2);
		}
	}



	// ���_���̐ݒ�
	g_nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// �C���f�b�N�X���̐ݒ�
	g_nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// �|���S�����̐ݒ�
	g_nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// �u���b�N�T�C�Y�̐ݒ�
	g_fBlockSizeXField = fBlockSizeX;
	g_fBlockSizeZField = fBlockSizeZ;


	// �Q�����z��̃��b�V���t�B�[���h�̉����ɂ��邩�𒲂ׂ�ϐ� ������
	int g_Enemy_chip_num = 0;  //�G�l�~�[�̍��W�|�C���g�𒲂ׂ�p
	int g_Player_chip_num = 0; //�v���C���[�̍��W�|�C���g�𒲂ׂ�p



	// ���_�����������ɍ���Ă����i�g�ׁ̈j
	// �g�̏���
	// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
	g_Vertex = new VERTEX_3D[g_nNumVertexField];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �g�̔����ꏊ
	g_Time = 0.0f;								// �g�̌o�ߎ���(�{�Ɓ[�Ƃœ����O���ɂȂ�)
	g_wave_frequency = 1.0f;					// �g�̎��g���i�㉺�^���̑����j
	g_wave_correction = 0.02f;					// �g�̋����␳�i�ς��Ȃ��Ă��ǂ��Ǝv���j
	g_wave_amplitude = 30.0f;					// �g�̐U��(�g�̍���)

	for (int z = 0; z < (g_nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (g_nNumBlockXField + 1); x++)
		{
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x = -(g_nNumBlockXField / 2.0f) * g_fBlockSizeXField + x * g_fBlockSizeXField;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField - z * g_fBlockSizeZField;

			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// �g��̒��S�_����̋����𓾂�
			float len = (float)sqrt(dx * dx + dz * dz);

			// �g�̍������Asin�֐��œ���
			// �g�̍����@= sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;

			// �@���̐ݒ�
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// ���ˌ��̐ݒ�
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			float texSizeX = 1.0f;
			float texSizeZ = 1.0f;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			g_Vertex[z * (g_nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}

	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �C���f�b�N�X�o�b�t�@����
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//���_�o�b�t�@�̒��g�𖄂߂�

		// ���_�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�

		// �C���f�b�N�X�o�b�t�@�̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZField; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1);
				nCntIdx++;
			}

			for (int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if (nCntVtxZ < (g_nNumBlockZField - 1))
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXField + 1) + g_nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMeshField(void)
{
	if (g_Load == FALSE) return;

	// �C���f�b�N�X�o�b�t�@�̉��
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// �e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMeshField(void)
{

	// return;	// �������X�L�b�v�I

	// �g�̏���
	float dt = 0.03f;

	for (int z = 0; z < g_nNumBlockZField; z++)
	{
		for (int x = 0; x < g_nNumBlockXField; x++)
		{
			float dx = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.x - g_Center.x;
			float dz = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.z - g_Center.z;

			// �g��̒��S�_����̋����𓾂�
			float len = (float)sqrt(dx * dx + dz * dz);

			// �g�̍������Asin�֐��œ���
			g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = 0.0f;
			//g_Vertex[z * (g_nNumBlockXField + 1) + x].Position.y = sinf(-g_Time * g_wave_frequency + len * g_wave_correction) * g_wave_amplitude;
		}

	}
	g_Time += dt;


	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// �S���_���𖈉�㏑�����Ă���̂�DX11�ł͂��̕�����������ł�
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D)*g_nNumVertexField);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);



	// �v���C���[�̈ʒu��񂩂烁�b�V���t�B�[���h�̉����ɂ��邩���v�Z
	PLAYER *player = GetPlayer();

	int player_Xchip_num = 0;
	int player_Zchip_num = 0;

	// ���b�V���t�B�[���hX���W�̔z�񉽔Ԗڂɂ��邩���v�Z
	player_Xchip_num = ((int)player[0].pos.x / (int)MESHFIELD_WIDTH);
	player_Xchip_num += MESHFIELD_BlockX / 2;

	if (player[0].pos.x >= 0.0f)
	{
		player_Xchip_num++;
	}
	player_Xchip_num -= 1; // 2�����z��擪�ɍ��킹�邽�߂�-1���Ă���

	PrintDebugProc("P���b�V����X�Ԗ�:%d\n", player_Xchip_num);

	// ���b�V���t�B�[���hZ���W�̔z�񉽔Ԗڂɂ��邩���v�Z
	player_Zchip_num = ((int)player[0].pos.z / (int)MESHFIELD_HEIGHT);
	player_Zchip_num += MESHFIELD_BlockZ / 2;

	if (player[0].pos.z >= 0.0f)
	{
		player_Zchip_num++;
	}
	player_Zchip_num -= 1; // 2�����z��擪�ɍ��킹�邽�߂�-1���Ă���

	PrintDebugProc("P���b�V����Z�Ԗ�:%d\n", player_Zchip_num);

	g_Player_chip_num = (player_Zchip_num * MESHFIELD_BlockZ) + player_Xchip_num; // Get�ňꎟ���z���Ԃ��p�ɗv�f�ԍ��Ƃ��č��킹�Ă���
	PrintDebugProc("P���b�V���̔Ԗ�:%d\n", g_Player_chip_num);



	// �G�l�~�[�̈ʒu��񂩂烁�b�V���t�B�[���h�̉����ɂ��邩���v�Z
	ENEMY *enemy = GetEnemy();

	int enemy_Xchip_num = 0;
	int enemy_Zchip_num = 0;

	// ���b�V���t�B�[���hX���W�̔z�񉽔Ԗڂɂ��邩���v�Z
	enemy_Xchip_num = ((int)enemy[0].pos.x / (int)MESHFIELD_WIDTH);
	enemy_Xchip_num += MESHFIELD_BlockX / 2;

	if (enemy[0].pos.x >= 0.0f)
	{
		enemy_Xchip_num++;
	}
	enemy_Xchip_num -= 1; // 2�����z��擪�ɍ��킹�邽�߂�-1���Ă���

	PrintDebugProc("E���b�V����X�Ԗ�:%d\n", enemy_Xchip_num);

	// ���b�V���t�B�[���hZ���W�̔z�񉽔Ԗڂɂ��邩���v�Z
	enemy_Zchip_num = ((int)enemy[0].pos.z / (int)MESHFIELD_HEIGHT);
	enemy_Zchip_num += MESHFIELD_BlockZ / 2;

	if (enemy[0].pos.z >= 0.0f)
	{
		enemy_Zchip_num++;
	}
	enemy_Zchip_num -= 1; // 2�����z��擪�ɍ��킹�邽�߂�-1���Ă���

	PrintDebugProc("E���b�V����Z�Ԗ�:%d\n", enemy_Zchip_num);

	g_Enemy_chip_num = (enemy_Zchip_num * MESHFIELD_BlockZ) + enemy_Xchip_num; // Get�ňꎟ���z���Ԃ��p�ɗv�f�ԍ��Ƃ��č��킹�Ă���
	PrintDebugProc("E���b�V���̔Ԗ�:%d\n", g_Enemy_chip_num);

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMeshField(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�ݒ�
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);


	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

#ifdef _DEBUG  // �f�o�b�O�p
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
#endif

	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);


	// �|���S���̕`��
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexField, 0, 0);
}

//                           �Ԃ������|���S���ꏊ �Ԃ������|���S���̌���
BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;
	XMFLOAT3 org = *HitPosition;

	// �����ォ��A�Y�h�[���Ɖ��փ��C���΂�
	start.y += 1000.0f;
	end.y -= 1000.0f;

	// ����������������ׂɑS�����ł͂Ȃ��āA���W����|���S��������o����
	float fz = (g_nNumBlockXField / 2.0f) * g_fBlockSizeXField;
	float fx = (g_nNumBlockZField / 2.0f) * g_fBlockSizeZField;
	int sz = (int)((-start.z + fz) / g_fBlockSizeZField);
	int sx = (int)((start.x + fx) / g_fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;

	if ((sz < 0) || (sz > g_nNumBlockZField - 1) ||
		(sx < 0) || (sx > g_nNumBlockXField - 1))
	{
		*Normal = { 0.0f, 1.0f, 0.0f };
		return FALSE;
	}


	// �K�v�����������J��Ԃ�
	for (int z = sz; z < ez; z++) // Z���̃}�X�@�����蔻��
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXField + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXField + 1) + (x + 1)].Position;

			// �O�p�|���S��������Q�����̓����蔻��
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}

	// �����ɂ��������Ă��Ȃ�������
	*HitPosition = org;
	return FALSE;
}

//=============================================================================
// �|�C���g�e�N�X�`���̒��_���̍쐬
//=============================================================================
HRESULT MakeVertexPoint(void)
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

	float fWidth = 10;
	float fHeight = 10;

	// ���_���W�̐ݒ�
	vertex[0].Position = { -fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[1].Position = { fWidth / 2.0f, 0.0f,  fHeight / 2.0f };
	vertex[2].Position = { -fWidth / 2.0f, 0.0f, -fHeight / 2.0f };
	vertex[3].Position = { fWidth / 2.0f, 0.0f, -fHeight / 2.0f };

	// �g�U���̐ݒ�
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}


//=============================================================================
// �e���b�V���t�B�[���h�̍��W �擪�A�h���X���擾
//=============================================================================
XMFLOAT3 *GetCoord()
{
	return &g_Coord[0][0];
}

//=============================================================================
// �G�l�~�[�����݃��b�V���t�B�[���h�̍��W���Ԗڂɂ��邩��v�f�ԍ��ŕԂ�
//=============================================================================
int *GetEnemy_chip_num()
{
	return &g_Enemy_chip_num;
}


//=============================================================================
// �v���C���[�����݃��b�V���t�B�[���h�̍��W���Ԗڂɂ��邩��v�f�ԍ��ŕԂ�
//=============================================================================
int *GetPlayer_chip_num()
{
	return &g_Player_chip_num;
}
