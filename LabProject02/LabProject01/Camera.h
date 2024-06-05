#pragma once
#include "stdafx.h"


#define FIRST_PERSON_CAMERA 0x01
#define SPACESHIP_CAMERA	 0x02
#define THIRD_PERSON_CAMERA  0x03

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH)/float(FRAME_BUFFER_HEIGHT))

class CCharacter;

struct VS_CB_CAMERA_INFO {

	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3		m_xmf3Position;
};

class CCamera
{

protected:
	BoundingFrustum m_xmFrustum;
	//12�� �߰�
	//ī�޶��� ��ġ �����̴�.
	XMFLOAT3		m_xmf3Position;
	//ī�޶��� ���� x��  y �� z  ���� ��Ÿ���� �����̴�.
	XMFLOAT3		m_xmf3Right;
	XMFLOAT3		m_xmf3Up;
	XMFLOAT3		m_xmf3Look;

	//ī�޶� x y z������ �󸶸�ŭ ȸ���ߴ°��� ��Ÿ���� �����̴�.
	float			m_fPitch;
	float			m_fRoll;
	float			m_fYaw;
	//ī�޶��� ������ ��Ÿ����.
	DWORD			m_nMode;

	//�÷��̾ �ٶ� ��ġ�����̴�. �ַ� 3��Ī ī�޶󿡼� ���ȴ�.
	XMFLOAT3		m_xmf3LookAtWorld;
	//�÷��̾�� ī�޶��� �������� ��Ÿ���� �����̴�.
	XMFLOAT3		m_xmf3offset;
	//�÷��̾ ȸ���Ҷ� ��ŭ���� �ð��� ������Ų�� ī�޶� ȸ����ų ���ΰ��� ��Ÿ����.
	float			m_fTimeLag;

	XMFLOAT4X4		 m_xmf4x4View;
	XMFLOAT4X4		m_xmf4x4Projection;



	//ī�޶� ������ �ִ� �÷��̾ ���� �������̴�.

	CCharacter* m_pPlayer = NULL;


	//����Ʈ�� ���� �簢��
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	ID3D12Resource* m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO* m_pcbMappedCamera = NULL;

public:
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera() {};

	//ī�޶��� ������ ���̴� ���α׷����� �����ϱ� ���� ������۸� �����ϰ� �����Ѵ�.
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);


	//ī�޶� ��ȯ ����� �����Ѵ�.
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Lookat, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	//���� ��ȯ ����� �����Ѵ�
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFovAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinz = 0.0f, float fMax = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	void SetPlayer(CCharacter* pPlayer) { m_pPlayer = pPlayer; }
	CCharacter* GetPlayer() { return (m_pPlayer); }

	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return (m_xmf3Position); }

	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }
	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return (m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return (m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3offset = xmf3Offset; }
	XMFLOAT3& GetOffset() { return(m_xmf3offset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }


	// ī�޶� xmf3Shift ��ŭ �̵��Ѵ�.
	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x; m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }

	//ī�޶� xyz������ ȸ���ϴ� �����Լ�
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) {}
	//ī�޶��� �̵�, ȸ���� ���� ī�޶��� ������ �����ϴ� �����Լ��̴�.
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) {}
	//3��Ī ī�޶󿡼� ī�޶� �ٶ󺸴� ������ �����Ѵ�. �Ϲ������� �÷��̾ �ٶ󺸵��� �����Ѵ�.
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt) {}

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);


	//����ü(���� ��ǥ��)�� �����Ѵ�. 
	void GenerateFrustum();
	//�ٿ�� �ڽ�(OOBB, ���� ��ǥ��)�� ����ü�� ���ԵǴ� ���� �˻��Ѵ�. 
	bool IsInFrustum(BoundingOrientedBox& xmBoundingBox);
};

class CSpaceShipCamera : public CCamera
{
public:
	CSpaceShipCamera(CCamera* pCamera);
	virtual ~CSpaceShipCamera() { }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};


class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CCamera* pCamera);
	virtual ~CFirstPersonCamera() {}
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CCamera* pCamera);
	virtual ~CThirdPersonCamera() { }
	
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
};