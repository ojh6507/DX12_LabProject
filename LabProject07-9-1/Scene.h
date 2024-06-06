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
enum  SceneType {
	MainMenu,
	InGame
};
class CScene
{
public:
    CScene();
    ~CScene();
	bool PickObjectPointedByCursor(float size_x, float size_y, CCamera* pCamera);
	
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void SetEnemyTarget();
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();
	void CheckObjectByBulletCollisions();
	void CheckGameEnd();

	CPlayer						*m_pPlayer = NULL;
	CCamera* m_pCamera = nullptr;
public:
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = nullptr;
	SceneType type;

	std::vector<CGameObject*>m_ppGameObjects;
	std::vector<CGameObject*>m_ppBossObjects;
	std::vector<CGameObject*>m_ppMainSceneObjects;
	int	m_nGameObjects = 0;
	int	m_nMainSceneObjects = 0;
	int	m_nBossIndex = 0;

	LIGHT*m_pLights = nullptr;
	CHeightMapTerrain* m_pTerrain = nullptr;
	CCrosshair*	m_pCrosshair = nullptr;
	int							m_nLights = 0;

	XMFLOAT4 m_xmf4GlobalAmbient;
	XMFLOAT3 m_xmf3TerrainScale;

	ID3D12Resource	*m_pd3dcbLights = nullptr;
	LIGHTS			*m_pcbMappedLights = nullptr;

	float m_fElapsedTime = 0.0f;
};
