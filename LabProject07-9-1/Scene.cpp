//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
	type = MainMenu;
}

CScene::~CScene()
{
}


void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;

	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.f, 0.f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.1f, 0.f, 0.f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CPlayer::PrepareExplosion(pd3dDevice, pd3dCommandList);

	std::default_random_engine generator; // 랜덤 숫자 생성기
	std::uniform_real_distribution<float> distribution(990.f, 2000.f);
	std::uniform_real_distribution<float> speed(60, 70.f);
	std::uniform_real_distribution<float> yDistribution(100, 200.f);
	{
		m_nMainSceneObjects = 10;
		m_ppMainSceneObjects.reserve(m_nMainSceneObjects);
		CGameObject** alphabet = new CGameObject * [5];
		alphabet[0] = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/s.bin");
		alphabet[1] = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/t.bin");
		alphabet[2] = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/a.bin");
		alphabet[3] = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/r.bin");
		alphabet[4] = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/t.bin");


		for (int i = 0; i < 5; i++) {
			CAlphabetObject* pRotatingObject = new CAlphabetObject();
			pRotatingObject->SetChild(alphabet[i], true);
			pRotatingObject->OnInitialize();
			pRotatingObject->SetPosition(XMFLOAT3((i - 2.4f) * 70.f, 0, 200));
			pRotatingObject->SetRotationSpeed((i + 2) * 20.f);
			pRotatingObject->SetScale(2.f, 2.f, 2.f);
			pRotatingObject->m_fVerticalSpeed = (i + 0.7f);

			pRotatingObject->SetRotationAxis(XMFLOAT3(0, 1, 0));

			char name = 'a' + i;
			if (i == 1 || i == 4) name = 't';
			else if (i == 0) name = 's';
			else if (i == 3) name = 'r';

			pRotatingObject->m_name = name;
			m_ppMainSceneObjects.push_back(pRotatingObject);
		}
		CCubeMesh* boxMesh = new CCubeMesh(pd3dDevice, pd3dCommandList, 30.f, 10.f, 30.f);
		for (int i = 0; i < 5; i++) {
			{
				CButtonCubeObject* pRotatingObject = new CButtonCubeObject();
				pRotatingObject->SetMesh(boxMesh);
				pRotatingObject->SetShader(CMaterial::m_pIlluminatedShader);
				pRotatingObject->OnInitialize();
				pRotatingObject->SetRotationSpeed(180.f);
				pRotatingObject->SetPosition(XMFLOAT3((i - 2.4f) * 70.f, 60, 200));
				pRotatingObject->m_fVerticalSpeed = (i + .7f);
				pRotatingObject->m_fBaseHeight = (-15.f);
				m_ppMainSceneObjects.push_back(pRotatingObject);
			}
			{
				CButtonCubeObject* pRotatingObject = new CButtonCubeObject();
				pRotatingObject->SetMesh(boxMesh);
				pRotatingObject->SetShader(CMaterial::m_pIlluminatedShader);
				pRotatingObject->OnInitialize();
				pRotatingObject->SetRotationSpeed(-180.f);
				pRotatingObject->SetPosition(XMFLOAT3((i - 2.4f) * 70.f, 60, 200));
				pRotatingObject->m_fVerticalSpeed = (i + .7f);
				pRotatingObject->m_fBaseHeight = (40.f);
				m_ppMainSceneObjects.push_back(pRotatingObject);
			}
		}

	}

	{
		m_nGameObjects = 35;
		m_ppGameObjects.reserve(m_nGameObjects);
		CGameObject* pApacheModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/orb.bin");
		CCubeMesh* bulletMesh = new CCubeMesh(pd3dDevice, pd3dCommandList, 2.f, 2.f, 2.f);

		for (int x = 0; x < m_nGameObjects; x++) {
			CEnemyObject* pEnemyObject{};
			pEnemyObject = new CEnemyObject();
			pEnemyObject->SetChild(pApacheModel, true);
			pEnemyObject->InitBullets(bulletMesh, 30);
			pEnemyObject->InitExplosionParticle();
			pEnemyObject->OnInitialize();

			XMFLOAT3 randomPosition;
			randomPosition.x = distribution(generator);
			randomPosition.y = yDistribution(generator);
			randomPosition.z = distribution(generator);

			pEnemyObject->SetPosition(randomPosition);
			pEnemyObject->SetScale(1.5f, 1.5f, 1.5f);

			pEnemyObject->m_fMovingSpeed = speed(generator);
			m_ppGameObjects.push_back(pEnemyObject);
		}

		CGameObject* pUFOModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList,
			m_pd3dGraphicsRootSignature, "Model/UFO.bin");

		for (int x = 0; x < 10; x++) {
			CBossObject* pEnemyObject{};
			pEnemyObject = new CBossObject();
			pEnemyObject->SetChild(pUFOModel, true);
			pEnemyObject->InitBullets(bulletMesh, 30);
			pEnemyObject->InitExplosionParticle();
			pEnemyObject->SetScale(2.5f, 2.5f, 2.5f);
			pEnemyObject->OnInitialize();

			XMFLOAT3 randomPosition;
			randomPosition.x = distribution(generator) + (x + 1)*100;
			randomPosition.y = yDistribution(generator);
			randomPosition.z = distribution(generator) + (x + 1) * 100;

			pEnemyObject->SetPosition(randomPosition);
			pEnemyObject->Rotate(0.0f, 90.0f, 0.0f);
			pEnemyObject->m_fMovingSpeed = speed(generator);
			m_ppBossObjects.push_back(pEnemyObject);
		}
	}
	BuildDefaultLightsAndMaterials();
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

