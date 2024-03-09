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

