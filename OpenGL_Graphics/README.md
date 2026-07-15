# OpenGL 3D Graphics Renderer & Editor

GLFW와 OpenGL 3.3 Core Profile을 기반으로 구축된 강력한 C++ 3D 그래픽스 렌더러 및 에디터 툴입니다. 대학생 그래픽스 포트폴리오를 위한 수준 높은 비주얼과 에디터 인터페이스를 제공하도록 설계되었습니다.

---

## 🎨 주요 기능 (Key Features)

### 1. 🛠️ 엔진 스타일 3D 기즈모 조작계 (ImGuizmo Integration)
* **Translate (이동 - `W` 키)**: X(적), Y(녹), Z(청) 화살표 드래그 조작.
* **Rotate (회전 - `E` 키)**: Pitch/Yaw/Roll 축별 원형 링 조작.
* **Scale (크기 - `R` 키)**: 축별 스케일 상자 핸들 및 중앙 삼각형 드래그를 이용한 균등 스케일링(Uniform Scaling) 지원.
* **조작계 변환 안정화**: 역조합 행렬 대칭 연산(`RecomposeMatrixFromComponents`)을 적용하여 조작 드래그 시 축 회전이 뒤집히거나 튀는 현상(Gimbal Jump)을 근본적으로 제거했습니다.

### 2. 🎯 정밀 3D 마우스 픽킹 (Ray-AABB Picking)
* 마우스 클릭 시 2D 화면 좌표를 NDC 및 Clip 공간을 거쳐 월드 공간 광선(Ray)으로 역연산합니다.
* 월드 공간 광선을 오브젝트의 로컬 변환 역행렬로 감싸 **오브젝트 로컬 공간에서의 Slab Method Ray-AABB 충돌 판정**을 수행합니다.
* 2D ImGui 패널 및 3D 기즈모 핸들을 붙잡고 있을 때는 레이캐스트 입력을 바이패스하여 오선택을 원천 차단합니다.

### 3. 🎥 언리얼 스타일 내비게이션 (Free Camera & Target Focus)
* **마우스 룩 (Look Around)**: 마우스 우클릭(`RMB`) 상태로 마우스 이동 시 시야 회전. 회전 중에는 마우스 커서가 화면에서 숨겨지고 중앙에 고정(Lock)되며, 우클릭을 해제하면 원래 커서 상태와 위치로 부드럽게 되돌아옵니다.
* **자유 비행 (Flight)**: 우클릭 유지 시 `W`, `A`, `S`, `D` 키로 수평 비행, `E`(상승) / `Q`(하강) 키로 수직 비행. `Left Shift` 입력 시 비행 속도가 3배 빠르게 가속(Boost)됩니다.
* **타겟 포커스 (`F` 키)**: 계층 구조 또는 뷰포트에서 오브젝트 선택 후 `F` 키를 누르면, 카메라가 대상과의 안전 거리를 계산하여 화면 정중앙에 고정(Focusing)합니다.

### 4. 🔷 기하도형 스포너 (Geometry Generator)
* **구체 (Sphere)**: 극좌표계 위/경도 분할 수학을 이용한 동적 생성.
* **큐브 (Cube)**: 6면의 인덱스 좌표계 동적 생성.
* **삼각뿔 (Pyramid)**: 4면 정점 및 밑면 사양 구성.
* 생성과 동시에 모든 기하도형의 정점을 순회하여 로컬 경계상자(AABB)를 실시간 계산 및 갱신합니다.

### 5. 🏁 바닥 조감 그리드 및 축 시각화
* 바닥 평면(`Y = 0`)에 10cm 간격 격자(Minor lines)와 1m 간격 격자(Major lines)를 배치했습니다.
* 월드 중심 X축(빨간색), Z축(파란색) 좌표선을 적용하여 3D 공간 상의 방위각과 거리감을 직관적으로 파악할 수 있습니다.

---

## ⌨️ 단축키 및 조작법 (Controls & Hotkeys)

| 기능 | 조작키 |
| :--- | :--- |
| **선택 도구 모음** | `Q` (선택), `W` (이동 기즈모), `E` (회전 기즈모), `R` (크기 기즈모) |
| **카메라 시야 회전** | 마우스 우클릭 (`RMB`) 유지 + 마우스 이동 (커서 하이드/락 적용) |
| **카메라 공간 비행** | 마우스 우클릭 유지 중 `W`, `A`, `S`, `D` (수평 이동) / `Q`, `E` (수직 강하/상승) |
| **카메라 가속 비행** | 마우스 우클릭 유지 중 `Left Shift` 키 동시 입력 (3배 가속) |
| **선택 객체 화면 집중** | 오브젝트 선택 후 키보드 `F` 키 입력 |
| **에디터 툴 강제 종료**| 키보드 `Escape` 키 입력 |

---

## 📂 폴더 구조 (Directory Structure)

```
OpenGL_Graphics/
├── OpenGL_Graphics/           # 메인 C++ 소스 코드 프로젝트 폴더
│   ├── Components/            # TransformComponent, MeshComponent 등
│   ├── Core/                  # Engine, Shader, GameFramework, EditorPanel
│   ├── Entities/              # GameObject 클래스
│   ├── Managers/              # Camera, Input, Resource, Scene 매니저
│   ├── Shaders/               # 렌더링에 사용되는 VS, FS 소스코드
│   ├── main.cpp               # 프로그램 시작 진입점
│   ├── stdafx.h / cpp         # 미리 컴파일된 헤더 (PCH) 및 글로벌 포함 헤더
│   └── OpenGL_Graphics.vcxproj# 비주얼 스튜디오 프로젝트 속성 파일
├── ThirdParty/                # 외부 라이브러리 폴더 (GLFW, GLEW, GLM, ImGui)
└── OpenGL_Graphics.slnx       # 비주얼 스튜디오 솔루션 파일
```

---

## 🛠️ 빌드 및 실행 방법 (Build & Run)

1. **사전 준비**: 
   * Windows 운영체제 컴퓨터.
   * **Visual Studio 2022** (C++ 데스크톱 개발 환경) 설치 필요.
2. **프로젝트 열기**:
   * 루트 폴더의 `OpenGL_Graphics.slnx` 또는 `OpenGL_Graphics/OpenGL_Graphics.vcxproj` 파일을 실행하여 Visual Studio를 실행합니다.
3. **컴파일 설정**:
   * 상단 구성 패널에서 **`Release`** 및 **`x64`** 구성을 선택합니다.
4. **빌드 및 실행**:
   * 키보드 `Ctrl + Shift + B`를 눌러 솔루션을 빌드합니다.
   * 빌드 완료 후 `F5` 또는 `Ctrl + F5`를 눌러 에디터를 실행합니다.

---

## 💡 로드맵 및 향후 계획

1. **[완료] 1단계: 엔진 프레임워크 완성** (기즈모 조작, 픽킹, 내비게이션, 컴팩트 UI 구성)
2. **[진행 예정] 2단계: PBR 라이팅 파이프라인 연동**
   * Cook-Torrance BRDF 조명 물리 수학 적용.
   * 알베도(Albedo), 금속성(Metallic), 거칠기(Roughness), 음영(AO) 실시간 UI 제어 및 Reinhard 톤매핑/감마보정 셰이더 적용.
3. **[진행 예정] 3단계: 재질에 따른 물리 연동 시뮬레이션** (라이팅 구현 이후 구체화)

---

## 🧪 디버깅 및 컴파일 검증 정책
* 본 프로젝트의 컴파일 유효성 체크 및 코드 디버깅은 **사용자가 직접 수행**하는 계약 조건 하에 작업이 유지되고 있습니다.
