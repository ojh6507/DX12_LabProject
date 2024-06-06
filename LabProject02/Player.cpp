//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

XMFLOAT3 CPlayer::m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
CMesh* CPlayer::m_pExplosionMesh = NULL;


void CPlayer::PrepareExplosion(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) XMStoreFloat3(&m_pxmf3SphereVectors[i], Random::RandomUnitVectorOnSphere());
	m_pExplosionMesh = new CCubeMesh(pd3dDevice, pd3dCommandList, 3.f, 3.f, 3.f);
}

CPlayer::CPlayer()
{
	m_pCamera = NULL;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 20.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	fullHP = currentHP = 2000.f;
	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

void CPlayer::GetDamage()
{
	currentHP -= 5.f;
	if (currentHP <= 0)
		m_bBlowingUp = true;
}

void CPlayer::ResetOrientationVectors()
{
	m_xmf3Right = XMFLOAT3(1, 0, 0);
	m_xmf3Look = XMFLOAT3(0, 0, 1);
	m_xmf3Up = XMFLOAT3(0, 1, 0);
	OnPrepareRender();
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 horizontalLook = m_xmf3Look;
		horizontalLook.y = 0.0f;
		horizontalLook = Vector3::Normalize(horizontalLook);

		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, horizontalLook, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, horizontalLook, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);
		
		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::ChangeMoveDirection()
{
	Vector3::ScalarProduct(m_xmf3Look, -1,false);
}

void CPlayer::LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
	}
	if(m_pCamera)
		m_pCamera->Move(xmf3Shift);
}

void CPlayer::Rotate(float x, float y, float z)
{
	if (m_bBlowingUp) {
		return;
	}
	DWORD nCameraMode = m_pCamera->GetMode();
	if (nCameraMode == FIRST_PERSON_CAMERA) {
		if (x != 0.0f) {
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f) {
			//로컬 y-축을 중심으로 회전하는 것은 몸통을 돌리는 것이므로 회전 각도의 제한이 없다. 
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f) {
			/*로컬 z-축을 중심으로 회전하는 것은 몸통을 좌우로 기울이는 것이므로 회전 각도는 -20.0~+20.0도 사이로 제한된다. z는 현재의 m_fRoll에서 실제 회전하는 각도이므로 z만큼 회전한 다음 m_fRoll이 +20도 보다 크거나 -20도보다
			작으면 m_fRoll이 +20도 또는 -20도가 되도록 회전각도(z)를 수정한다.*/
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCameraMode == SPACESHIP_CAMERA || (nCameraMode == THIRD_PERSON_CAMERA)) {
		/*스페이스-쉽 카메라에서 플레이어의 회전은 회전 각도의 제한이 없다. 그리고 모든 축을 중심으로 회전을 할 수 있다.*/
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look),
				XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	/*회전으로 인해 플레이어의 로컬 x-축, y-축, z-축이 서로 직교하지 않을 수 있으므로
	z-축(Look 벡터)을 기준으로 하여 서로 직교하고 단위벡터가 되도록 한다.*/
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{
	/*플레이어의 속도 벡터를 중력 벡터와 더한다.
	중력 벡터에 fTimeElapsed를 곱하는 것은 중력을 시간에 비례하도록 적용한다는 의미이다.*/
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ) {
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	/*플레이어의 속도 벡터의 y-성분의 크기를 구한다.
	이것이 y-축 방향의 최대 속력보다 크면 속도 벡터의 y-방향 성분을 조정한다.*/
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라도 이동될 것이다).
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);
	/*플레이어의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 플레이어의 새로운 위치가 유효한 위치가 아닐 수도
	있고 또는 플레이어의 충돌 검사 등을 수행할 필요가 있다. 이러한 상황에서 플레이어의 위치를 유효한 위치로 다시
	변경할 수 있다.*/
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	DWORD nCameraMode = m_pCamera->GetMode();
	//플레이어의 위치가 변경되었으므로 3인칭 카메라를 갱신한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	//카메라의 카메라 변환 행렬을 다시 생성한다. 
	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

}


CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
			pNewCamera = new CFirstPersonCamera(m_pCamera);
			break;
		case THIRD_PERSON_CAMERA:
			pNewCamera = new CThirdPersonCamera(m_pCamera);
			break;
		case SPACESHIP_CAMERA:
			pNewCamera = new CSpaceShipCamera(m_pCamera);
			break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
	m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
	m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
	m_xmf4x4Transform._41 = m_xmf3Position.x; m_xmf4x4Transform._42 = m_xmf3Position.y; m_xmf4x4Transform._43 = m_xmf3Position.z;

	UpdateTransform(NULL);
}

