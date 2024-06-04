#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Object.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
protected:
	float						fullHP;
	float						currentHP;
	XMFLOAT3					m_xmf3Position;
	XMFLOAT3					m_xmf3Right;
	XMFLOAT3					m_xmf3Up;
	XMFLOAT3					m_xmf3Look;

	float           			m_fPitch;
	float           			m_fYaw;
	float           			m_fRoll;

	XMFLOAT3					m_xmf3Velocity;
	XMFLOAT3     				m_xmf3Gravity;
	float           			m_fMaxVelocityXZ;
	float           			m_fMaxVelocityY;
	float           			m_fFriction;

	LPVOID						m_pPlayerUpdatedContext;
	LPVOID						m_pCameraUpdatedContext;

	CCamera						*m_pCamera = NULL;
	CGameObject*				gunObj =nullptr;
public:
	static CMesh*	m_pExplosionMesh;
	static XMFLOAT3	m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
	static void PrepareExplosion(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	void GetDamage();
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }
	void SetPosition(float x, float y, float z) { Move(XMFLOAT3(x - m_xmf3Position.x, y - m_xmf3Position.y, z - m_xmf3Position.z), false); }
	void ChangeMoveDirection();
	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }
	void LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up);
	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	void Rotate(float x, float y, float z);

	void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
	
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Fire();
	void Recoil();
	void UpdateRecoil(float fTimeElapsed);
	virtual void InitBullets(CMesh* pbullet, float speed);
	virtual void InitExplosionParticle();
public:
	std::vector<CBulletObject*>m_ppBullets;
	std::vector<CExplosionCubeObject*>m_exp;

protected:
	bool m_bIsRecoiling{};
	float m_fCurrentRecoil = 0.0f;
	float m_fRecoilSpeed = 20.0f;
	float m_fRecoilLength = 400.f;
	XMFLOAT3 m_Recoiloffset{};
	XMFLOAT3 m_xmf3OriginalPosition{};
	float m_fRecoilTime{};
	XMFLOAT4X4	m_pxmf4x4Transforms[EXPLOSION_DEBRISES];

	float m_fElapsedTimes = 0.0f;
	float m_fDuration = 5.f;
	float m_fExplosionSpeed = 30.0f;
	float m_fExplosionRotation = 720.0f;
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CAirplanePlayer();

	CGameObject* HandObject = NULL;
	CGameObject*				m_pTailRotorFrame = NULL;

private:
	virtual void OnInitialize();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent = NULL);

public:
	virtual	void Fire() override;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();
};



class CEnemyObject : public CPlayer
{
public:
	virtual void OnInitialize();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
	virtual void SetTarget(CGameObject* target) { m_target = target; }
	virtual void SetTerrain(CHeightMapTerrain* target) { m_pTerrain = target; }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Update() override;
public:
	CGameObject* m_BodyObject;
	float m_fBulletFireDelay{ 4.f };
	float m_fTimeSinceLastBarrage{};
	XMFLOAT3 m_RandomDirection{};
	float m_ChangeDirectionInterval{ 5.f };
	float m_TimeSinceLastDirectionChange{ 5.f };
	float m_timeSinceLastRotation{};
	XMFLOAT3 m_xmf3TerrainScale;
	virtual void Fire() override;
private:
	CGameObject* m_target{};
protected:
	CHeightMapTerrain* m_pTerrain{};
};



class CBossObject :public CEnemyObject
{
public:
	virtual void OnInitialize();
};