void CScene::SetEnemyTarget()
{
	for (int i = 0; i < m_ppGameObjects.size(); i++) {
		((CEnemyObject*)m_ppGameObjects[i])->SetTarget(m_pPlayer);
		((CEnemyObject*)m_ppGameObjects[i])->SetTerrain(m_pTerrain);
		((CEnemyObject*)m_ppGameObjects[i])->m_xmf3TerrainScale = m_xmf3TerrainScale;
	}
	for (int i = 0; i < m_ppBossObjects.size(); i++) {
		((CBossObject*)m_ppBossObjects[i])->SetTarget(m_pPlayer);
		((CBossObject*)m_ppBossObjects[i])->SetTerrain(m_pTerrain);
		((CBossObject*)m_ppBossObjects[i])->m_xmf3TerrainScale = m_xmf3TerrainScale;
	}
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for ( auto& object :m_ppGameObjects) if (object) object->Release();
	for ( auto& object :m_ppMainSceneObjects) if (object) object->Release();


	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_ppGameObjects.size(); i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_ppMainSceneObjects.size(); i++) m_ppMainSceneObjects[i]->ReleaseUploadBuffers();
}

void CScene::CheckObjectByBulletCollisions()
{
	for (int i = 0; i < m_ppGameObjects.size(); i++) {
		CGameObject* object = m_ppGameObjects[i];
		if (object->m_bBlowingUp) continue;
		std::vector<CBulletObject*> ppBullets2 = ((CEnemyObject*)object)->m_ppBullets;
		for (int j = 0; j < ppBullets2.size(); j++) {
			
			if (ppBullets2[j]->m_bActive && m_pPlayer->xmBoundingBox.Intersects(ppBullets2[j]->xmBoundingBox)) {
				auto bullet_pos = ppBullets2[j]->GetPosition();
				auto TerrainHeight = m_pTerrain->GetHeight(bullet_pos.x / m_xmf3TerrainScale.x, bullet_pos.z / m_xmf3TerrainScale.z);

				if (bullet_pos.y < TerrainHeight) {
					ppBullets2[j]->Reset();
					continue;
				}
				m_pPlayer->m_bBlowingUp =true;
			}
		}
	}
}

void CScene::CheckGameEnd()
{
	int count{};
	for (auto& obj : m_ppBossObjects) {
		if (obj->m_bBlowingUp) {
			count++;
		}
		else
			break;
	}
	if (count == m_ppBossObjects.size()) {
		::PostQuitMessage(0);
	}
}

