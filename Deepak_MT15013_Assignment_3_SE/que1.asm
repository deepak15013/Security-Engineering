section .data
	len equ 1024

section .bss 
	buffer: resb 1024
	fd resb 1

section .text
	global _start

_start:
	jmp msg
	
openFile:
	mov rdi, file
	mov rax, 2
	mov rsi, 0
	syscall
	test rax, rax
	js exitProgram

	mov [fd], rax
	jmp readFile

readFile:
	mov rax, 0
	mov rdi, [fd]
	mov rsi, buffer
	mov rdx, len
	syscall
	test rax, rax
	js exitProgramRead

	jmp writeFile

writeFile:
	mov rax, 1
	mov rdi, 1
	mov rsi, buffer
	mov rdx, len
	syscall
	cmp rax, -1
	je exitProgramWrite

	jmp closeFile

closeFile:
	mov rax, 3
	syscall
	cmp rax, -1
	je exitProgram

	jmp exitProgram
	
exitProgram:
	mov rax, 1
	mov rdi, 1
	mov rsi, error
	mov rdx, 24
	syscall
	mov rax, 60
	mov rdi, 0
	syscall

exitProgramRead:
	mov rax, 1
	mov rdi, 1
	mov rsi, readError
	mov rdx, 11
	syscall
	mov rax, 60
	mov rdi, 0
	syscall

exitProgramWrite:
	mov rax, 1
	mov rdi, 1
	mov rsi, writeError
	mov rdx, 13
	syscall
	mov rax, 60
	mov rdi, 0
	syscall

msg:
	call openFile
	file db "./test.txt", 0
	error db "Error file doesn't exist"
	readError db "Read Error"
	writeError db "Error Writing"


