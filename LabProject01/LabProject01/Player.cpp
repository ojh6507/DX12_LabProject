#include "Player.h"
#include "Shader.h"

CCharacter::CCharacter()
{
	m_pCamera = nullptr;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_pPlayerUpdatedContext = nullptr;
	m_pCameraUpdatedContext = nullptr;
}
CCharacter::~CCharacter()
{
	ReleaseShaderVariables();
	if (m_pCamera) delete m_pCamera;
}

void CCharacter::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}
void CCharacter::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CCharacter::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection) {
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right,-fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);
		Move(xmf3Shift, bUpdateVelocity);
	}
}
void CCharacter::LookAt(const XMFLOAT3& xmf3LookAt, const XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}
void CCharacter::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	//bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다. 
	if (bUpdateVelocity) {
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}
}
//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다. 
void CCharacter::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();
	//1인칭 카메라 또는 3인칭 카메라의 경우 플레이어의 회전은 약간의 제약이 따른다. 
	if (nCameraMode == FIRST_PERSON_CAMERA) {
		/*로컬 x-축을 중심으로 회전하는 것은 고개를 앞뒤로 숙이는 동작에 해당한다. 그러므로 x-축을 중심으로 회전하는
		각도는 -89.0~+89.0도 사이로 제한한다. x는 현재의 m_fPitch에서 실제 회전하는 각도이므로 x만큼 회전한 다음
		Pitch가 +89도 보다 크거나 -89도 보다 작으면 m_fPitch가 +89도 또는 -89도가 되도록 회전각도(x)를 수정한다.*/
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
		//카메라를 x, y, z 만큼 회전한다. 플레이어를 회전하면 카메라가 회전하게 된다. 
		m_pCamera->Rotate(x, y, z);
		/*플레이어를 회전한다. 1인칭 카메라 또는 3인칭 카메라에서 플레이어의 회전은 로컬 y-축에서만 일어난다.
		플레이어의 로컬 y-축(Up 벡터)을 기준으로 로컬 z-축(Look 벡터)와 로컬 x-축(Right 벡터)을 회전시킨다. 
		기본적으로 Up 벡터를 기준으로 회전하는 것은 플레이어가 똑바로 서있는 것을 가정한다는 의미이다.*/
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
//이 함수는 매 프레임마다 호출된다. 플레이어의 속도 벡터에 중력과 마찰력 등을 적용한다. 
void CCharacter::Update(float fTimeElapsed)
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
	//카메라의 위치가 변경될 때 추가로 수행할 작업을 수행한다. 
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	//카메라의 카메라 변환 행렬을 다시 생성한다. 
	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}


CCamera* CCharacter::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	//새로운 카메라의 모드에 따라 카메라를 새로 생성한다. 
	CCamera *pNewCamera = nullptr;
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
	/*현재 카메라의 모드가 스페이스-쉽 모드의 카메라이고 새로운 카메라가 1인칭 또는 3인칭 카메라이면 플레이어의
	Up 벡터를 월드좌표계의 y-축 방향 벡터(0, 1, 0)이 되도록 한다. 즉, 똑바로 서도록 한다. 그리고 스페이스-쉽 카메라의 경우 플레이어의 이동에는 제약이 없다. 특히, y-축 방향의 움직임이 자유롭다. 그러므로 플레이어의 위치는 공중(위치 벡터의 y-좌표가 0보다 크다)이 될 수 있다. 이때 새로운 카메라가 1인칭 또는 3인칭 카메라이면 플레이어의
	위치는 지면이 되어야 한다. 그러므로 플레이어의 Right 벡터와 Look 벡터의 y 값을 0으로 만든다. 이제 플레이어의
	Right 벡터와 Look 벡터는 단위벡터가 아니므로 정규화한다.*/
	if (nCurrentCameraMode == SPACESHIP_CAMERA) {
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		/*Look 벡터와 월드좌표계의 z-축(0, 0, 1)이 이루는 각도(내적=cos)를 계산하여 플레이어의 y-축의 회전 각도
		m_fYaw로 설정한다.*/
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera) {
		/*새로운 카메라의 모드가 스페이스-쉽 모드의 카메라이고 현재 카메라 모드가 1인칭 또는 3인칭 카메라이면 플레이어의 로컬 축을 현재 카메라의 로컬 축과 같게 만든다.*/
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}
	if (pNewCamera) {
		pNewCamera->SetMode(nNewCameraMode);
		//현재 카메라를 사용하는 플레이어 객체를 설정한다. 
		pNewCamera->SetPlayer(this);
	}
	if (m_pCamera) delete m_pCamera;
	return(pNewCamera);
}

