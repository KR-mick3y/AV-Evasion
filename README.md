# AV-Evasion
AV/EDR 솔루션의 정적/동적 탐지를 우회하는 방법에 대해 기초적인 실습 코드를 제공하는 프로젝트입니다.

본 프로젝트의 소스코드들은 새로운 기술이 아닌, 이미 알려진 방법 들에 대해 다루고 있습니다.

수록된 정보는 불법적인 목적으로 사용할 것을 금지하며 이를 위반할 경우 작성자에게 책임이 없습니다.
<br><br><br>
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
