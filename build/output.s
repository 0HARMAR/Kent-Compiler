print_int:
	mov %rsi,%r8
	lea buffer(%rip), %rsi
	call itoa
	test %r8,%r8
	mov $10, %rax
	jz skip_call
	call newline
skip_call:
	mov %rax,%r8
	mov $1, %rax
	mov $1, %rdi
	lea buffer(%rip), %rsi
	mov %r8, %rdx
	syscall
	ret

newline:
	lea buffer(%rip), %r8
	movb $0xA, 11(%r8)
	mov $12, %rax
	ret
itoa:
	mov %rdi, %rax
	mov %rsi, %rdi
	mov $10, %rbx
	mov $0, %rcx
	cmp $0, %rax
	jge 1f
	neg %rax
	movb $'-', (%rdi)
	inc %rdi
1:
	cmp $0, %rax
	jnz 2f
	movb $'0', (%rdi)
	inc %rdi
	jmp 4f
2:
	xor %rdx, %rdx
	div %rbx
	add $'0', %dl
	push %dx
	inc %rcx
	test %rax, %rax
	jnz 2b
3:
	pop %dx
	movb %dl, (%rdi)
	inc %rdi
	loop 3b
4:
	movb $0, (%rdi)
	ret

.section .data
buffer: .fill 20
.section .text
.globl _start
.code64
_start:
	# 分配内存到0x500000
	mov $9, %rax
	mov $0x500000, %rdi
	mov $4096, %rsi
	mov $3, %rdx
	mov $0x1022, %r10
	mov $-1, %r8
	xor %r9, %r9
	syscall
	test %rax,%rax
	js .error
	mov $0x500000, %r15
	movl $10, %eax
	movl %eax, 256(%r15)
	movl $20, %eax
	movl %eax, 336(%r15)
	movl $30, %eax
	movl %eax, 512(%r15)
	movl 512(%r15), %eax
	mov %rax, %rdi
	mov $0, %rsi
	call print_int
	movl 512(%r15), %eax
	mov %rax, %rdi
	mov $1, %rsi
	call print_int
	movl $30, %eax
	movl %eax, 768(%r15)
	jmp .exit
.error:
	mov $60, %rax
	mov $1, %rdi
	syscall
.exit:
	mov $60, %rax
	xor %rdi, %rdi
	syscall
