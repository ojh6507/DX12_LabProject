#pragma once
#include "Timer.h"
#include "Shader.h"
class CScene
{
public:
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();
	void ReleaseUploadBuffers();
	
	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
protected:
	CShader** m_ppShaders{};
	int m_nShaders{};

private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature{};
	//��Ʈ �ñ׳��ĸ� ��Ÿ���� �������̽� �������̴�. 
	//ID3D12PipelineState* m_pd3dPipelineState{};
	//���������� ���¸� ��Ÿ���� �������̽� �������̴�. 
};

