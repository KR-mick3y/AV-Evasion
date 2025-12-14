## 함수 포인터
함수 포인터는 함수의 주소를 저장하는 포인터입니다.

YARA 룰과 같이 정적 탐지에서는 시그니처, 코드 구조, 공격에 자주 사용되는 함수 등을 탐지합니다.

악성코드에는 필수적으로 사용되는 여러 Windows API들이 존재하며, 대표적으로 VirtualAlloc, CreateThread 등이 있습니다.

Windows API들을 소스코드에서 사용하면 IAT Table에 관련된 모듈과 함수를 호출한 기록이 남게 되며, YARA 룰에도 즉시 걸리게 됩니다.

<img width="696" height="288" alt="3" src="https://github.com/user-attachments/assets/a2a669f3-e4e5-4d69-a33c-79cc0470c691" />


이러한 정적 탐지를 우회하기 위한 방법 중 하나로 함수 포인터의 사용은 IAT Table에 사용한 Windows API 흔적을 남기지 않으며

단어를 조각내서 사용하거나, 연관 없는 변수 명을 사용한다면 시그니처 탐지도 회피 할 수 있습니다.

---

<img width="925" height="157" alt="4" src="https://github.com/user-attachments/assets/3045ef40-39af-4864-9c53-c24ba5078dfe" />

Windows API 함수를 직접 호출하여 VirtualAlloc 사용

<img width="936" height="478" alt="5" src="https://github.com/user-attachments/assets/734883a9-0338-42a8-91dd-8448f7be38dc" />

함수 포인터를 통해 간접적으로 VirtualAlloc 사용


<br><br>

## 사용 가이드

#### 1. msfvenom을 통한 바이트 코드 생성
<img width="793" height="272" alt="image" src="https://github.com/user-attachments/assets/11f9836a-bce3-4fd6-a585-9d9f704fd85a" />
<br><br>

#### 2. 생성한 바이트 코드를 xor_encryptor.c의 페이로드로 삽입
<img width="577" height="168" alt="image" src="https://github.com/user-attachments/assets/6991447f-c6b1-4776-8da5-fb9a3cbfd970" />
<br><br>

#### 3. xor_encryptor.c 컴파일 및 암호화 바이트 코드 생성
<img width="621" height="240" alt="image" src="https://github.com/user-attachments/assets/488fcf15-169a-4f7c-96cf-0ef2061926a5" />
<br><br>

#### 4. 암호화된 바이트 코드 bypass.c의 페이로드로 삽입
<img width="716" height="250" alt="image" src="https://github.com/user-attachments/assets/72fd9bdd-4c77-4a40-b2d1-f58d3254c7c0" />
<br><br>

#### 5. 리버스쉘 연결 대기 및 bypass.c 실행으로 쉘 획득
<img width="624" height="205" alt="image" src="https://github.com/user-attachments/assets/5dee1408-c256-4d3e-8f04-a09fe1afb328" />
<br><br>

## Demo

![2025-12-14 22-29-54 (2)](https://github.com/user-attachments/assets/7a26dced-5c8b-499b-a87d-5dcbdf1a8656)
