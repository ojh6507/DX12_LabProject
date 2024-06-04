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
{
	if (bUpdateVelocity) {
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else {
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);

		if (gunObj) {
			XMMATRIX worldMatrix = XMLoadFloat4x4(&gunObj->m_xmf4x4World);
			XMMATRIX recoilMatrix = XMMatrixTranslation(m_Recoiloffset.x, m_Recoiloffset.y, m_Recoiloffset.z);
			XMVECTOR positionVector = worldMatrix.r[3];


			// 리코일 오프셋을 위치 벡터에 더함
			XMVECTOR recoilOffset = XMLoadFloat3(&m_Recoiloffset);
			positionVector = XMVectorAdd(positionVector, recoilOffset);

			// 위치 벡터를 다시 행렬에 저장
			worldMatrix.r[3] = positionVector;
			XMStoreFloat4x4(&gunObj->m_xmf4x4World, worldMatrix);
		}
		if (m_pCamera)
			m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	if (m_bBlowingUp) {
		return;
	}
	if (m_pCamera) {

		DWORD nCurrentCameraMode = m_pCamera->GetMode();
		if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
		{
			// 기존 카메라 회전
			m_pCamera->Rotate(x, y, 0);
			if (x != 0.0f) {
				m_fPitch += x;
				if (m_fPitch > +20.0f) { x -= (m_fPitch - 20.0f); m_fPitch = +20.0f; }
				if (m_fPitch < -15.0f) { x -= (m_fPitch + 15.0f); m_fPitch = -15.0f; }

				XMVECTOR axis = XMLoadFloat3(&m_xmf3Right);
				XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, XMConvertToRadians(x));
				m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, rotationMatrix);
				m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, rotationMatrix);
			}
			if (y != 0.0f) {
				m_fYaw += y;
				if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
				if (m_fYaw < 0.0f) m_fYaw += 360.0f;

				XMFLOAT3 upVector = { 0.0f, 1.0f, 0.0f };
				XMVECTOR axis = XMLoadFloat3(&upVector);
				XMMATRIX rotationMatrix = XMMatrixRotationAxis(axis, XMConvertToRadians(y));
				m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, rotationMatrix);
				m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, rotationMatrix);
				m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, rotationMatrix);
			}

		}
	}
}


void CPlayer::Update(float fTimeElapsed)
{
	if (m_bBlowingUp) {
		return;
	}
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Gravity);
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = m_fMaxVelocityY;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);

	Move(xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	if (m_pCamera) {
		DWORD nCurrentCameraMode = m_pCamera->GetMode();
		if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);
		if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
		if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
		m_pCamera->SetLookAt(m_xmf3Position);
		m_pCamera->RegenerateViewMatrix();

		fLength = Vector3::Length(m_xmf3Velocity);
		float fDeceleration = (m_fFriction * fTimeElapsed);
		if (fDeceleration > fLength) fDeceleration = fLength;
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	}
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
void CPlayer::UpdateRecoil(float deltaTime)
{
	if (m_fCurrentRecoil > 0.0f) {
		m_fRecoilTime += deltaTime;
		float normalizedSinValue = (sin(m_fRecoilTime * m_fRecoilSpeed) + 1) * 0.5;
		m_fCurrentRecoil = m_fRecoilLength * normalizedSinValue;

		// 리코일이 거의 완료되었을 때 리코일 종료
		if (m_fRecoilTime >= XM_PI / m_fRecoilSpeed) {
			m_fCurrentRecoil = 0;
			m_fRecoilTime = 0;
			m_Recoiloffset = XMFLOAT3(0, 0, 0);  // 오프셋 초기화
			m_bIsRecoiling = false;  // 리코일 종료
		}
		else {
			XMVECTOR lookVector = XMLoadFloat3(&m_xmf3Look);
			XMVECTOR recoilVector = lookVector * m_fCurrentRecoil;
			XMStoreFloat3(&m_Recoiloffset, recoilVector);
		}
	}


}

void CPlayer::InitBullets(CMesh* pbullet, float speed)
{
	for (int i = 0; i < BULLETS; i++) {
		CBulletObject* bullet = new CBulletObject();
		bullet->SetMesh(pbullet);
		bullet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		bullet->SetRotationSpeed(300.0f);
		bullet->m_fMovingSpeed = speed;
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
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);

//	CGameObject *pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Apache.bin");
	CGameObject *pGameObject = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/GunWith_Mesh_FireIdleMesh1.bin");

	pGameObject->Rotate(-2.0f, -15.f, 0.0f);
	pGameObject->SetScale(8.5f, 8.5f, 8.5f);
	pGameObject->SetPosition(0.f,0.f,0.f);
	SetChild(pGameObject, true);
	CCubeMesh* bulletMesh = new CCubeMesh(pd3dDevice, pd3dCommandList, 8.f, 8.f, 8.f);
	OnInitialize();
	InitBullets(bulletMesh,100.f);
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

	gunObj = FindFrame("gun");
	HandObject = FindFrame("GunWith_Mesh_FireIdleMesh1");
}