void CPlayer::Fire()
{
	if (m_bBlowingUp) return;
	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < m_ppBullets.size(); i++) {
		if (!m_ppBullets[i]->m_bActive) {
			pBulletObject = m_ppBullets[i];
			break;
		}
	}
	CPlayer::OnPrepareRender();

	if (pBulletObject) {
		XMFLOAT3 xmf3Position = GetPosition();
		XMFLOAT3 xmf3Direction = GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 2.0f, false));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);
	}
	
}
void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
		CGameObject::Render(pd3dCommandList, pCamera);
}

void CPlayer::InitBullets(CMesh* pbullet, float speed, float lifetime)
{
	for (int i = 0; i < BULLETS; i++) {
		CBulletObject* bullet = new CBulletObject();
		bullet->SetMesh(pbullet);
		bullet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		bullet->SetRotationSpeed(300.0f);
		bullet->m_fMovingSpeed = speed;
		bullet->m_fLockingTime = lifetime;
		bullet->SetActive(false);
		bullet->SetShader(CMaterial::m_pIlluminatedShader);
		m_ppBullets.push_back(bullet);
	}
}

void CPlayer::InitExplosionParticle()
{
	for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
		CExplosionCubeObject* EXP = new CExplosionCubeObject();
		EXP->SetMesh(m_pExplosionMesh);
		EXP->SetShader(CMaterial::m_pIlluminatedShader);
		m_exp.push_back(EXP);
	}
}


void CPlayer::Recoil()
{
	if (!m_bIsRecoiling) {
		m_fRecoilTime = 0;
		m_Recoiloffset = XMFLOAT3(0, 0, 0);
		m_fCurrentRecoil = m_fRecoilLength;
		m_bIsRecoiling = true;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAirplanePlayer

CAirplanePlayer::CAirplanePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

//	CGameObject *pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Apache.bin");
	CGameObject *pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/FlyerPlayership.bin");

	pGameObject->SetScale(.5f,.5f,.5f);
	SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));
	SetChild(pGameObject, true);

	CCubeMesh* bulletMesh = new CCubeMesh(pd3dDevice, pd3dCommandList, 8.f, 8.f, 8.f);
	CSphereMesh* pSphere = new CSphereMesh(pd3dDevice, pd3dCommandList, 90.f, 20.f, 20.f);
	OnInitialize();
	InitBullets(bulletMesh,500.f, 0.5f);
	InitBarrier(pSphere);
	InitExplosionParticle();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

CAirplanePlayer::~CAirplanePlayer()
{
}


void CAirplanePlayer::OnInitialize()
{
//	m_pMainRotorFrame = FindFrame("rotor");
//	m_pTailRotorFrame = FindFrame("black_m_7");

	m_BodyObject = FindFrame("FlyerPlayership");
	UpdateBoundingBox(m_BodyObject->m_pMesh);
}

void CAirplanePlayer::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_bBlowingUp) {
		m_fElapsedTimes += fTimeElapsed;
		if (m_fElapsedTimes <= m_fDuration) {
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();
				
				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;

				XMVECTOR axis = XMLoadFloat3(&m_pxmf3SphereVectors[i]);
				XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, m_fExplosionRotation * m_fElapsedTimes);

				XMMATRIX transformMatrix = XMLoadFloat4x4(&m_pxmf4x4Transforms[i]);
				transformMatrix = XMMatrixMultiply(rotationMatrix, transformMatrix);

				XMStoreFloat4x4(&m_pxmf4x4Transforms[i], transformMatrix);
			}

		}
		else {
		
			currentHP = fullHP;
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
		}
	
	}
	else {
		if (m_bBarrier) {
			m_pBarrierObject->Animate(fTimeElapsed);
			m_pBarrierObject->SetPosition(GetPosition());
		}
		CPlayer::Animate(fTimeElapsed, pxmf4x4Parent);
		for (auto& obj : m_ppBullets) {
			if (obj->m_bActive) obj->Animate(fTimeElapsed);
		}
	}
}

void CAirplanePlayer::OnPrepareRender()
{
	if (m_bBlowingUp) {
		return;
	}
	CPlayer::OnPrepareRender();
}


void CAirplanePlayer::Fire()
{
	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < m_ppBullets.size(); i++) {
		if (!m_ppBullets[i]->m_bActive) {
			pBulletObject = m_ppBullets[i];
			break;
		}
	}
	CPlayer::OnPrepareRender();

	if (pBulletObject) {
		XMFLOAT3 xmf3Position = GetPosition();
		OnPrepareRender();
		XMFLOAT3 xmf3Direction = GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 3.0f, false));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);
	}
	Recoil();
}

void CAirplanePlayer::InitBarrier(CMesh* pSphere)
{
	CBarrierObject* barrier = new CBarrierObject();
	barrier->SetMesh(pSphere);
	barrier->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	barrier->SetRotationSpeed(300.0f);
	barrier->SetShader(CMaterial::m_pWireframeShader);
	m_pBarrierObject = barrier;
}

void CAirplanePlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (!m_bGameStart) return;
	if (m_bBlowingUp) {

		for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
			m_exp[i]->m_xmf4x4World = m_pxmf4x4Transforms[i];
			m_exp[i]->Render(pd3dCommandList, pCamera);

		}
	}
	else {

		if (m_bBarrier) m_pBarrierObject->Render(pd3dCommandList, pCamera);
		CPlayer::Render(pd3dCommandList, pCamera);
		for (auto& obj : m_ppBullets) {
			if (obj->m_bActive)obj->Render(pd3dCommandList, pCamera);

		}
	}
}

void CAirplanePlayer::SetBulletResetTimer(float t)
{
	for (auto& obj : m_ppBullets) {
		if (obj->m_bActive)obj -> SetTimer(t);

	}
}

CCamera *CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
			SetFriction(130.f);
			SetGravity(XMFLOAT3(0.0f, -50.0f, 0.0f));
			SetMaxVelocityXZ(70.0f);
			SetMaxVelocityY(120.0f);
			m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.0f);
			m_pCamera->SetOffset(XMFLOAT3(-25.0f, 25.0f, -25.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		case SPACESHIP_CAMERA:
			SetFriction(100.5f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(40.0f);
			SetMaxVelocityY(40.0f);
			m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.0f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		case THIRD_PERSON_CAMERA:
			SetFriction(20.5f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(400.5f);
			SetMaxVelocityY(40.0f);
			m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.25f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 50.0f, -200.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		default:
			break;
	}

	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	Update(fTimeElapsed);

	return(m_pCamera);
}


void CEnemyObject::OnInitialize()
{
	m_BodyObject =FindFrame("orb");
	UpdateBoundingBox(m_BodyObject->m_pMesh);
}

void CEnemyObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CPlayer::OnPrepareRender();
	if (m_bBlowingUpAvailable) {
		m_fElapsedBlowupTimes += fTimeElapsed;
		if (m_fElapsedBlowupTimes > m_fDelay) {
			m_bBlowingUp = true;
		}
	}
	if (m_bBlowingUp) {
		m_fElapsedBlowupTimes = 0;
		m_bBlowingUpAvailable = false;
		m_fElapsedTimes += fTimeElapsed;
		if (m_fElapsedTimes <= m_fDuration) {
			XMFLOAT3 xmf3Position = GetPosition();
			for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
				m_pxmf4x4Transforms[i] = Matrix4x4::Identity();

				m_pxmf4x4Transforms[i]._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._42 = xmf3Position.y + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_pxmf4x4Transforms[i]._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;

				XMVECTOR axis = XMLoadFloat3(&m_pxmf3SphereVectors[i]);
				XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, m_fExplosionRotation * m_fElapsedTimes);

				XMMATRIX transformMatrix = XMLoadFloat4x4(&m_pxmf4x4Transforms[i]);
				transformMatrix = XMMatrixMultiply(rotationMatrix, transformMatrix);

				XMStoreFloat4x4(&m_pxmf4x4Transforms[i], transformMatrix);
			}
		}
		else {
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
		}
	}
	else {
		EnemyMovement(fTimeElapsed);
	}
	for (auto& obj: m_ppBullets) {
		if (obj->m_bActive) obj->Animate(fTimeElapsed);
	}

}

float CEnemyObject::ActivateBlowsUp()
{
	m_bBlowingUpAvailable = true;
	auto sub_v = Vector3::Subtract(m_target->GetPosition(), GetPosition());
	float distance = sqrt(Vector3::Length(sub_v));
	m_fDelay = distance / 90.f;
	return m_fDelay;
}

void CEnemyObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp) {
		for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
			m_exp[i]->m_xmf4x4World = m_pxmf4x4Transforms[i];
			m_exp[i]->Render(pd3dCommandList, pCamera);
		}
	}
	else {
		CPlayer::Render(pd3dCommandList, pCamera);
	}
	for (auto& obj : m_ppBullets) {
		if (obj->m_bActive)obj->Render(pd3dCommandList, pCamera);
		
	}
}

void CEnemyObject::Update()
{
}

int CEnemyObject::PickObjectByRayIntersection(XMFLOAT3& xmf3PickPosition, XMFLOAT4X4& xmf4x4View, float* pfNearHitDistance)
{
	if (!m_BodyObject) return 0;
	int nIntersected = 0;
	if (m_BodyObject->m_pMesh) {
		XMFLOAT3 xmf3PickRayOrigin, xmf3PickRayDirection;
		GenerateRayForPicking(xmf3PickPosition, xmf4x4View, &xmf3PickRayOrigin, &xmf3PickRayDirection);
		nIntersected = m_BodyObject-> m_pMesh->CheckRayIntersection(xmf3PickRayOrigin, xmf3PickRayDirection, pfNearHitDistance);		
	}
	return(nIntersected);
}