bool CScene::PickObjectPointedByCursor(float xClient, float yClient, CCamera* pCamera)
{
	if (!pCamera) return false;
	XMFLOAT4X4 xmf4x4View = pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT d3dViewport = pCamera->GetViewport();
	XMFLOAT3 xmf3PickPosition;
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다. 그 결과는 카메라
	좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	xmf3PickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
	xmf3PickPosition.z = 1.0f;
	int nIntersected = 0;
	float fHitDistance = FLT_MAX, fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	//셰이더의 모든 게임 객체들에 대한 마우스 픽킹을 수행하여 카메라와 가장 가까운 게임 객체를 구한다. 
	if (type == InGame) {
		for (auto& obj : m_ppGameObjects) {
			if (!obj->IsVisible(m_pCamera)) continue;
			nIntersected = obj->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
			if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance)) {
				fNearestHitDistance = fHitDistance;
				pNearestObject = obj;
			}
		}

		for (auto& obj : m_ppBossObjects) {
			if (!obj->IsVisible(m_pCamera)) continue;
			nIntersected = obj->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
			if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance)) {
				fNearestHitDistance = fHitDistance;
				pNearestObject = obj;
				pNearestObject->m_bBlowingUp = true;
			}
		}

		if (pNearestObject) {
			float delayTime = pNearestObject->ActivateBlowsUp();
			m_pPlayer->SetBulletResetTimer(delayTime);
			return true;
		}
	}

	else {
		char buffer[256];
		XMFLOAT3 yaw = m_pCamera->GetRightVector();
		sprintf_s(buffer, sizeof(buffer), "right vec:  %.2f, %.2f, %.2f\n", yaw.x, yaw.y, yaw.z);
		OutputDebugStringA(buffer);

		// Roll 값을 가져와서 문자열로 변환
		XMFLOAT3 roll = m_pCamera->GetUpVector();
		sprintf_s(buffer, sizeof(buffer), "up vec:  %.2f, %.2f, %.2f\n", roll.x, roll.y, roll.z);
		OutputDebugStringA(buffer);

		// UpVector 값을 가져와서 문자열로 변환
		DirectX::XMFLOAT3 upVector = m_pCamera->GetUpVector();
		sprintf_s(buffer, sizeof(buffer), "UpVector: (%.2f, %.2f, %.2f)\n", upVector.x, upVector.y, upVector.z);
		OutputDebugStringA(buffer);
		for (auto& obj : m_ppMainSceneObjects) {
			if (!obj->IsVisible(m_pCamera)) continue;
			nIntersected = obj->PickObjectByRayIntersection(xmf3PickPosition, xmf4x4View, &fHitDistance);
			if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance)) {
				fNearestHitDistance = fHitDistance;
				pNearestObject = obj;
			}
		}
		if (pNearestObject) {
			return true;
		}

	}

	return false;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_CONTROL:
			if(!m_pPlayer->m_bBlowingUp)
				m_pPlayer->Fire();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;
	if (type == InGame) {
		if (m_pLights) {
			auto p = m_pPlayer->GetPosition();
			p.y += 30.f;
			m_pLights[1].m_xmf3Position = p;
			m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
		}
		for (auto& obj : m_ppGameObjects) {
			obj->Animate(m_fElapsedTime, NULL);
			obj->UpdateTransform(NULL);
		}
		for (auto& obj : m_ppBossObjects) {
			obj->Animate(m_fElapsedTime, NULL);
			obj->UpdateTransform(NULL);
		}
	}
	else {
		for (auto& obj : m_ppMainSceneObjects) {
			obj->Animate(m_fElapsedTime, NULL);
			obj->UpdateTransform(NULL);
		}
	}

}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
	if (type == InGame) {
		if(m_pTerrain)
			m_pTerrain->Render(pd3dCommandList, pCamera);
		if (m_pCrosshair) 
			m_pCrosshair->Render(pd3dCommandList, m_pCamera);

		for (int i = 0; i < m_nGameObjects; i++) {
			if (m_ppGameObjects[i]) {
				if (m_ppGameObjects[i]->IsVisible(pCamera))
					m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
			}
		}
		for (auto& obj : m_ppBossObjects) {
			if (obj) {
				if (obj->IsVisible(pCamera)) {
					obj->Render(pd3dCommandList, pCamera);
				}
			}
		}
		CheckObjectByBulletCollisions();
		CheckGameEnd();
	}
	else {
		for (int i = 0; i < m_ppMainSceneObjects.size(); i++) {
			if (m_ppMainSceneObjects[i]) {
				m_ppMainSceneObjects[i]->Update();
				if (m_ppMainSceneObjects[i]->IsVisible(pCamera)) {}
					m_ppMainSceneObjects[i]->Render(pd3dCommandList, pCamera);
			}
		}
	}
}