void CCharacter::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x;
	m_xmf4x4World._12 = m_xmf3Right.y;
	m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x;
	m_xmf4x4World._22 = m_xmf3Up.y;
	m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x;
	m_xmf4x4World._32 = m_xmf3Look.y;
	m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}
void CCharacter::Animate(float fElapsedTime)
{
	
}
void CCharacter::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	//카메라 모드가 3인칭이면 플레이어 객체를 렌더링한다. 
	if (nCameraMode == THIRD_PERSON_CAMERA) {
		CGameObject::Render(pd3dCommandList, pCamera);
	}
}

CAirplanePlayer::CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CModelMesh* pAirplaneMesh = new CModelMesh(pd3dDevice, pd3dCommandList, "Models/Meshes/FlyerPlayership.bin");

	SetMesh(pAirplaneMesh);
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));
	CreateShaderVariables(pd3dDevice, pd3dCommandList); //Create Camera Shader Varibles 
	CCharacterShader* pShader = new CCharacterShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	SetShader(pShader);

	InitBullets(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}
CAirplanePlayer::~CAirplanePlayer()
{
}


void CAirplanePlayer::OnPrepareRender()
{
	CCharacter::OnPrepareRender();

}
void CAirplanePlayer::Animate(float fElapsedTime)
{
	if (m_bBlowingUp) {

	}
	else {
		for (int i = 0; i < BULLETS; i++) {
			if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Animate(fElapsedTime);
		}
	}
}
void CAirplanePlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_bBlowingUp) {
		
	}
	else {
		CCharacter::Render(pd3dCommandList, pCamera);
		for (auto& bullet : m_ppBullets) {
			if (bullet->m_bActive){
				bullet->UpdateShaderVariables(pd3dCommandList);
				bullet->Render(pd3dCommandList, pCamera);
			}
		}
	}
}
void CAirplanePlayer::InitBullets(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CCubeMeshIlluminated* pBulletMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 1.f, 1.0f, 4.0f);
	CDiffusedShader* pBulletShader = new CDiffusedShader();
	pBulletShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	for (int i = 0; i < BULLETS; i++) {
		CBulletObject* bullet = new CBulletObject();
		bullet->SetMesh(pBulletMesh);
		bullet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		bullet->SetRotationSpeed(360.0f);
		bullet->SetMovingSpeed(360.0f);
		bullet->SetActive(false);
		bullet->SetShader(pBulletShader);
		m_ppBullets.push_back(bullet);
	}
}
/*3인칭 카메라일 때 플레이어 메쉬를 로컬 x-축을 중심으로 +90도 회전하고 렌더링한다. 
왜냐하면 비행기 모델 메쉬는 다음 그림과 같이 y-축 방향이 비행기의 앞쪽이 되도록 모델링이 되었기 때문이다. 
그리고 이 메쉬를 카메라의 z- 축 방향으로 향하도록 그릴 것이기 때문이다.*/