void CEnemyObject::EnemyMovement(float fTimeElapsed)
{
	const float targetFrameTime = 1.0f / 60.0f; // 60 FPS 기준 프레임 시간
	const float interpolationFactor = 0.1f; // 보간 계수
	const float fieldOfView = 180.f;
	const float followDistance = 160.f;
	const float targetAvoidanceRadius = 40.f;
	const float avoidanceRadius = 2.f;
	const float separationFactor = 1.4f; // 회피 벡터의 가중치
	const float deadZone = 20.0f;

	// 타겟 방향 및 거리 계산
	XMFLOAT3 targetDirection = Vector3::Subtract(m_target->GetPosition(), GetPosition());
	float targetDistance = Vector3::Length(targetDirection);
	targetDirection = Vector3::Normalize(targetDirection);

	// 타겟 회피 벡터 계산
	XMFLOAT3 targetAvoidanceVector = XMFLOAT3(0, 0, 0);
	if (targetDistance < targetAvoidanceRadius) {
		targetAvoidanceVector = Vector3::Normalize(Vector3::Subtract(GetPosition(), m_target->GetPosition()));
		targetAvoidanceVector = Vector3::ScalarProduct(targetAvoidanceVector, 1 / targetDistance);
	}

	XMFLOAT3 finalDirection = Vector3::Add(targetDirection, Vector3::ScalarProduct(targetAvoidanceVector, separationFactor));
	finalDirection = Vector3::Normalize(finalDirection);

	// 현재 방향을 쿼터니언으로 변환
	XMVECTOR currentLook = XMLoadFloat3(&m_xmf3Look);
	XMVECTOR currentQuat = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_xmf3Position), currentLook, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

	// 목표 방향을 쿼터니언으로 변환
	XMVECTOR targetQuat = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&finalDirection), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

	// SLERP를 사용하여 현재 방향과 목표 방향 사이를 보간
	XMVECTOR newQuat = XMQuaternionSlerp(currentQuat, targetQuat, interpolationFactor);

	// 보간된 쿼터니언을 행렬로 변환하여 방향 설정
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(newQuat);
	XMFLOAT3 newLook;
	XMStoreFloat3(&newLook, XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix));

	// 이동 및 회전 로직 적용
	if ((!m_target->m_bBlowingUp) && targetDistance < followDistance + deadZone) {
		float speedFactor = (targetDistance - (followDistance - deadZone)) / (deadZone + 1);
		float adjustedSpeed = m_fMovingSpeed * speedFactor * targetFrameTime;

		XMFLOAT3 shift = Vector3::ScalarProduct(finalDirection, adjustedSpeed);

		LookAt(m_target->GetPosition(), newLook);
		Move(shift, false);

		XMFLOAT3 actorDirection = Vector3::Normalize(GetLook());
		float angle = acos(Vector3::DotProduct(actorDirection, finalDirection));
		float angleDegrees = angle * (180.0f / XM_PI);


		// 총알 발사 로직
		m_fTimeSinceLastBarrage += fTimeElapsed;
		if ((!m_target->m_bBlowingUp) && targetDistance < 300.f && angleDegrees <= 160.f && m_fTimeSinceLastBarrage >= m_fBulletFireDelay) {
			Fire();
			m_fTimeSinceLastBarrage = 0;
		}
	}
	else {
		// Random patrol logic
		m_TimeSinceLastDirectionChange += fTimeElapsed;

		if (m_TimeSinceLastDirectionChange >= m_ChangeDirectionInterval) {
			m_RandomDirection = XMFLOAT3(rand() / float(RAND_MAX) * 2.0f - 1.0f,
				rand() / float(RAND_MAX) * 2.0f - 1.0f,
				rand() / float(RAND_MAX) * 2.0f - 1.0f);
			m_RandomDirection = Vector3::Normalize(m_RandomDirection);
			m_TimeSinceLastDirectionChange = 0.0f;
		}
		XMFLOAT3 shift = XMFLOAT3(m_RandomDirection.x * m_fMovingSpeed * targetFrameTime,
			m_RandomDirection.y * m_fMovingSpeed * targetFrameTime,
			m_RandomDirection.z * m_fMovingSpeed * targetFrameTime);

		XMFLOAT3 newpos = XMFLOAT3(m_xmf4x4World._41 + shift.x,
			m_xmf4x4World._42 + shift.y,
			m_xmf4x4World._43 + shift.z);

		if (!XMVector3Equal(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&newpos))) {
			LookAt(newpos, newLook);
		}
		m_xmf3Look = m_RandomDirection;

		Move(shift, false);


	}
}


void CEnemyObject::Fire()
{
	if (m_bBlowingUp) return;
	CPlayer::Fire();
}


