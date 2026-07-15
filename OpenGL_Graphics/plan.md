# Project Plan: 3D Graphics Engine Roadmap

본 문서는 그래픽스 렌더러 에디터 프로젝트의 3단계 전체 개발 로드맵 및 차세대 **PBR 라이팅** / **물리 시스템 연동** 구현 계획을 나타내는 공식 계획서(plan.md)입니다.

---

## 1. ⚙️ [완료] 1단계: 엔진 코어 완성 (Engine Core)
지금까지 구현 및 조율이 왼료된 핵심 조작 엔진 사양입니다.

* **Dear ImGui 에디터 통합**: 외부 링킹 없이 소스 코드를 프로젝트에 정적 빌드하고 Precompiled Header(stdafx.h)에 주입하여 컴파일 속도를 극대화했습니다.
* **기하도형 생성기 (Geometry Generator)**: 큐브, UV 구체(Sphere), 삼각뿔(Pyramid)의 수학적 정점들을 실시간 생성하고 이들의 로컬 경계 상자(AABB)를 자동 연산합니다.
* **마우스 픽킹 (Ray-AABB Picking)**: 2D 마우스 입력을 NDC/클립 공간을 거쳐 월드 공간의 Ray로 역산한 뒤, 오브젝트의 로컬 변환 역행렬로 역투영하여 Slab Method 기반의 정밀 로컬 픽킹을 지원합니다.
* **언리얼 스타일 프리 카메라 (Free Navigation)**: 
  * 마우스 우클릭 시 커서 락 및 하이드(`GLFW_CURSOR_DISABLED`).
  * WASD + E/Q + Shift 부스터 가속 비행 제어 및 `F` 키 포커싱 타겟 뷰.
  * 초기 시작 시, 원점을 정확히 **45도 내려다보는** 쿼터뷰 각도 구성.
* **ImGuizmo 3D 마우스 조작기**: 
  * Q(선택), W(이동), E(회전), R(크기 및 중앙 균등 스케일) 기즈모 연동.
  * 행렬 역연산 분해 대칭 조합(`RecomposeMatrixFromComponents`)을 통해 기즈모 드래그 조작 시 조작계 축이 튀는 오류(Gimbal Jump) 방지.
  * 2D ImGui 윈도우 위에서는 레이캐스트 클릭이 투과하지 못하게 호버 마스킹 처리.
* **월드 조감 그리드**: 10cm 격자(0.1 단위) 및 1m 주격자(1.0 단위)와 RGB 월드 기준 중심축 렌더링.

---

## 2. 💡 [진행 예정] 2단계: PBR 라이팅 파이프라인 구현 (Physically Based Rendering)
빛의 물리 법칙에 따라 반사율을 구현하는 정밀 Cook-Torrance BRDF 조명 파이프라인을 구축합니다.

### 2.1. BRDF 조명 물리식 구성
셰이더 내에서 연산할 반사율 방정식 공식입니다:
\[f_r = k_d f_{\text{lambert}} + k_s f_{\text{cook-torrance}}\]

* **Lambertian Diffuse**:
  \[f_{\text{lambert}} = \frac{c}{\pi}\]
* **Cook-Torrance Specular**:
  \[f_{\text{cook-torrance}} = \frac{D \cdot F \cdot G}{4(\omega_o \cdot n)(\omega_i \cdot n)}\]
  * **D (Trowbridge-Reitz GGX)**: 표면 미세면 거칠기의 분포 방향성 묘사.
  * **G (Smith's Schlick-GGX)**: 미세면 자가 음영(Shadowing/Masking) 묘사.
  * **F (Schlick's Approximation)**: 바라보는 시야 시선 입사각에 따른 기본 반사율($F_0$) 변조.

### 2.2. 실시간 재질 속성 제어계 (ImGui UI)
오브젝트별 Inspector 패널에서 다음 물리기반 재질 값을 제어할 수 있도록 셰이더 uniform 바인딩 및 조작 드래그 필드를 구성합니다:
* **Albedo (알베도)**: 표면의 고유 색상값 (RGB 컬러 피커)
* **Metallic (금속성)**: 0.0(비금속) ~ 1.0(순수 금속) 스케일 슬라이더
* **Roughness (거칠기)**: 0.05(매끄러운 재질) ~ 1.0(탁하고 거친 재질) 스케일 슬라이더
* **AO (Ambient Occlusion)**: 미세 표면 밀폐도 음영 가중치 (0.0 ~ 1.0)

### 2.3. 광원 컨트롤 및 포스트 프로세싱 (Post-Processing)
* **광원 제어**: 씬에 조명(Directional 또는 Point Lights)을 추가하고, 이들의 XYZ 위치, 색상, 조도 세기를 조절하는 UI 제작.
* **톤매핑 & 감마 보정**: 선형 조명 공간(Linear Space)에서 연산을 수행한 후, 모니터 출력을 위한 **Reinhard Tone Mapping** 및 **감마 보정($\gamma = 2.2$)** 최종 포스트 프로세스 셰이더 적용.

---

## 3. ☄️ [진행 예정] 3단계: 재질에 따른 물리 시뮬레이션 변화 (Physics)
* 물체에 지정된 PBR 재질 속성(거칠기/마찰력, 금속성에 따른 밀도/무게 등) 수치에 따라 충돌 물리 반응이나 이동 관성, 반발 계수 등 **3D 물리 시뮬레이션 거동이 지능적으로 변하는 상호작용 레이어**를 개발할 예정입니다.
* (상세 스펙과 수식은 2단계 PBR 라이팅 조명 완료 직후 설계 반영)

---

## 🧪 컴파일 및 디버깅 검증 정책
* 본 프로젝트의 컴파일 성공 검증 및 실시간 런타임 버그 디버깅은 **사용자(유저)가 직접 제어하고 검토**하는 것을 약속으로 규정합니다.
