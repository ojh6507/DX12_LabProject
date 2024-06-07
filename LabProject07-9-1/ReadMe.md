# 3D 게임 03
2019180024 오정훈

## 목표
Sc-fi FPS 게임

## 가정
Windows 10/11에서 실행되고 마우스와 키보드로 컨트롤한다는 것을 가정한다.

## 실행 화면 
 ### < Start 화면 >
![alt text](image.png)
 ### < 인 게임 화면 >
![alt text](image-2.png)
![alt text](image-3.png)
## 구현
### Scene 전환
기존 과제01에서 Scene을 스택으로 관리하고, new와 delete과정으로 씬 전환을 구현했었으나.
dx12로 구현되면서 런타임에 리소스 해제, 같은 역할을 하는 루트시그니쳐 생성하는 작업 등을 또 할 필요가 없다고 판단하여, 굳이 여러 Scene들을 인스턴스하지 않고 렌더링/Animate 돼야할 오브젝트들을 분리 했습니다.


### Terrain 
```c++
 // height map GameObject class

```
```c++
 // height map Image Load Code

```
```c++
 // height map Shader class

```

 16bit raw파일을 읽어옴
 GetHeight(float x, float z) 함수로 높이를 가져옴.
   + GetHeight 함수에서 height map은 TerrainScale 굽해져 있지 않기 때문에 x,z 값은 TerrainScale 나눈 값으로 전달해야한다.
   + 받은 높이 값을 SetPostion으로 높이 업데이트


### 플레이어 객체
1인칭 FPS를 위하여 Unreal의 FPS person과 총 fbx를 추출.
Hand Object -> Gun Object 구성된 계층구조로 불러옴

### 카메라
기존 First Person Camera는 상하 회전이 안됐다.
x값이 변할 때도 회전되도록 수정. -> 그러나 카메라를 x값만큼 회전만 하면 캐릭터와 이질적인 회전 문제 발생했다.
카메라 회전할 때 카메라의 위치도 업데이트하도록 수정.
 ```c++
 // First Person Camera Rotate

```

### Crosshair
Shader.hlsl에 직접 투영좌표를 입력하여 렌더링
쉐이더가 추가됐기 때문에 Crosshair PSO 추가.
Crosshair 메쉬에서 vertex 꼭짓점 수와 토폴로지 정하고 crosshair 쉐이더 pso 생성.
 ```c++
 // Crosshair Rasterizer

```
 ```hlsl
 // Crosshair hlsl
 
```
## Player Fire 로직
기존 플레이어는 Fire될 때 bullet들의 bounding box와 intersect하도록 구현했었으나, 플레이어와 Enemy가
지나치게 가까워지면 충돌되지 않은 문제가 발생했다.
이를 해결하기 위해, 크로스 헤어 (화면 중앙) 픽킹 로직을 수행했다.
Triangle 단위로 검사하진 않고 bounding bux와 ray intersect 검사하도록 구현했다.


### EnemyObject
 + idle 상태 ( Random Patrol )
 + 공격
 + 플레이어 따라가기
 ```c++
 // Enemy Animate

```

