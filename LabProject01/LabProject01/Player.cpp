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
	//bUpdateVelocity�� ���̸� �÷��̾ �̵����� �ʰ� �ӵ� ���͸� �����Ѵ�. 
	if (bUpdateVelocity) {
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}
}
//�÷��̾ ���� x-��, y-��, z-���� �߽����� ȸ���Ѵ�. 
void CCharacter::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();
	//1��Ī ī�޶� �Ǵ� 3��Ī ī�޶��� ��� �÷��̾��� ȸ���� �ణ�� ������ ������. 
	if (nCameraMode == FIRST_PERSON_CAMERA) {
		/*���� x-���� �߽����� ȸ���ϴ� ���� ���� �յڷ� ���̴� ���ۿ� �ش��Ѵ�. �׷��Ƿ� x-���� �߽����� ȸ���ϴ�
		������ -89.0~+89.0�� ���̷� �����Ѵ�. x�� ������ m_fPitch���� ���� ȸ���ϴ� �����̹Ƿ� x��ŭ ȸ���� ����
		Pitch�� +89�� ���� ũ�ų� -89�� ���� ������ m_fPitch�� +89�� �Ǵ� -89���� �ǵ��� ȸ������(x)�� �����Ѵ�.*/
		if (x != 0.0f) {
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f) {
			//���� y-���� �߽����� ȸ���ϴ� ���� ������ ������ ���̹Ƿ� ȸ�� ������ ������ ����. 
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f) {
			/*���� z-���� �߽����� ȸ���ϴ� ���� ������ �¿�� ����̴� ���̹Ƿ� ȸ�� ������ -20.0~+20.0�� ���̷� ���ѵȴ�. z�� ������ m_fRoll���� ���� ȸ���ϴ� �����̹Ƿ� z��ŭ ȸ���� ���� m_fRoll�� +20�� ���� ũ�ų� -20������
			������ m_fRoll�� +20�� �Ǵ� -20���� �ǵ��� ȸ������(z)�� �����Ѵ�.*/
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		//ī�޶� x, y, z ��ŭ ȸ���Ѵ�. �÷��̾ ȸ���ϸ� ī�޶� ȸ���ϰ� �ȴ�. 
		m_pCamera->Rotate(x, y, z);
		/*�÷��̾ ȸ���Ѵ�. 1��Ī ī�޶� �Ǵ� 3��Ī ī�޶󿡼� �÷��̾��� ȸ���� ���� y-�࿡���� �Ͼ��.
		�÷��̾��� ���� y-��(Up ����)�� �������� ���� z-��(Look ����)�� ���� x-��(Right ����)�� ȸ����Ų��. 
		�⺻������ Up ���͸� �������� ȸ���ϴ� ���� �÷��̾ �ȹٷ� ���ִ� ���� �����Ѵٴ� �ǹ��̴�.*/
		if (y != 0.0f) {
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCameraMode == SPACESHIP_CAMERA || (nCameraMode == THIRD_PERSON_CAMERA)) {
		/*�����̽�-�� ī�޶󿡼� �÷��̾��� ȸ���� ȸ�� ������ ������ ����. �׸��� ��� ���� �߽����� ȸ���� �� �� �ִ�.*/
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
	/*ȸ������ ���� �÷��̾��� ���� x-��, y-��, z-���� ���� �������� ���� �� �����Ƿ�
	z-��(Look ����)�� �������� �Ͽ� ���� �����ϰ� �������Ͱ� �ǵ��� �Ѵ�.*/
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}
//�� �Լ��� �� �����Ӹ��� ȣ��ȴ�. �÷��̾��� �ӵ� ���Ϳ� �߷°� ������ ���� �����Ѵ�. 
void CCharacter::Update(float fTimeElapsed)
{
	/*�÷��̾��� �ӵ� ���͸� �߷� ���Ϳ� ���Ѵ�. 
	�߷� ���Ϳ� fTimeElapsed�� ���ϴ� ���� �߷��� �ð��� ����ϵ��� �����Ѵٴ� �ǹ��̴�.*/
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ) {
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	/*�÷��̾��� �ӵ� ������ y-������ ũ�⸦ ���Ѵ�. 
	�̰��� y-�� ������ �ִ� �ӷº��� ũ�� �ӵ� ������ y-���� ������ �����Ѵ�.*/
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
	//�÷��̾ �ӵ� ���� ��ŭ ������ �̵��Ѵ�(ī�޶� �̵��� ���̴�).
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);
	/*�÷��̾��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. �÷��̾��� ���ο� ��ġ�� ��ȿ�� ��ġ�� �ƴ� ����
	�ְ� �Ǵ� �÷��̾��� �浹 �˻� ���� ������ �ʿ䰡 �ִ�. �̷��� ��Ȳ���� �÷��̾��� ��ġ�� ��ȿ�� ��ġ�� �ٽ�
	������ �� �ִ�.*/
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	DWORD nCameraMode = m_pCamera->GetMode();
	//�÷��̾��� ��ġ�� ����Ǿ����Ƿ� 3��Ī ī�޶� �����Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	//ī�޶��� ��ġ�� ����� �� �߰��� ������ �۾��� �����Ѵ�. 
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	//ī�޶� 3��Ī ī�޶��̸� ī�޶� ����� �÷��̾� ��ġ�� �ٶ󺸵��� �Ѵ�. 
	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	//ī�޶��� ī�޶� ��ȯ ����� �ٽ� �����Ѵ�. 
	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}


CCamera* CCharacter::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	//���ο� ī�޶��� ��忡 ���� ī�޶� ���� �����Ѵ�. 
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
	/*���� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾���
	Up ���͸� ������ǥ���� y-�� ���� ����(0, 1, 0)�� �ǵ��� �Ѵ�. ��, �ȹٷ� ������ �Ѵ�. �׸��� �����̽�-�� ī�޶��� ��� �÷��̾��� �̵����� ������ ����. Ư��, y-�� ������ �������� �����Ӵ�. �׷��Ƿ� �÷��̾��� ��ġ�� ����(��ġ ������ y-��ǥ�� 0���� ũ��)�� �� �� �ִ�. �̶� ���ο� ī�޶� 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾���
	��ġ�� ������ �Ǿ�� �Ѵ�. �׷��Ƿ� �÷��̾��� Right ���Ϳ� Look ������ y ���� 0���� �����. ���� �÷��̾���
	Right ���Ϳ� Look ���ʹ� �������Ͱ� �ƴϹǷ� ����ȭ�Ѵ�.*/
	if (nCurrentCameraMode == SPACESHIP_CAMERA) {
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		/*Look ���Ϳ� ������ǥ���� z-��(0, 0, 1)�� �̷�� ����(����=cos)�� ����Ͽ� �÷��̾��� y-���� ȸ�� ����
		m_fYaw�� �����Ѵ�.*/
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera) {
		/*���ο� ī�޶��� ��尡 �����̽�-�� ����� ī�޶��̰� ���� ī�޶� ��尡 1��Ī �Ǵ� 3��Ī ī�޶��̸� �÷��̾��� ���� ���� ���� ī�޶��� ���� ��� ���� �����.*/
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}
	if (pNewCamera) {
		pNewCamera->SetMode(nNewCameraMode);
		//���� ī�޶� ����ϴ� �÷��̾� ��ü�� �����Ѵ�. 
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
	//ī�޶� ��尡 3��Ī�̸� �÷��̾� ��ü�� �������Ѵ�. 
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
/*3��Ī ī�޶��� �� �÷��̾� �޽��� ���� x-���� �߽����� +90�� ȸ���ϰ� �������Ѵ�. 
�ֳ��ϸ� ����� �� �޽��� ���� �׸��� ���� y-�� ������ ������� ������ �ǵ��� �𵨸��� �Ǿ��� �����̴�. 
�׸��� �� �޽��� ī�޶��� z- �� �������� ���ϵ��� �׸� ���̱� �����̴�.*/

//ī�޶� ������ �� ȣ��Ǵ� �Լ��̴�. nNewCameraMode�� ���� ������ ī�޶� ����̴�.
CCamera *CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		//�÷��̾��� Ư���� 1��Ī ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�. 
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
		//�÷��̾��� Ư���� �����̽�-�� ī�޶� ��忡 �°� �����Ѵ�. �߷��� �������� �ʴ´�. 
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
		//�÷��̾��� Ư���� 3��Ī ī�޶� ��忡 �°� �����Ѵ�. ���� ȿ���� ī�޶� �������� �����Ѵ�.
		SetFriction(100.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(500.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		//3��Ī ī�޶��� ���� ȿ���� �����Ѵ�. ���� 0.25f ��ſ� 0.0f�� 1.0f�� ������ ����� ���ϱ� �ٶ���.
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
	//�÷��̾ �ð��� ����� ���� ����(��ġ�� ������ ����: �ӵ�, ������, �߷� ���� ó��)�Ѵ�. 
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
		const float targetFrameTime = 1.0f / 60.0f; // 60 FPS ���� ������ �ð�
		const float interpolationFactor = 0.1f; // ���� ���
		const float fieldOfView = 180.f;
		const float followDistance = 100.f;
		const float targetAvoidanceRadius = 40.f;
		const float avoidanceRadius = 2.f;
		const float separationFactor = 1.4f; // ȸ�� ������ ����ġ
		const float deadZone = 20.0f;

		// Ÿ�� ���� �� �Ÿ� ���
		XMFLOAT3 targetDirection = Vector3::Subtract(m_target->GetPosition(), GetPosition());
		float targetDistance = Vector3::Length(targetDirection);
		targetDirection = Vector3::Normalize(targetDirection);

		// Ÿ�� ȸ�� ���� ���
		XMFLOAT3 targetAvoidanceVector = XMFLOAT3(0, 0, 0);
		if (targetDistance < targetAvoidanceRadius) {
			targetAvoidanceVector = Vector3::Normalize(Vector3::Subtract(GetPosition(), m_target->GetPosition()));
			targetAvoidanceVector = Vector3::ScalarProduct(targetAvoidanceVector, 1 / targetDistance);
		}

		XMFLOAT3 finalDirection = Vector3::Add(targetDirection, Vector3::ScalarProduct(targetAvoidanceVector, separationFactor));
		finalDirection = Vector3::Normalize(finalDirection);

		// ���� ������ ���ʹϾ����� ��ȯ
		XMVECTOR currentLook = XMLoadFloat3(&m_xmf3Look);
		XMVECTOR currentQuat = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_xmf3Position), currentLook, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

		// ��ǥ ������ ���ʹϾ����� ��ȯ
		XMVECTOR targetQuat = XMQuaternionRotationMatrix(XMMatrixLookToLH(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&finalDirection), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));

		// SLERP�� ����Ͽ� ���� ����� ��ǥ ���� ���̸� ����
		XMVECTOR newQuat = XMQuaternionSlerp(currentQuat, targetQuat, interpolationFactor);

		// ������ ���ʹϾ��� ��ķ� ��ȯ�Ͽ� ���� ����
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(newQuat);
		XMFLOAT3 newLook;
		XMStoreFloat3(&newLook, XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix));

		// �̵� �� ȸ�� ���� ����
		if (targetDistance < followDistance - deadZone) {
			LookAt(m_target->GetPosition(), newLook); // LookAt �Լ��� ���������� ���� ��ȯ ����� ������Ʈ��
		}
		else if (targetDistance < followDistance + deadZone) {
			float speedFactor = (targetDistance - (followDistance - deadZone)) / (deadZone + 1);
			float adjustedSpeed = m_fMovingSpeed * speedFactor * targetFrameTime;

			XMFLOAT3 shift = Vector3::ScalarProduct(finalDirection, adjustedSpeed);
			SetMovingDirection(finalDirection);
			LookAt(m_target->GetPosition(), newLook); // LookAt �Լ��� ���������� ���� ��ȯ ����� ������Ʈ��
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
				LookAt(newpos, newLook); // LookAt �Լ��� ���������� ���� ��ȯ ����� ������Ʈ��
			}
			SetMovingDirection(m_RandomDirection);
			Move(shift, false);
		}
		// ȸ�� ���� ��� �� ����� ���
		XMFLOAT3 actorDirection = Vector3::Normalize(GetLook());
		float angle = acos(Vector3::DotProduct(actorDirection, finalDirection));
		float angleDegrees = angle * (180.0f / XM_PI);


		// �Ѿ� �߻� ����
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

