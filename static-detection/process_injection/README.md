## 프로세스 인젝션
프로세스 인젝션은 실행 중인 정상 프로세스의 가상 메모리 공간에 악성코드를 삽입하여 실행하는 기법입니다.

악성코드 자체를 별도의 바이너리에 저장하지 않고, 암호화된 상태로 메모리에 적재하여 실행하기 때문에 디스크에 악성코드가 남지 않아 탐지를 우회할 수 있습니다.

프로세스 인젝션에서 악성코드를 가져오는 방식은 다양하게 있지만, 이 페이지에서는 기본적인 실습에 초점을 맞추기 때문에

C2 주소에서 띄워둔 악성코드에 접근해서 다운로드 한 뒤, 그 코드를 타겟 프로세스의 가상 메모리에 올려서 실행하는 것을 다루겠습니다.

process_injection.cpp 파일에서는 본 페이지에서 다루는 프로세스 인젝션에 필요한 Windows API 함수 호출들을 개별적인 사용자 함수로 정의하여 호출합니다.

#### 1. getProcId : 대상 시스템에서 실행 중인 모든 프로세스를 스냅샷으로 저장
#### 2. openProcessByPid : 인자로 지정한 프로세스가 스냅샷 목록에서 있는지 확인 후 핸들 반환
#### 3. downloadBinary : C2에서 페이로드 다운로드
#### 4. allocRemoteMemory : 인자로 지정한 프로세스 내부에 가상 메모리 공간 할당
#### 5. writeRemoteMemory : 가상 메모리 공간에 다운로드 한 페이로드 삽입
#### 6. createRemoteThread : 가상 메모리 공간 속 페이로드를 실행할 원격 쓰레드 생성


---

## 사용 가이드

#### 1. msfvenom을 통한 리버스쉘 바이너리 생성 및 암호화
<img width="1043" height="237" alt="image" src="https://github.com/user-attachments/assets/a4ddeece-c01e-4d7c-8a3e-3149ebbb24b2" />
<br>

#### 2. 인젝션 할 프로세스, C2 주소, xor 암호화에 사용한 키를 인자로 전달하여 process_injection.exe 실행
<img width="659" height="105" alt="image" src="https://github.com/user-attachments/assets/23eb3815-25bc-4c56-8cdb-2d99e83c2faf" />
<br>

#### 3. 리버스쉘 연결 확인
<img width="764" height="343" alt="image" src="https://github.com/user-attachments/assets/bd98a97f-6529-4b05-ab0d-a6bfe1cbef77" />
<br>

---
## Demo
![2025-12-20 02-02-01](https://github.com/user-attachments/assets/949ee4e4-e59a-4dc7-ac18-2afe6182f344)