void CAirplanePlayer::Animate(float fTimeElapsed, XMFLOAT4X4 *pxmf4x4Parent)
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
		UpdateBoundingBox(HandObject->m_pMesh);
		UpdateRecoil(fTimeElapsed);
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
		XMFLOAT3 xmf3Position = gunObj->GetPosition();
		xmf3Position.y += 10.f;
		gunObj->OnPrepareRender();
		XMFLOAT3 xmf3Direction = gunObj->GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 50.0f, false));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);
	}
	Recoil();
}

void CAirplanePlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp) {

		for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
			m_exp[i]->m_xmf4x4World = m_pxmf4x4Transforms[i];
			m_exp[i]->Render(pd3dCommandList, pCamera);

		}
	}
	else
	{

		CPlayer::Render(pd3dCommandList, pCamera);
		for (auto& obj : m_ppBullets) {
			if (obj->m_bActive)obj->Render(pd3dCommandList, pCamera);

		}
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
			m_pCamera->SetOffset(XMFLOAT3(30.0f, 25.0f, -34.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		case THIRD_PERSON_CAMERA:
			SetFriction(20.5f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(25.5f);
			SetMaxVelocityY(40.0f);
			m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.25f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 105.0f, -140.0f));
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
	m_BodyObject =FindFrame("Sphere");
}

void CEnemyObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	CPlayer::OnPrepareRender();
	if(m_BodyObject)
		CGameObject::UpdateBoundingBox(m_BodyObject->m_pMesh);
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
			m_bBlowingUp = false;
			m_fElapsedTimes = 0.0f;
		}
	}
	else {
		const float targetFrameTime = 1.0f / 60.0f; // 60 FPS 기준 프레임 시간
		const float interpolationFactor = 0.1f; // 보간 계수
		const float fieldOfView = 180.f;
		const float followDistance = 120.f;
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
		if (targetDistance < followDistance - deadZone) {
			LookAt(m_target->GetPosition(), newLook); // LookAt 함수가 내부적으로 월드 변환 행렬을 업데이트함
		}
		else if (targetDistance < followDistance + deadZone) {
			float speedFactor = (targetDistance - (followDistance - deadZone)) / (deadZone + 1);
			float adjustedSpeed = m_fMovingSpeed * speedFactor * targetFrameTime;

			XMFLOAT3 shift = Vector3::ScalarProduct(finalDirection, adjustedSpeed);

			LookAt(m_target->GetPosition(), newLook);

			XMFLOAT3 predictedPosition = Vector3::Add(GetPosition(), shift); // 예상 위치 계산
			float predictedTerrainHeight = m_pTerrain->GetHeight(predictedPosition.x / m_xmf3TerrainScale.x, 
				predictedPosition.z / m_xmf3TerrainScale.z) + 30.f; // 예상 위치의 지형 높이 계산

			// 지형 높이에 따라 shift 조정
			if (predictedPosition.y < predictedTerrainHeight) {
				shift.y = predictedTerrainHeight - GetPosition().y; // y 축 이동량 조정
			}
			else if(predictedPosition.y > 200.f) {
				shift.y = 200.f - GetPosition().y; // y 축 이동량 조정
			}

			Move(shift, false);
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
				LookAt(newpos, newLook); // LookAt 함수가 내부적으로 월드 변환 행렬을 업데이트함
			}
			m_xmf3Look = m_RandomDirection;

			XMFLOAT3 predictedPosition = Vector3::Add(GetPosition(), shift); // 예상 위치 계산
			float predictedTerrainHeight = m_pTerrain->GetHeight(predictedPosition.x / m_xmf3TerrainScale.x, 
				predictedPosition.z / m_xmf3TerrainScale.z) + 30.f; // 예상 위치의 지형 높이 계산

			// 지형 높이에 따라 shift 조정
			if (predictedPosition.y < predictedTerrainHeight) {
				shift.y = predictedTerrainHeight - GetPosition().y; // y 축 이동량 조정
			}
			else if (predictedPosition.y > 200.f) {
				shift.y = 200.f - GetPosition().y;
			}

			Move(shift, false);
			//MoveForward();
		}
		// 회전 각도 계산 및 디버깅 출력
		XMFLOAT3 actorDirection = Vector3::Normalize(GetLook());
		float angle = acos(Vector3::DotProduct(actorDirection, finalDirection));
		float angleDegrees = angle * (180.0f / XM_PI);


		// 총알 발사 로직
		m_fTimeSinceLastBarrage += fTimeElapsed;
		if (targetDistance < 300.f && angleDegrees <= 160.f && m_fTimeSinceLastBarrage >= m_fBulletFireDelay) {
			Fire();
			m_fTimeSinceLastBarrage = 0;
		}
		
	}


	for (auto& obj: m_ppBullets) {
		if (obj->m_bActive) obj->Animate(fTimeElapsed);
	}

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
	if(m_BodyObject)
		CGameObject::UpdateBoundingBox(m_BodyObject->m_pMesh);
}


void CEnemyObject::Fire()
{
	if (m_bBlowingUp) return;
	CPlayer::Fire();
}

void CBossObject::OnInitialize()
{
	m_BodyObject = FindFrame("default");
}
