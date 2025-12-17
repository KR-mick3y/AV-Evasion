## HTTP Request Fileless Malware
HTTP를 통한 파일리스 악성코드는 HTTP 통신을 통해 다운로드 한 바이너리를 메모리에 삽입함으로써 흔적을 남기지 않는 방법입니다.

이 방법은 파일리스 악성코드 중에서 구식 공격 기법에 속하며, 정상 바이너리에서 사용이 드문 API를 호출함에 따라 발각 가능성이 높지만, 파일리스를 공부할 때 기초가 됩니다.

현재 실행중인 프로세스의 메모리에 악성코드를 로드하는 파일리스 기법은 구시대적인 HTTP를 쓰더라도,

프로세스가 실행되기 이전에 악성코드를 검사하는 디펜더의 특성상 msfvenom으로 생성한 쉘코드를 그대로 사용하여도 우회가 가능합니다.

---

### 사용 가이드
#### 1. msfvenom을 통한 바이트 코드 생성
<img width="792" height="156" alt="image" src="https://github.com/user-attachments/assets/cf825b49-605f-4214-b6cc-d73618c41f5d" />
<br><br>

#### 2. 생성한 바이트 코드를 다운로드 할 수 있는 HTTP 서버 오픈
<img width="561" height="109" alt="image" src="https://github.com/user-attachments/assets/cb0320a1-06b6-49b5-8c15-98dc3368a373" />
<br><br>

#### 3. HTTP 통신을 통해 프로세스의 메모리에 악성코드를 로드해서 실행
<img width="686" height="336" alt="image" src="https://github.com/user-attachments/assets/d3643937-476e-415b-9944-0548741a5c0a" />
<br><br>

--- 

## Demo
![2025-12-17 22-34-58](https://github.com/user-attachments/assets/0ccb9289-3540-4320-8b9d-284a78d99aa2)

