#pragma once
#include "stdafx.h"
class CVertex
{
protected:
	//������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�). 
	XMFLOAT3 m_xmf3Position;
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};
class CDiffusedVertex : public CVertex
{
protected:
	//������ �����̴�. 
	XMFLOAT4 m_xmf4Diffuse;
public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse =
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position =
			XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position =
			xmf3Position; m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

class CIlluminatedVertex : public CVertex
{
protected:
	XMFLOAT3 m_xmf3Normal;
public:
	CIlluminatedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) {
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf3Normal = xmf3Normal;
	}
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) {
		m_xmf3Position = xmf3Position; 
		m_xmf3Normal = xmf3Normal;
	}
	~CIlluminatedVertex() { }
};

class CMesh
{
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();
private:
	int m_nReferences = 0;
	//������ ��ŷ�� ���Ͽ� �����Ѵ�(���� ���۸� Map()�Ͽ� ���� �ʾƵ� �ǵ���).
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	virtual void ReleaseUploadBuffers();
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	BoundingOrientedBox GetBoundingBox() { return(m_xmBoundingBox); }
	int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float* pfNearHitDistance);
protected:
	ID3D12Resource* m_pd3dIndexBuffer = nullptr;
	ID3D12Resource* m_pd3dIndexUploadBuffer = nullptr;
	CDiffusedVertex* m_pVertices = nullptr;
	//�޽��� �ε����� �����Ѵ�(�ε��� ���۸� Map()�Ͽ� ���� �ʾƵ� �ǵ���).
	UINT* m_pnIndices = nullptr;
	
	/*�ε��� ����(�ε����� �迭)�� �ε��� ���۸� ���� ���ε� ���ۿ� ���� �������̽� �������̴�. 
	�ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.*/

	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	UINT m_nIndices = 0;
	//�ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�.
	UINT m_nStartIndex = 0;
	//�ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�. 
	int m_nBaseVertex = 0;
	//�ε��� ������ �ε����� ������ �ε����̴�
	ID3D12Resource* m_pd3dVertexBuffer = nullptr;
	ID3D12Resource* m_pd3dVertexUploadBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
	//�� ��ǥ���� OOBB �ٿ�� �ڽ��̴�. 
	BoundingOrientedBox m_xmBoundingBox;

};

class CCubeMeshDiffused : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�. 
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
	*pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused() {};
};

class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};

class CSphereMeshDiffused : public CMesh
{
public:
	CSphereMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphereMeshDiffused();
};


class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMeshIlluminated();
public:
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, 
											UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions,
											 UINT nVertices, UINT* pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3* pxmf3Normals, XMFLOAT3* pxmf3Positions, 
								int nVertices, UINT* pnIndices, int nIndices);
};

class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, 
						float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};


class CModelMesh : public CMesh
{
public:
	CModelMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName = NULL);
	virtual ~CModelMesh();

private:
	int								m_nReferences = 0;

public:
	
	virtual void ReleaseUploadBuffers() override;

protected:
	XMFLOAT3* m_pxmf3Positions = NULL;
	ID3D12Resource* m_pd3dPositionBuffer = NULL;
	ID3D12Resource* m_pd3dPositionUploadBuffer = NULL;

	XMFLOAT3* m_pxmf3Normals = NULL;
	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;

	XMFLOAT2* m_pxmf2TextureCoords = NULL;
	ID3D12Resource* m_pd3dTextureCoordBuffer = NULL;
	ID3D12Resource* m_pd3dTextureCoordUploadBuffer = NULL;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;

	UINT							m_nVertexBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW* m_pd3dVertexBufferViews = NULL;

	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;

	UINT							m_nStartIndex = 0;
	int								m_nBaseVertex = 0;

	BoundingBox						m_xmBoundingBox;

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* pstrFileName);
};
