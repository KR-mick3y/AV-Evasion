## 프로세스 할로잉
프로세스 할로잉은 대상 시스템에서 새로운 프로세스를 실행한 뒤, 기존 정상 프로세스의 스레드를 중지하고 악성코드를 실행하는 기법입니다.
프로세스 인젝션과 비슷하지만 새로운 프로세스를 생성한다는 점에서 차이가 있으며, 프로세스 인젝션과 마찬가지로 악성코드를 별도의 바이너리로 저장하지 않고,
메모리에 적재하여 실행하기 때문에 디스크에 악성코드가 남지 않는 파일리스 맬웨어입니다.

본 실습 코드에서 악성코드를 가져오는 방식은 C2 주소에 띄워둔 악성 바이너리를 다운로드 한 뒤, 그 코드를 타켓 프로세스의 가상 메모리에 올려서
실행하는 것으로 다룹니다.

1. xor_encrypt_decrypt : C2로부터 가져올 암호화된 맬웨어를 복호화하는 함수
2. downloadBinary : C2에서 맬웨어 다운로드
3. createSuspendedProcess : 일시 정지 상태의 프로세스 생성
4. allocRemoteMemory : 생성한 프로세스에 가상 메모리 공간 할당
5. writeRemoteMemory : 할당된 프로세스 가상 메모리 공간에 복호화한 맬웨어 삽입

## 사용 가이드
#### 1. msfvenom을 통한 리버스쉘 바이너리 생성 및 암호화(암호화 키 기본 mick3y 하드코딩됨)
<img width="889" height="238" alt="image" src="https://github.com/user-attachments/assets/fd531464-d49d-440e-a6eb-6ad9e298cb6b" />
<br>

#### 2. 생성할 프로세스(기본 notepad.exe로 하드코딩됨)에 주입할 맬웨어 주소 인자로 전달
<img width="788" height="106" alt="image" src="https://github.com/user-attachments/assets/a8e00c7c-95a1-4cef-95c9-0426a44d1ac5" />
<br>

#### 3. 리버스쉘 연결 확인
<img width="799" height="400" alt="image" src="https://github.com/user-attachments/assets/2e60890e-5f56-4f30-b411-9e1a9863ca5e" />
