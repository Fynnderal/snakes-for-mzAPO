	.arch armv7-a
	.fpu vfpv3-d16
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"change_me.c"
	.text
	.section	.rodata
	.align	2
.LC0:
	.ascii	"Hello world\000"
	.align	2
.LC1:
	.ascii	"LED val 0x%x\012\000"
	.align	2
.LC2:
	.ascii	"Goodbye world\000"
	.text
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #64
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7]
	ldr	r2, .L24
.LPIC3:
	add	r2, pc
	ldr	r3, .L24+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #60]
	mov	r3, #0
	movs	r3, #5
	str	r3, [r7, #12]
	ldr	r3, .L24+8
.LPIC0:
	add	r3, pc
	mov	r0, r3
	bl	puts(PLT)
	movs	r0, #1
	bl	sleep(PLT)
	movs	r2, #0
	mov	r1, #16384
	movs	r0, #0
	movt	r0, 17348
	bl	map_phys_address(PLT)
	str	r0, [r7, #32]
	ldr	r3, [r7, #32]
	cmp	r3, #0
	bne	.L2
	movs	r0, #1
	bl	exit(PLT)
.L2:
	movs	r3, #0
	str	r3, [r7, #52]
	mov	r3, #11520
	movt	r3, 305
	str	r3, [r7, #56]
	movs	r3, #0
	str	r3, [r7, #16]
	b	.L3
.L4:
	ldr	r3, [r7, #32]
	adds	r3, r3, #4
	ldr	r2, [r7, #12]
	str	r2, [r3]
	ldr	r3, [r7, #12]
	lsls	r3, r3, #1
	str	r3, [r7, #12]
	ldr	r1, [r7, #12]
	ldr	r3, .L24+12
.LPIC1:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	add	r2, r7, #52
	movs	r3, #0
	movs	r1, #0
	movs	r0, #1
	bl	clock_nanosleep(PLT)
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L3:
	ldr	r3, [r7, #16]
	cmp	r3, #29
	ble	.L4
	movs	r2, #0
	mov	r1, #16384
	movs	r0, #0
	movt	r0, 17344
	bl	map_phys_address(PLT)
	str	r0, [r7, #36]
	ldr	r3, [r7, #36]
	cmp	r3, #0
	bne	.L5
	movs	r0, #1
	bl	exit(PLT)
.L5:
	ldr	r0, [r7, #36]
	bl	parlcd_hx8357_init(PLT)
	movs	r1, #44
	ldr	r0, [r7, #36]
	bl	parlcd_write_cmd(PLT)
	movs	r3, #0
	str	r3, [r7, #16]
	b	.L6
.L9:
	movs	r3, #0
	str	r3, [r7, #20]
	b	.L7
.L8:
	movs	r3, #0
	str	r3, [r7, #48]
	ldr	r3, [r7, #48]
	uxth	r3, r3
	mov	r1, r3
	ldr	r0, [r7, #36]
	bl	parlcd_write_data(PLT)
	ldr	r3, [r7, #20]
	adds	r3, r3, #1
	str	r3, [r7, #20]
.L7:
	ldr	r3, [r7, #20]
	cmp	r3, #480
	blt	.L8
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L6:
	ldr	r3, [r7, #16]
	cmp	r3, #320
	blt	.L9
	movs	r1, #44
	ldr	r0, [r7, #36]
	bl	parlcd_write_cmd(PLT)
	movs	r3, #0
	str	r3, [r7, #16]
	b	.L10
.L13:
	movs	r3, #0
	str	r3, [r7, #20]
	b	.L11
.L12:
	ldr	r3, [r7, #16]
	lsls	r3, r3, #11
	uxth	r2, r3
	ldr	r3, [r7, #20]
	and	r3, r3, #31
	orrs	r3, r3, r2
	str	r3, [r7, #48]
	ldr	r3, [r7, #48]
	uxth	r3, r3
	mov	r1, r3
	ldr	r0, [r7, #36]
	bl	parlcd_write_data(PLT)
	ldr	r3, [r7, #20]
	adds	r3, r3, #1
	str	r3, [r7, #20]
.L11:
	ldr	r3, [r7, #20]
	cmp	r3, #480
	blt	.L12
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L10:
	ldr	r3, [r7, #16]
	cmp	r3, #320
	blt	.L13
	movs	r3, #0
	str	r3, [r7, #52]
	mov	r3, #49664
	movt	r3, 3051
	str	r3, [r7, #56]
	movs	r3, #0
	str	r3, [r7, #28]
	b	.L14
.L21:
	movs	r3, #0
	str	r3, [r7, #24]
	b	.L15
.L20:
	ldr	r3, [r7, #32]
	adds	r3, r3, #36
	ldr	r3, [r3]
	str	r3, [r7, #28]
	ldr	r3, [r7, #28]
	lsrs	r3, r3, #16
	uxtb	r3, r3
	lsrs	r3, r3, #2
	str	r3, [r7, #40]
	ldr	r3, [r7, #28]
	uxtb	r3, r3
	lsrs	r3, r3, #2
	str	r3, [r7, #44]
	movs	r1, #44
	ldr	r0, [r7, #36]
	bl	parlcd_write_cmd(PLT)
	movs	r3, #0
	str	r3, [r7, #16]
	b	.L16
.L19:
	movs	r3, #0
	str	r3, [r7, #20]
	b	.L17
.L18:
	ldr	r2, [r7, #16]
	ldr	r3, [r7, #40]
	add	r3, r3, r2
	lsls	r3, r3, #11
	uxth	r2, r3
	ldr	r1, [r7, #20]
	ldr	r3, [r7, #44]
	add	r3, r3, r1
	and	r3, r3, #31
	orrs	r3, r3, r2
	str	r3, [r7, #48]
	ldr	r3, [r7, #48]
	uxth	r3, r3
	mov	r1, r3
	ldr	r0, [r7, #36]
	bl	parlcd_write_data(PLT)
	ldr	r3, [r7, #20]
	adds	r3, r3, #1
	str	r3, [r7, #20]
.L17:
	ldr	r3, [r7, #20]
	cmp	r3, #480
	blt	.L18
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L16:
	ldr	r3, [r7, #16]
	cmp	r3, #320
	blt	.L19
	add	r2, r7, #52
	movs	r3, #0
	movs	r1, #0
	movs	r0, #1
	bl	clock_nanosleep(PLT)
	ldr	r3, [r7, #24]
	adds	r3, r3, #1
	str	r3, [r7, #24]
.L15:
	ldr	r3, [r7, #24]
	cmp	r3, #59
	ble	.L20
.L14:
	ldr	r3, [r7, #28]
	lsrs	r3, r3, #24
	and	r3, r3, #7
	cmp	r3, #0
	beq	.L21
	ldr	r3, .L24+16
.LPIC2:
	add	r3, pc
	mov	r0, r3
	bl	puts(PLT)
	movs	r3, #0
	ldr	r1, .L24+20
.LPIC4:
	add	r1, pc
	ldr	r2, .L24+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #60]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L23
	bl	__stack_chk_fail(PLT)
.L23:
	mov	r0, r3
	adds	r7, r7, #64
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L25:
	.align	2
.L24:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC3+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC0+4)
	.word	.LC1-(.LPIC1+4)
	.word	.LC2-(.LPIC2+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC4+4)
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",%progbits
