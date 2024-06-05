//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};
struct RAY {
	XMFLOAT3 Origin;
	XMFLOAT3 Direction;
};

enum SceneType {
	MAIN_MENU,
	GAME
};

class CBaseScene
{
public:
	SceneType GetType() const { return type; }
	virtual void PickObjectPointedByCursor(float xClient, float yClient, CCamera* pCamera) = 0;
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

public:
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)=0;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)=0;

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) = 0;;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) = 0;;
	virtual void ReleaseShaderVariables() = 0;

	virtual void BuildDefaultLightsAndMaterials() = 0;
	virtual void ReleaseObjects() = 0;


	virtual void AnimateObjects(float fTimeElapsed) = 0;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = nullptr) = 0;;

	virtual void ReleaseUploadBuffers() = 0;;

public:
	CPlayer* m_pPlayer = nullptr;
	CCamera* m_pCamera = nullptr;
	CHeightMapTerrain* m_pTerrain = nullptr;
	CCrosshair* m_CrossHair = nullptr;
	XMFLOAT3 m_xmf3TerrainScale;
protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = nullptr;
	SceneType type;
private:
	std::vector<CGameObject*>m_ppGameObjects;
	int	m_nGameObjects = 0;
	int	m_nBossIndex = 0;

	LIGHT* m_pLights = nullptr;
	
	int	m_nLights = 0;

	XMFLOAT4 m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = nullptr;
	LIGHTS* m_pcbMappedLights = nullptr;
	float m_fElapsedTime = 0.0f;
};

/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
class CMainMenu : public CBaseScene
{
public:
	CMainMenu() {
		type = MAIN_MENU;
	};
public:
	virtual void PickObjectPointedByCursor(float xClient, float yClient, CCamera* pCamera) override;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override {};
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override {};

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override {};
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) override {};
	virtual void ReleaseShaderVariables() override {};

	virtual void BuildDefaultLightsAndMaterials() override;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseObjects() override;

	
	virtual void AnimateObjects(float fTimeElapsed) override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = nullptr);

	virtual void ReleaseUploadBuffers() override;
	
private:
	std::vector<CGameObject*>m_ppGameObjects;
	int	m_nGameObjects = 0;

	LIGHT* m_pLights = nullptr;
	int	m_nLights = 0;
	XMFLOAT4 m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = nullptr;
	LIGHTS* m_pcbMappedLights = nullptr;

	float m_fElapsedTime = 0.0f;
};


/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
class CStageScene : public CBaseScene
{
public:
	CStageScene() {
		type = GAME;
	};
public:
	virtual void PickObjectPointedByCursor(float nxClient, float nyClient, CCamera* pCamera) override;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) override;
	virtual void ReleaseShaderVariables() override;

	virtual void BuildDefaultLightsAndMaterials() override;
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseObjects() override;
	virtual void ReleaseUploadBuffers() override;

	virtual void AnimateObjects(float fTimeElapsed) override;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = nullptr);

public:
	void SetEnemyTarget();
	void CheckObjectByBulletCollisions();
	

public:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = nullptr;
	std::vector<CGameObject*>m_ppGameObjects;

	int	m_nLights = 0;
	int	m_nGameObjects = 0;
	int	m_nBossIndex = 0;

	LIGHT* m_pLights = nullptr;
	XMFLOAT4 m_xmf4GlobalAmbient;
	
	ID3D12Resource* m_pd3dcbLights = nullptr;
	LIGHTS* m_pcbMappedLights = nullptr;

	float m_fElapsedTime = 0.0f;
};

class SceneManager {
private:
	std::stack<CBaseScene*> sceneStack;

public:
	void PushScene(CBaseScene* scene, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
		if (!sceneStack.empty()) {
			sceneStack.top()->ReleaseObjects();  // 현재 씬 언로드
		}
		scene->BuildObjects(pd3dDevice, pd3dCommandList);  // 새 씬 초기화
		sceneStack.push(scene);
	}

	void PopScene(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
		if (!sceneStack.empty()) {
			sceneStack.top()->ReleaseObjects();  // 현재 씬 언로드
			delete sceneStack.top();
			sceneStack.pop();
			if (!sceneStack.empty()) {
				sceneStack.top()->BuildObjects(pd3dDevice, pd3dCommandList);  // 이전 씬 다시 초기화
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

	void RenderCurrentScene(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = nullptr) {
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
