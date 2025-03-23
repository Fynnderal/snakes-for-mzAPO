.option norelax

.globl    array_size
.globl    array_start

.text
.globl _start

_start:

	la   x1, array_start
	la   x2, array_size
	lw   x2, 0(x2) // number of elements in the array
	addi x9, x2, -1
	addi x8, x0, 0
    for1:
        addi x3, x0, 0
        sub x10, x9, x8
        la x1, array_start
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
                slt x6, x3, x10
                bne x6, x0, for2
         addi x8, x8, 1
         slt x6, x8, x2
         bne x6, x0, for1

//Final infinite loop
end_loop:
	fence           // flush cache memory
	ebreak          // stop the simulator
	j end_loop


.data
// .align    2 // not supported by qtrvsim yet

array_size:
.word	15
.space 200
array_start:
.word	5, 3, 4, 1, 15, 8, 9, 2, 10, 6, 11, 1, 6, 9, 12