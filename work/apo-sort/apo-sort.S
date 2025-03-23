.option norelax

.globl    array_size
.globl    array_start

.text
.globl _start

_start:

	la   x15, array_start
	la   x2, array_size
	lw   x2, 0(x2) // number of elements in the array
	addi x9, x2, -1
	addi x8, x0, 0
    for1:
        addi x3, x0, 0
	    sub x10, x9, x8
        addi x1, x15, 0
        for2:
            lw x4, 0(x1)
            lw x5 , 4(x1)
            
            slt x6, x5, x4
            beq x6, x0, skip
                sw x5, 0(x1)
                sw x4, 4(x1)
            skip:
                addi x1, x1, 4
                addi x3, x3, 1
                bne x3, x10, for2
         addi x8, x8, 1
         bne x8, x2, for1

//Final infinite loop
end_loop:
	fence           // flush cache memory
	ebreak          // stop the simulator
	j end_loop


.data
// .align    2 // not supported by qtrvsim yet

array_start:
.word	0x00000022
.space 200
array_size:
.word 15