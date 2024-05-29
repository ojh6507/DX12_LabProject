// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
            // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
///////////////////////////
//23.05.12 다이렉트 파일 추가하기
#include <string>
#include <stack>
#include <vector>
#include <memory>
///////////////////////////
#include <random>
//////////////////////////
#include <wrl.h>	
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXpackedvector.h>
#include <DirectXColors.h>

#include <DirectXCollision.h>
#include <DXGIDebug.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Winmm.lib")

#define FRAME_BUFFER_WIDTH 800	
#define FRAME_BUFFER_HEIGHT 600
#define DegreeToRadian(x)	float((x)*3.141592654f/180.0f)

#define BULLETS					50
#define EXPLOSION_DEBRISES		240

#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))


#define MAX_LIGHTS 8
#define MAX_MATERIALS 8
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define DIRECTIONAL_LIGHT 3

extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
                                            void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
                                            D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                                            ID3D12Resource** ppd3dUploadBuffer = nullptr);


inline int ReadUnityBinaryString(FILE* pFile, char* pstrToken, BYTE* pnStrLength)
{
    UINT nReads = 0;
    nReads = (UINT)::fread(pnStrLength, sizeof(BYTE), 1, pFile);
    nReads = (UINT)::fread(pstrToken, sizeof(char), *pnStrLength, pFile);
    pstrToken[*pnStrLength] = '\0';

    return(nReads);
}


//3차원 벡터의 연산
namespace Vector3
{
    inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, xmvVector);
        return(xmf3Result);
    }
    inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize =
        true)
    {
        XMFLOAT3 xmf3Result;
        if (bNormalize)
            XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) *
                fScalar);
        else
            XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
        return(xmf3Result);
    }
    inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) +
            XMLoadFloat3(&xmf3Vector2));
        return(xmf3Result);
    }
    inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, float fScalar)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
        return(xmf3Result);
    }
    inline XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) -
            XMLoadFloat3(&xmf3Vector2));
        return(xmf3Result);
    }
    inline float DotProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1),
            XMLoadFloat3(&xmf3Vector2)));
        return(xmf3Result.x);
    }
    inline XMFLOAT3 CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, bool
        bNormalize = true)
    {
        XMFLOAT3 xmf3Result;
        if (bNormalize)
            XMStoreFloat3(&xmf3Result,
                XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
                    XMLoadFloat3(&xmf3Vector2))));
        else
            XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
                XMLoadFloat3(&xmf3Vector2)));
        return(xmf3Result);
    }
    inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 m_xmf3Normal;
        XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
        return(m_xmf3Normal);
    }
    inline float Length(const XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
        return(xmf3Result.x);
    }
    inline float Angle(const XMVECTOR& xmvVector1, const XMVECTOR& xmvVector2)
    {
        XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
        return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
    }
    inline float Angle(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
    }
    inline XMFLOAT3 TransformNormal(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector),
            xmmtxTransform));
        return(xmf3Result);
    }
    inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector),
            xmmtxTransform));
        return(xmf3Result);
    }
    inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix)
    {
        return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
    }
}
//4차원 벡터의 연산
namespace Vector4
{
    inline XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) +
            XMLoadFloat4(&xmf4Vector2));
        return(xmf4Result);
    }
    inline XMFLOAT4 Multiply(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) *
            XMLoadFloat4(&xmf4Vector2));
        return(xmf4Result);
    }
    inline XMFLOAT4 Multiply(float fScalar, const XMFLOAT4& xmf4Vector)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
        return(xmf4Result);
    }
}
//행렬의 연산
namespace Matrix4x4
{
    inline XMFLOAT4X4 Identity()
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) *
            XMLoadFloat4x4(&xmmtx4x4Matrix2));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMMATRIX& xmmtxMatrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Inverse(const XMFLOAT4X4& xmmtx4x4Matrix)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(nullptr,
            XMLoadFloat4x4(&xmmtx4x4Matrix)));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Transpose(const XMFLOAT4X4& xmmtx4x4Matrix)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result,
            XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio,
            NearZ, FarZ));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition,
                               const XMFLOAT3& xmf3UpDirection)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition),
            XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
        return(xmmtx4x4Result);
    }
}
