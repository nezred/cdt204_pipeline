__start:
 addi $t1, $t1, 150
 addi $t2, $t2, 48
 addi $t3, $t3, 21
 nop
 nop
 nop
 and $t4, $t1, $t2
 ori $t5, $t3, 16
 sub $t6, $t1, $t2
 nop
 addi $v0, $v0, 10
 syscall
