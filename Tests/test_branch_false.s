__start:
addi $t1, $t1, 9
addi $t2, $t2, 5
nop
nop
beq $t1, $t2, L1
sub $t3, $t1, $t2
or $t4, $t1, $t2
L1:
addi $v0, $v0, 10
syscall
