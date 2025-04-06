.option norelax

.globl    fibo_limit
.globl    fibo_series

.text
.globl _start

_start:
la x10, fibo_series
la x11, fibo_limit
lw x11, 0(x11)

addi x5, x0, 0

beq x11, x0, no_elements

addi x1, x0, 0
addi x2, x0, 1
addi x3, x0, 1
Loop:
add x3, x2, x1
sw x3, 0(x10)
addi x1, x2, 0
addi x2, x3, 0
addi x10, x10, 4
addi x5, x5, 1
bge x5, x11, end_loop
j Loop





no_elements:
sw x0, 0(x10)

//Final infinite loop
end_loop:
	fence           // flush cache memory
	ebreak          // stop the simulator
	j end_loop
	nop

.data
// .align    2 // not supported by QtRVSim yet
fibo_limit:
	.word 15
fibo_series:
	.skip 1000*4

// Specify location to show in memory window
#pragma qtrvsim focus memory fibo_limit