//카메라를 변경할 때 호출되는 함수이다. nNewCameraMode는 새로 설정할 카메라 모드이다.
CCamera *CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		//플레이어의 특성을 1인칭 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다. 
		SetFriction(100.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f,
			1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		//플레이어의 특성을 스페이스-쉽 카메라 모드에 맞게 변경한다. 중력은 적용하지 않는다. 
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		//플레이어의 특성을 3인칭 카메라 모드에 맞게 변경한다. 지연 효과와 카메라 오프셋을 설정한다.
		SetFriction(100.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(500.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다.
		m_pCamera->SetTimeLag(.3f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	//플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)한다. 
	Update(fTimeElapsed);
	return(m_pCamera);
}

void CAirplanePlayer::FireBullet(CGameObject* pLockedObject)
{
	/*
		if (pLockedObject)
		{
			LookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
			OnUpdateTransform();
		}
	*/

	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++)
	{
		if (!m_ppBullets[i]->m_bActive) {
			pBulletObject = m_ppBullets[i];
			break;
		}
	}

	if (pBulletObject) {
		XMFLOAT3 xmf3Position = GetPosition();
		XMFLOAT3 xmf3Direction = GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);

		if (pLockedObject) {
			pBulletObject->m_pLockedObject = pLockedObject;

		}
	}

}
//-------------------------------------------------------------------------------------

CEnemyCharacter::CEnemyCharacter(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
							   ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	InitBullets(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}

CEnemyCharacter::~CEnemyCharacter()
{
}

void CEnemyCharacter::FireBullet(CGameObject* pLockedObject)
{
	/*
		if (pLockedObject)
		{
			LookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
			OnUpdateTransform();
		}
	*/

	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLETS; i++) {
		if (!m_ppBullets[i]->m_bActive) {
			pBulletObject = m_ppBullets[i];
			break;
		}
	}

	if (pBulletObject) {
		XMFLOAT3 xmf3Position = GetPosition();
		XMFLOAT3 xmf3Direction = GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));

		pBulletObject->m_xmf4x4World = m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetActive(true);

		if (pLockedObject) {
			pBulletObject->m_pLockedObject = pLockedObject;

		}
	}
}

void CEnemyCharacter::OnPrepareRender()
{
	CCharacter::OnPrepareRender();
}

void CEnemyCharacter::Animate(float fElapsedTime)
{
	if (m_bBlowingUp) {

	}
	else {
		const float targetFrameTime = 1.0f / 60.0f; // 60 FPS 기준 프레임 시간
		const float interpolationFactor = 0.1f; // 보간 계수
		const float fieldOfView = 180.f;
		const float followDistance = 100.f;
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
			SetMovingDirection(finalDirection);
			LookAt(m_target->GetPosition(), newLook); // LookAt 함수가 내부적으로 월드 변환 행렬을 업데이트함
			Move(shift, false);
		}
		else {
			// Random patrol logic
			m_TimeSinceLastDirectionChange += fElapsedTime;

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
			SetMovingDirection(m_RandomDirection);
			Move(shift, false);
		}
		// 회전 각도 계산 및 디버깅 출력
		XMFLOAT3 actorDirection = Vector3::Normalize(GetLook());
		float angle = acos(Vector3::DotProduct(actorDirection, finalDirection));
		float angleDegrees = angle * (180.0f / XM_PI);


		// 총알 발사 로직
		m_fTimeSinceLastBarrage += fElapsedTime;
		if (targetDistance < 300.f && angleDegrees <= 160.f && m_fTimeSinceLastBarrage >= m_fBulletFireDelay) {
			FireBullet(nullptr);
			m_fTimeSinceLastBarrage = 0;
		}

	}


	for (int i = 0; i < BULLETS; i++) {
		if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Animate(fElapsedTime);
	}

}

void CEnemyCharacter::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender();
	CGameObject::Render(pd3dCommandList, pCamera);
}

void CEnemyCharacter::RenderBullets(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (auto& bullet : m_ppBullets) {
		if (bullet->m_bActive) {
			bullet->UpdateShaderVariables(pd3dCommandList);
			bullet->Render(pd3dCommandList, pCamera);
		}
	}
}

void CEnemyCharacter::InitBullets(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CCubeMeshIlluminated* pBulletMesh = new CCubeMeshIlluminated(pd3dDevice, pd3dCommandList, 1.0f, 1.0f, 4.0f);
	CDiffusedShader* pBulletShader = new CDiffusedShader();
	pBulletShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	for (int i = 0; i < BULLETS; i++) {
		CBulletObject* bullet = new CBulletObject();
		bullet->SetMesh(pBulletMesh);
		bullet->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		bullet->SetRotationSpeed(360.0f);
		bullet->SetMovingSpeed(360.0f);
		bullet->SetActive(false);
		bullet->SetShader(pBulletShader);
		m_ppBullets.push_back(bullet);
	}
}

