## PPID Spoofing
대부분의 EDR/AV는 복합적인 요소를 평가하여 악성코드를 탐지합니다. 그 중에서 실행된 프로세스의 부모를 통해 의심스러운 활동을 감지할 수 있습니다.
예를 들어 시스템에서 Word가 PowerShell을 호출하는 등의 이상 행위는 즉각 발각될 수 있기 때문에, 이 PowerShell같은 프로그램을
explorer.exe와 같은 정상 범주의 프로세스에서 실행된 것처럼 스푸핑하는 공격이 PPID 스푸핑입니다.

## 사용 가이드
#### 1. ppid_spoofing.cpp 코드에서 부모 프로세스, 자식 프로세스 설정
<img width="691" height="335" alt="3" src="https://github.com/user-attachments/assets/3f0d0005-58ef-403e-8a35-bf8e9bb7dc6a" />

#### 2. ppid_spoofing.cpp 실행 시 그림판의 PPID가 ppid_spoofing.exe가 아닌 메모장으로 설정된 것을 확인
<img width="768" height="344" alt="image" src="https://github.com/user-attachments/assets/c2070991-4d12-4829-bd7f-3eda3e2eaae4" />

#### 3. example.cpp 실행 결과와 비교
<img width="768" height="341" alt="image" src="https://github.com/user-attachments/assets/f1e7e92a-8749-46c0-8cc1-dc8ed6d10feb" />
