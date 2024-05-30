#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"

class CGameFramework {
private:
	HINSTANCE m_hInstance{};
	HWND m_hWnd{};

	int m_nWndClientWidth{};
	int m_nWndClientHeight{};

	ComPtr <IDXGIFactory4> m_pdxgiFactory{};
	//DXGI ���丮 �������̽��� ���� ������
	ComPtr<IDXGISwapChain3> m_pdxgiswapChain{};
	//���� ü�� �������̽��� ���� ������ �̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ComPtr<ID3D12Device>  m_pd3dDevice{};
	//Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���� �ʿ��ϴ�.

	bool m_bMsaa4xEnable{};

	UINT m_nMsaa4xqualityLevels{};
	//msaa ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	static const UINT m_nSwapChainBuffers{ 2 };
	//����ü���� �ĸ� ������ �����̴�.
	UINT m_nSwapChainBufferIndex{};
	//���� ���� ü���� �ĸ� ���� �ε����̴�.

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers]{};
	ComPtr <ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap{};
	UINT m_nRtvDescriptorIncrementSize{};
	//���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.

	ComPtr <ID3D12Resource> m_pd3dDepthStencilBuffer{};
	ComPtr <ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap{};
	UINT m_nDsvDescriptorIncrementSize{};
	//����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�. 
	ComPtr <ID3D12CommandQueue>m_pd3dCommandQueue{};
	ComPtr <ID3D12CommandAllocator> m_pd3dCommandAllocator{};
	ComPtr <ID3D12GraphicsCommandList> m_pd3dCommandList{};
	//��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.



	ComPtr < ID3D12Fence> m_pd3dFence{};
	//6�忡 �迭�� �߰��Ǹ鼭 ������
	//UINT64 m_nFenceValue;

	HANDLE m_hFenceEvent{};
	//�潺 �������̽� ������ ,�潺�� ��, �̺�Ʈ �ڵ��̴�.


	//������ ���� ������ ��ũ���� ����� Ÿ�̸��̴�.
	CGameTimer m_GameTimer;
	//������ �����ӷ���Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�
	_TCHAR m_pszFrameRate[50]{};
	//�ĸ� ���۸��� ������ �潺���� �����ϱ� ���Ͽ� m_nFenceValue ����̴�.
	UINT64 m_nFenceValues[m_nSwapChainBuffers]{};
	//���� �׸��� ���� �������
	CScene* m_pScene;
	CGameObject* m_pSelectedObject = nullptr;
public:
	CCamera* m_pCamera = nullptr;
	CCharacter* m_pPlayer = nullptr;
	POINT m_ptOldCursorPos;
public:
	CGameFramework();
	~CGameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	//�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�.(�� �����찡 �����Ǹ� ȣ��ȴ�).
	void OnDestroy();

	void CreateSwapChain();
	void CreateRtvAndDsvDesriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	//���� ü��, ����̽� ,������ �� , ��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//���� Ÿ�� ��� ����-���ٽ� �並 �����ϴ� �Լ��̴�.

	void BuildObjects();
	void ReleaseObjects();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�.

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	//CPU �� GPU�� ����ȭ�ϴ� �Լ��̴�.

	void ChangeSwapChainState();
	//06�� �����ϱ� �߰�
	void MoveToNextFrame();
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM  lParam);
	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};