#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Camera.h"

struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT3 m_xmf3Position;
	float m_fFalloff;
	XMFLOAT3 m_xmf3Direction;
	float m_fTheta; //cos(m_fTheta)
	XMFLOAT3 m_xmf3Attenuation;
	float m_fPhi; //cos(m_fPhi)
	bool m_bEnable;
	int m_nType;
	float m_fRange;
	float padding;
};

struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
};

struct MATERIALS
{
	MATERIAL m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
	//���� ��� ����� ������ ����
	virtual void BuildLightsAndMaterials();
	//���� ��� ����� ������ ���� ���ҽ��� �����ϰ� ����
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseUploadBuffers();
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
protected:
	CObjectsShader* m_pShaders = nullptr;
	int m_nShaders = 0;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = nullptr;

public:
	CCharacter* m_pPlayer = NULL;
protected:
	//���� ����
	LIGHTS* m_pLights = NULL;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�. 
	ID3D12Resource *m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
	//���� ��ü�鿡 ����Ǵ� ����
	MATERIALS* m_pMaterials = NULL;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�. 
	ID3D12Resource *m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;
};

/*
class CMainScene : CScene
{
public:
	//���� ��� ����� ������ ����
	virtual void BuildLightsAndMaterials();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	void ReleaseUploadBuffers();
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
protected:
	CObjectsShader* m_pShaders = nullptr;
	int m_nShaders = 0;
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = nullptr;

public:
	CCharacter* m_pPlayer = NULL;
protected:
	//���� ����
	LIGHTS* m_pLights = NULL;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�. 
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;
	//���� ��ü�鿡 ����Ǵ� ����
	MATERIALS* m_pMaterials = NULL;
	//������ ��Ÿ���� ���ҽ��� ���ҽ��� ���� �������̴�. 
	ID3D12Resource* m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;
};


class SceneManager {
private:
	std::stack<CScene*> sceneStack;
	ID3D12Device* m_pd3dDevice;
	ID3D12GraphicsCommandList* m_pd3dCommandList;
public:
	void PushScene(CScene* scene, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
		if (!sceneStack.empty()) {
			sceneStack.top()->ReleaseObjects();  // ���� �� ��ε�
		}
		scene->BuildObjects(pd3dDevice, pd3dCommandList);  // �� �� �ʱ�ȭ
		sceneStack.push(scene);
	}

	void PopScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
		if (!sceneStack.empty()) {
			sceneStack.top()->ReleaseObjects();  // ���� �� ��ε�
			delete sceneStack.top();
			sceneStack.pop();
			if (!sceneStack.empty()) {
				sceneStack.top()->BuildObjects(pd3dDevice, pd3dCommandList);  // ���� �� �ٽ� �ʱ�ȭ
			}
		}
	}
	void ClearAllScenes() {
		while (!sceneStack.empty()) {
			auto* scene = sceneStack.top();
			scene->ReleaseObjects();
			delete scene;
			sceneStack.pop();
		}
	}

	void UpdateCurrentScene(float fElapsedTime) {
		if (!sceneStack.empty()) {
			sceneStack.top()->AnimateObjects(fElapsedTime);
		}
	}

	void RenderCurrentScene(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {
		if (!sceneStack.empty()) {
			sceneStack.top()->Render(pd3dCommandList, pCamera);
		}
	}
	CBaseScene* GetCurrentScene() {
		if (!sceneStack.empty()) {
			return sceneStack.top();
		}
		return nullptr;
	}
};

*/