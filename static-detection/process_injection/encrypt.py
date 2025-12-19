import sys

if len(sys.argv) != 3:
    print(f"Usage: {sys.argv[0]} <input_file> <xor_key>")
    sys.exit(1)

input_filename = sys.argv[1]
xor_key = sys.argv[2].encode()

# 원본 쉘코드 읽기
with open(input_filename, "rb") as input_file:
    raw_shellcode = input_file.read()

# XOR 암호화 수행
encrypted_shellcode = bytes(
    byte ^ xor_key[idx % len(xor_key)]
    for idx, byte in enumerate(raw_shellcode)
)

# 암호화된 파일 저장
output_filename = input_filename.rsplit(".", 1)[0] + "_enc.bin"
with open(output_filename, "wb") as output_file:
    output_file.write(encrypted_shellcode)

print(f"[+] XOR encryption complete: {output_filename}")
