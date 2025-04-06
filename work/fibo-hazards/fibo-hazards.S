.option norelax

.globl    fibo_limit
.globl    fibo_series

.text
.globl _start

_start:

addi a0, x0, 0x300
addi a1, x0, 0x304

addi x1, x0, 1

addi x2, x0, 1
sw x1, 0(a0)
addi x4, x0, 2
lw a1, 0(a1)
sw x2, 4(a0)
addi a0, a0, 4
//	la   a0, fibo_series // la is pseudoinstruction expanded
//	la   a1, fibo_limit  // into two instructions with dependency
	                     // look for replacements in seminaries
//	lw   a1, 0(a1) // number of elements in the array

Loop:
add x3, x2, x1
add x1, x0, x2
addi a0, a0, 4
add x2, x3, x0
bge x4, a1, end_loop
addi x4, x4, 1
sw x2, 0(a0)
j Loop



//Final infinite loop
end_loop:
	fence           // flush cache memory
	ebreak          // stop the simulator
	j end_loop
	nop

.data
// .align    2 // not supported by QtRVSim yet
.org 0x300
fibo_limit:
	.word 42
fibo_series:
	.skip 1000*4

// Specify location to show in memory window
#pragma qtrvsim focus memory fibo_limit
