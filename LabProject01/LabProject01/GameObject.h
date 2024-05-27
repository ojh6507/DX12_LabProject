#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;

struct MATERIAL
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive;
};
class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	//������ �⺻ ����
	XMFLOAT4 m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//������ ��ȣ
	UINT m_nReflection = 0;
	//������ �����Ͽ� �������� �ϱ� ���� ���̴�
	CShader* m_pShader = NULL;
	void SetAlbedo(XMFLOAT4& xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetShader(CShader* pShader);
};

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
protected:
	CMesh* m_pMesh{};
	float	m_fMovingSpeed{};
	XMFLOAT3 m_xmf3MovingDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);
	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();
public:
	XMFLOAT4X4 m_xmf4x4World;
	CMaterial* m_pMaterial = NULL;
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
public:
	//��� ���۸� �����Ѵ�. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	//��� ������ ������ �����Ѵ�. 
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	void ReleaseUploadBuffers();
	bool IsVisible(CCamera* pCamera = nullptr);
	void SetMovingSpeed(float speed) { m_fMovingSpeed = speed; }
	void SetMovingDirection(XMFLOAT3& xmf3MovingDirection) { m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection); }

	//���� ��ü�� ���� ��ȯ ��Ŀ��� ��ġ ���Ϳ� ����(x-��, y-��, z-��) ���͸� ��ȯ�Ѵ�. 
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	
	//���� ��ü�� ��ġ�� �����Ѵ�. 
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void SetMaterial(CMaterial* pMaterial);
	void SetMaterial(UINT nReflection);

	//���� ��ü�� ���� x-��, y-��, z-�� �������� �̵��Ѵ�.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	//���� ��ü�� ȸ��(x-��, y-��, z-��)�Ѵ�. 
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Move(XMFLOAT3& vDirection, float fSpeed);
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);

	void UpdateBoundingBox();
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	//�� ��ǥ���� ��ŷ ������ �����Ѵ�. 
	void GenerateRayForPicking(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View,
	XMFLOAT3* pxmf3PickRayOrigin, XMFLOAT3* pxmf3PickRayDirection);
	int PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfHitDistance);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
protected:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}
	virtual void Animate(float fTimeElapsed);
};

class CBulletObject : public CRotatingObject
{
public:
	CBulletObject() {};
	virtual ~CBulletObject(){};

public:
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Animate(float fElapsedTime);
	void SetActive(bool bactive) { m_bActive = bactive; }
	void SetFirePosition(XMFLOAT3 xmf3FirePosition);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void Reset();
public:
	bool m_bActive{};
	float						m_fBulletEffectiveRange = 50.0f;
	float						m_fMovingDistance = 0.0f;
	float						m_fRotationAngle = 0.0f;
	XMFLOAT3					m_xmf3FirePosition = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float						m_fElapsedTimeAfterFire = 0.0f;
	float						m_fLockingDelayTime = 0.3f;
	float						m_fLockingTime = 4.0f;
	CGameObject* m_pLockedObject = NULL;

};
