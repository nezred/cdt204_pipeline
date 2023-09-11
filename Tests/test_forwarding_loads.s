.globl __start
.data
EXPECTED: .word 15
.text
__start: 

	addi $s0, $zero, 15
	addi $s2, $zero, 0
	nop
	lw $s1, EXPECTED
	and $s2, $s0, $s1
end: 
	addi $v0, $zero, 10
	syscall
