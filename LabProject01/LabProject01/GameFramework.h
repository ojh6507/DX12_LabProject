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
	//DXGI 팩토리 인터페이스에 대한 포인터
	ComPtr<IDXGISwapChain3> m_pdxgiswapChain{};
	//스왑 체인 인터페이스에 대한 포인터 이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	ComPtr<ID3D12Device>  m_pd3dDevice{};
	//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위해 필요하다.

	bool m_bMsaa4xEnable{};

	UINT m_nMsaa4xqualityLevels{};
	//msaa 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	static const UINT m_nSwapChainBuffers{ 2 };
	//스왑체인의 후면 버퍼의 개수이다.
	UINT m_nSwapChainBufferIndex{};
	//현재 스왑 체인의 후면 버퍼 인덱스이다.

	ID3D12Resource* m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers]{};
	ComPtr <ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap{};
	UINT m_nRtvDescriptorIncrementSize{};
	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.

	ComPtr <ID3D12Resource> m_pd3dDepthStencilBuffer{};
	ComPtr <ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap{};
	UINT m_nDsvDescriptorIncrementSize{};
	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다. 
	ComPtr <ID3D12CommandQueue>m_pd3dCommandQueue{};
	ComPtr <ID3D12CommandAllocator> m_pd3dCommandAllocator{};
	ComPtr <ID3D12GraphicsCommandList> m_pd3dCommandList{};
	//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.



	ComPtr < ID3D12Fence> m_pd3dFence{};
	//6장에 배열이 추가되면서 삭제됨
	//UINT64 m_nFenceValue;

	HANDLE m_hFenceEvent{};
	//펜스 인터페이스 포인터 ,펜스의 값, 이벤트 핸들이다.


	//다음은 게임 프레임 워크에서 사용할 타이머이다.
	CGameTimer m_GameTimer;
	//다음은 프레임레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다
	_TCHAR m_pszFrameRate[50]{};
	//후면 버퍼마다 현재의 펜스값을 관리하기 위하여 m_nFenceValue 멤버이다.
	UINT64 m_nFenceValues[m_nSwapChainBuffers]{};
	//씬을 그리기 위한 멤버변수
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
	//프레임워크를 초기화하는 함수이다.(주 윈도우가 생성되면 호출된다).
	void OnDestroy();

	void CreateSwapChain();
	void CreateRtvAndDsvDesriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	//스왑 체인, 디바이스 ,서술자 힙 , 명령 큐/할당자/리스트를 생성하는 함수이다.

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//랜더 타겟 뷰와 깊이-스텐실 뷰를 생성하는 함수이다.

	void BuildObjects();
	void ReleaseObjects();
	//랜더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void WaitForGpuComplete();
	//CPU 와 GPU를 동기화하는 함수이다.

	void ChangeSwapChainState();
	//06장 따라하기 추가
	void MoveToNextFrame();
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM  lParam);
	void ProcessSelectedObject(DWORD dwDirection, float cxDelta, float cyDelta);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};