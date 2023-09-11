__start:
j Label1
sub $t3, $t1, $t2
or $t4, $t1, $t2
sub $t5, $t1, $t2
Label1:
ori $t6, $t2, 99
addi $v0, $v0, 10
syscall
