.globl __start
.data

.text
#jmm01: To check if data forwarding is done correctly
__start: 
	addi $s0, $zero, 1
	addi $s1, $zero, 2
	addi $s2, $zero, 3
	addi $s3, $zero, 4
	addi $s4, $zero, 5

	#s5: Sum of "vector"
	addi $s5, $zero, 0

	add $s5, $s5, $s0
	add $s5, $s1, $s5
	add $s5, $s5, $s2
	add $s5, $s3, $s5
	add $s5, $s5, $s4 # $s5 = 1+2+3+4+5 = 15
	
end: 
	addi $v0, $zero, 10
	syscall
