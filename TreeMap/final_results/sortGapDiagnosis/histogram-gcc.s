	.arch armv8.5-a
	.build_version macos,  15, 0
	.text
	.align	6
__Z5countIxLb1EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0:
LFB6347:
	stp	x29, x30, [sp, -64]!
LCFI0:
	mov	x29, sp
LCFI1:
	stp	x19, x20, [sp, 16]
LCFI2:
	mov	x20, x0
	mov	x19, x1
	mov	x0, x2
	mov	w1, 0
	mov	x2, 2048
	str	x21, [sp, 32]
LCFI3:
	mov	w21, w4
	str	w3, [x29, 52]
	str	x5, [x29, 56]
	bl	_memset
	subs	x2, x19, x20
	beq	L1
	ldr	x5, [x29, 56]
	mov	x7, x0
	asr	x2, x2, 3
	uxtw	x4, w21
	mov	x1, 0
	ldr	w3, [x29, 52]
	.p2align 5,,15
L3:
	ldr	x0, [x20, x1, lsl 3]
	add	x1, x1, 1
	sub	x0, x5, x0
	lsr	x0, x0, x3
	and	x0, x0, 255
	eor	x0, x0, x4
	ldr	x6, [x7, x0, lsl 3]
	add	x6, x6, 1
	str	x6, [x7, x0, lsl 3]
	cmp	x1, x2
	bcc	L3
L1:
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 64
LCFI4:
	ret
LFE6347:
	.align	6
__Z5countIxLb0EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0:
LFB6348:
	stp	x29, x30, [sp, -64]!
LCFI5:
	mov	x29, sp
LCFI6:
	stp	x19, x20, [sp, 16]
LCFI7:
	mov	x20, x0
	mov	x19, x1
	mov	x0, x2
	mov	w1, 0
	mov	x2, 2048
	str	x21, [sp, 32]
LCFI8:
	mov	w21, w4
	str	w3, [x29, 52]
	str	x5, [x29, 56]
	bl	_memset
	subs	x2, x19, x20
	beq	L10
	ldr	x5, [x29, 56]
	mov	x7, x0
	asr	x2, x2, 3
	uxtw	x4, w21
	mov	x1, 0
	ldr	w3, [x29, 52]
	.p2align 5,,15
L12:
	ldr	x0, [x20, x1, lsl 3]
	add	x1, x1, 1
	sub	x0, x0, x5
	lsr	x0, x0, x3
	and	x0, x0, 255
	eor	x0, x0, x4
	ldr	x6, [x7, x0, lsl 3]
	add	x6, x6, 1
	str	x6, [x7, x0, lsl 3]
	cmp	x1, x2
	bcc	L12
L10:
	ldr	x21, [sp, 32]
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 64
LCFI9:
	ret
LFE6348:
	.align	6
__Z5countIiLb1EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0:
LFB6349:
	stp	x29, x30, [sp, -48]!
LCFI10:
	mov	x29, sp
LCFI11:
	stp	x19, x20, [sp, 16]
LCFI12:
	mov	x20, x0
	mov	x19, x1
	mov	x0, x2
	mov	w1, 0
	mov	x2, 2048
	stp	w3, w4, [x29, 36]
	str	w5, [x29, 44]
	bl	_memset
	subs	x2, x19, x20
	beq	L18
	ldp	w3, w4, [x29, 36]
	mov	x7, x0
	asr	x2, x2, 2
	mov	x1, 0
	ldr	w5, [x29, 44]
	.p2align 5,,15
L20:
	ldr	w0, [x20, x1, lsl 2]
	add	x1, x1, 1
	sub	w0, w5, w0
	lsr	w0, w0, w3
	and	w0, w0, 255
	eor	w0, w0, w4
	ldr	x6, [x7, x0, lsl 3]
	add	x6, x6, 1
	str	x6, [x7, x0, lsl 3]
	cmp	x1, x2
	bcc	L20
L18:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
LCFI13:
	ret
LFE6349:
	.align	6
__Z5countIiLb0EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0:
LFB6350:
	stp	x29, x30, [sp, -48]!
LCFI14:
	mov	x29, sp
LCFI15:
	stp	x19, x20, [sp, 16]
LCFI16:
	mov	x20, x0
	mov	x19, x1
	mov	x0, x2
	mov	w1, 0
	mov	x2, 2048
	stp	w3, w4, [x29, 36]
	str	w5, [x29, 44]
	bl	_memset
	subs	x2, x19, x20
	beq	L26
	ldp	w3, w4, [x29, 36]
	mov	x7, x0
	asr	x2, x2, 2
	mov	x1, 0
	ldr	w5, [x29, 44]
	.p2align 5,,15
L28:
	ldr	w0, [x20, x1, lsl 2]
	add	x1, x1, 1
	sub	w0, w0, w5
	lsr	w0, w0, w3
	and	w0, w0, 255
	eor	w0, w0, w4
	ldr	x6, [x7, x0, lsl 3]
	add	x6, x6, 1
	str	x6, [x7, x0, lsl 3]
	cmp	x1, x2
	bcc	L28
L26:
	ldp	x19, x20, [sp, 16]
	ldp	x29, x30, [sp], 48
LCFI17:
	ret
LFE6350:
	.align	2
	.p2align 5,,15
	.globl __ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
	.weak_definition __ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv:
LFB6076:
	ldr	x5, [x0]
	mov	x6, 6633
	mov	x2, x0
	movk	x6, 0xa966, lsl 16
	add	x7, x0, 1248
	movk	x6, 0x6f5a, lsl 32
	mov	x4, x0
	movk	x6, 0xb502, lsl 48
	.p2align 5,,15
L35:
	and	x3, x5, -2147483648
	ldr	x5, [x4, 8]
	and	x1, x5, 2147483647
	orr	x1, x1, x3
	ldr	x3, [x4, 1248]
	eor	x3, x3, x1, lsr 1
	sbfx	x1, x1, 0, 1
	and	x1, x1, x6
	eor	x1, x3, x1
	str	x1, [x4], 8
	cmp	x4, x7
	bne	L35
	ldr	x4, [x0, 1248]
	mov	x5, 6633
	add	x6, x0, 1240
	movk	x5, 0xa966, lsl 16
	movk	x5, 0x6f5a, lsl 32
	movk	x5, 0xb502, lsl 48
	.p2align 5,,15
L36:
	and	x3, x4, -2147483648
	ldr	x4, [x2, 1256]
	add	x2, x2, 8
	and	x1, x4, 2147483647
	orr	x1, x1, x3
	ldr	x3, [x2, -8]
	eor	x3, x3, x1, lsr 1
	sbfx	x1, x1, 0, 1
	and	x1, x1, x5
	eor	x1, x3, x1
	str	x1, [x2, 1240]
	cmp	x6, x2
	bne	L36
	ldr	x2, [x0]
	str	xzr, [x0, 2496]
	ldr	x1, [x0, 2488]
	bfi	x1, x2, 0, 31
	ldr	x2, [x0, 1240]
	eor	x2, x2, x1, lsr 1
	sbfx	x1, x1, 0, 1
	and	x1, x1, x5
	eor	x1, x2, x1
	str	x1, [x0, 2488]
	ret
LFE6076:
	.align	2
	.p2align 5,,15
	.globl __ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
	.weak_definition __ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv:
LFB5908:
	stp	x29, x30, [sp, -32]!
LCFI18:
	mov	x29, sp
LCFI19:
	mov	x1, x0
	ldr	x2, [x0, 2496]
	cmp	x2, 311
	bhi	L42
L40:
	add	x0, x2, 1
	str	x0, [x1, 2496]
	ldr	x1, [x1, x2, lsl 3]
	ldp	x29, x30, [sp], 32
LCFI20:
	lsr	x0, x1, 29
	and	x0, x0, 6148914691236517205
	eor	x0, x0, x1
	mov	x1, 3987079168
	movk	x1, 0x7fff, lsl 32
	movk	x1, 0x71d6, lsl 48
	and	x1, x1, x0, lsl 17
	eor	x1, x1, x0
	mov	x0, 262645840084992
	movk	x0, 0xfff7, lsl 48
	and	x0, x0, x1, lsl 37
	eor	x0, x0, x1
	eor	x0, x0, x0, lsr 43
	ret
	.p2align 2,,3
L42:
LCFI21:
	str	x0, [x29, 24]
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
	ldr	x1, [x29, 24]
	ldr	x2, [x1, 2496]
	b	L40
LFE5908:
	.align	2
	.p2align 5,,15
	.globl __ZSt7shuffleIPjRSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEEvT_S4_OT0_
	.weak_definition __ZSt7shuffleIPjRSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEEvT_S4_OT0_
__ZSt7shuffleIPjRSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEEvT_S4_OT0_:
LFB5911:
	cmp	x0, x1
	beq	L69
	stp	x29, x30, [sp, -80]!
LCFI22:
	mov	x29, sp
LCFI23:
	stp	x23, x24, [sp, 48]
LCFI24:
	mov	x23, x0
	sub	x0, x1, x0
	asr	x0, x0, 2
	add	x24, x23, 4
	stp	x21, x22, [sp, 32]
LCFI25:
	mov	x21, x1
	umulh	x1, x0, x0
	mov	x22, x2
	cbz	x1, L74
	cmp	x21, x24
	beq	L43
	stp	x19, x20, [x29, 16]
LCFI26:
	mov	x20, 3987079168
	mov	x19, 262645840084992
	movk	x20, 0x7fff, lsl 32
	movk	x19, 0xfff7, lsl 48
	movk	x20, 0x71d6, lsl 48
	stp	x25, x26, [x29, 64]
LCFI27:
	.p2align 5,,15
L60:
	sub	x26, x24, x23
	asr	x26, x26, 2
	cmn	x26, #1
	beq	L55
	add	x25, x26, 1
	mov	x0, x22
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
	mul	x1, x0, x25
	umulh	x3, x0, x25
	cmp	x25, x1
	bls	L59
	mvn	x26, x26
	udiv	x0, x26, x25
	msub	x26, x0, x25, x26
	cmp	x1, x26
	bcs	L59
	ldr	x0, [x22, 2496]
	b	L58
	.p2align 2,,3
L57:
	ldr	x3, [x22, x1, lsl 3]
	add	x0, x1, 1
	str	x0, [x22, 2496]
	lsr	x1, x3, 29
	and	x1, x1, 6148914691236517205
	eor	x3, x3, x1
	and	x1, x20, x3, lsl 17
	eor	x3, x3, x1
	and	x1, x19, x3, lsl 37
	eor	x3, x3, x1
	eor	x3, x3, x3, lsr 43
	mul	x1, x3, x25
	umulh	x3, x3, x25
	cmp	x26, x1
	bls	L59
L58:
	mov	x1, x0
	cmp	x0, 311
	bls	L57
	mov	x0, x22
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
	ldr	x1, [x22, 2496]
	b	L57
	.p2align 2,,3
L55:
	mov	x0, x22
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
	mov	x3, x0
L59:
	lsl	x3, x3, 2
	ldr	w0, [x24]
	ldr	w1, [x23, x3]
	str	w1, [x24], 4
	str	w0, [x23, x3]
	cmp	x21, x24
	bne	L60
L72:
	ldp	x19, x20, [x29, 16]
LCFI28:
	ldp	x25, x26, [x29, 64]
LCFI29:
L43:
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x29, x30, [sp], 80
LCFI30:
	ret
	.p2align 2,,3
L74:
LCFI31:
	tbz	x0, 0, L75
L48:
	cmp	x21, x24
	beq	L43
	stp	x19, x20, [x29, 16]
LCFI32:
	sub	x20, x24, x23
	mov	x0, x22
	asr	x20, x20, 2
	add	x19, x20, 2
	stp	x25, x26, [x29, 64]
LCFI33:
	madd	x20, x20, x19, x19
	cbz	x20, L50
	.p2align 5,,15
L76:
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
	mul	x1, x0, x20
	umulh	x0, x0, x20
	cmp	x20, x1
	bls	L53
	neg	x2, x20
	udiv	x25, x2, x20
	msub	x25, x25, x20, x2
	cmp	x1, x25
	bcs	L53
	.p2align 5,,15
L52:
	mov	x0, x22
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
	mul	x1, x20, x0
	umulh	x0, x20, x0
	cmp	x25, x1
	bhi	L52
L53:
	udiv	x1, x0, x19
	ldr	w3, [x24]
	lsl	x2, x1, 2
	msub	x0, x1, x19, x0
	ldr	w4, [x23, x2]
	lsl	x0, x0, 2
	str	w4, [x24], 8
	str	w3, [x23, x2]
	ldr	w2, [x23, x0]
	ldr	w1, [x24, -4]
	str	w2, [x24, -4]
	str	w1, [x23, x0]
	cmp	x21, x24
	beq	L72
	sub	x20, x24, x23
	mov	x0, x22
	asr	x20, x20, 2
	add	x19, x20, 2
	madd	x20, x20, x19, x19
	cbnz	x20, L76
L50:
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
	b	L53
L75:
LCFI34:
	mov	x0, x2
	add	x24, x23, 8
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEclEv
	mov	x1, 2
	umulh	x0, x0, x1
	ldr	w1, [x23, 4]
	ubfiz	x0, x0, 2, 32
	ldr	w2, [x23, x0]
	str	w2, [x23, 4]
	str	w1, [x23, x0]
	b	L48
	.p2align 2,,3
L69:
LCFI35:
	ret
LFE5911:
	.cstring
	.align	3
lC0:
	.ascii "histogram mismatch\0"
	.text
	.align	2
	.p2align 5,,15
	.globl __Z7measureIiLb0EEvPKcS1_
	.weak_definition __Z7measureIiLb0EEvPKcS1_
__Z7measureIiLb0EEvPKcS1_:
LFB5686:
	mov	x12, 4800
	mov	x2, 32557
	sub	sp, sp, x12
LCFI36:
	movk	x2, 0x4c95, lsl 16
	movk	x2, 0xf42d, lsl 32
	stp	x29, x30, [sp]
LCFI37:
	mov	x29, sp
LCFI38:
	movk	x2, 0x5851, lsl 48
	str	x0, [x29, 160]
	mov	x0, 50759
	movk	x0, 0x6, lsl 16
	stp	x19, x20, [sp, 16]
LCFI39:
	mov	x19, 1
	str	x1, [x29, 184]
	add	x1, x29, 2304
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	stp	x27, x28, [sp, 80]
LCFI40:
	str	x0, [x29, 2296]
L78:
	eor	x0, x0, x0, lsr 62
	madd	x0, x0, x2, x19
	add	x19, x19, 1
	str	x0, [x1], 8
	cmp	x19, 312
	bne	L78
	mov	x0, 2304
	str	x19, [x29, 4792]
	add	x23, x29, 2296
	movk	x0, 0x3d, lsl 16
	mov	x22, 3987079168
LEHB0:
	bl	__Znwm
LEHE0:
	add	x1, x0, 3997696
	mov	x20, x0
	str	wzr, [x0], 4
	add	x1, x1, 2304
	mov	x2, 2300
	movk	x22, 0x7fff, lsl 32
	mov	x21, 262645840084992
	movk	x2, 0x3d, lsl 16
	stp	x1, x20, [x29, 168]
	mov	w1, 0
	movk	x22, 0x71d6, lsl 48
	movk	x21, 0xfff7, lsl 48
	bl	_memset
	str	x23, [x29, 104]
	b	L80
L79:
	add	x19, x0, 1
	ldr	x0, [x23, x0, lsl 3]
	str	x19, [x29, 4792]
	lsr	x1, x0, 29
	and	x1, x1, 6148914691236517205
	eor	x0, x0, x1
	and	x1, x22, x0, lsl 17
	eor	x0, x0, x1
	and	x1, x21, x0, lsl 37
	eor	x0, x0, x1
	eor	x0, x0, x0, lsr 43
	ubfiz	w0, w0, 16, 8
	str	w0, [x20], 4
	ldr	x0, [x29, 168]
	cmp	x0, x20
	beq	L119
L80:
	mov	x0, x19
	cmp	x19, 311
	bls	L79
	mov	x0, x23
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
	ldr	x0, [x29, 4792]
	b	L79
L119:
	add	x23, x29, 240
	mov	x2, 2048
	adrp	x21, __ZSt4cout@GOTPAGE
	ldr	x21, [x21, __ZSt4cout@GOTPAGEOFF]
	mov	w1, 0
	mov	x0, x23
	bl	_memset
	adrp	x0, lC1@PAGE
	add	x27, x29, 2288
	ldr	q31, [x0, #lC1@PAGEOFF]
	add	x0, x29, 224
	str	x0, [x29, 112]
	add	x0, x29, 223
	stp	x0, xzr, [x29, 128]
	add	x0, x29, 222
	str	x0, [x29, 120]
	str	q31, [x29, 224]
L105:
	ldr	x1, [x29, 136]
	adrp	x0, _C.2.0@PAGE
	mov	w25, -1
	add	x0, x0, _C.2.0@PAGEOFF;
	ldr	w24, [x0, x1]
L104:
	ldp	x2, x19, [x29, 104]
	mov	x26, 16960
	movk	x26, 0xf, lsl 16
	add	x28, x19, 16
	mov	x0, x19
	mov	x1, x28
	bl	__ZSt7shuffleIPjRSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEEvT_S4_OT0_
	add	x0, x29, 221
	str	x0, [x29, 144]
L103:
	ldr	w20, [x19]
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 200]
	mov	w5, 0
	mov	w4, w20
	ldp	x1, x0, [x29, 168]
	mov	w3, w24
	mov	x2, x23
	bl	__Z5countIiLb0EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 192]
	mov	x1, 0
	mov	x0, x23
	.p2align 5,,15
L81:
	ldr	x2, [x0], 8
	add	x1, x1, x2
	cmp	x0, x27
	bne	L81
	cmp	x1, x26
	bne	L82
	cmp	w24, 15
	bgt	L83
	ldr	x0, [x23, w20, uxtw 3]
	cmp	x0, x26
	bne	L82
L83:
	cmn	w25, #1
	beq	L86
	ldr	x0, [x29, 160]
	cbz	x0, L120
	ldr	x22, [x29, 160]
	mov	x0, x22
	bl	_strlen
	mov	x2, x0
	mov	x1, x22
	mov	x0, x21
LEHB1:
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L88:
	mov	w0, 44
	strb	w0, [x29, 218]
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	x0, [x0, 16]
	cbz	x0, L89
	mov	x2, 1
	add	x1, x29, 218
	mov	x0, x21
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
	ldr	x0, [x29, 184]
	cbz	x0, L121
L91:
	ldr	x0, [x29, 184]
	bl	_strlen
	ldr	x1, [x29, 184]
	mov	x2, x0
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L92:
	mov	w0, 44
	strb	w0, [x29, 219]
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	x0, [x0, 16]
	cbz	x0, L93
	mov	x2, 1
	add	x1, x29, 219
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
L94:
	mov	w1, w24
	mov	x0, x22
	bl	__ZNSolsEi
	mov	w1, 44
	strb	w1, [x29, 220]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L95
	mov	x2, 1
	add	x1, x29, 220
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x2, x0
L96:
	uxtw	x1, w20
	mov	x0, x2
	bl	__ZNSo9_M_insertImEERSoT_
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 221]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L97
	ldr	x1, [x29, 144]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L98:
	mov	w1, w25
	mov	x0, x20
	bl	__ZNSolsEi
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 222]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L99
	ldr	x1, [x29, 120]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L100:
	ldp	x0, x1, [x29, 192]
	sub	x22, x0, x1
	mov	x0, x20
	scvtf	d0, x22
	bl	__ZNSo9_M_insertIdEERSoT_
	mov	w1, 10
	strb	w1, [x29, 223]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L101
	ldr	x1, [x29, 128]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
LEHE1:
L86:
	add	x19, x19, 4
	cmp	x28, x19
	bne	L103
	add	w25, w25, 1
	cmp	w25, 11
	bne	L104
	ldr	x0, [x29, 136]
	add	x0, x0, 4
	str	x0, [x29, 136]
	cmp	x0, 12
	bne	L105
	mov	x12, 4800
	mov	x1, 2304
	ldr	x0, [x29, 176]
	ldp	x19, x20, [sp, 16]
	movk	x1, 0x3d, lsl 16
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x27, x28, [sp, 80]
	ldp	x29, x30, [sp]
	add	sp, sp, x12
LCFI41:
LEHB2:
	b	__ZdlPvm
LEHE2:
L101:
LCFI42:
	mov	w1, 10
LEHB3:
	bl	__ZNSo3putEc
	b	L86
L99:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L100
L97:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L98
L95:
	mov	w1, 44
	str	x0, [x29, 152]
	bl	__ZNSo3putEc
	ldr	x2, [x29, 152]
	b	L96
L93:
	mov	w1, 44
	mov	x0, x22
	bl	__ZNSo3putEc
	b	L94
L89:
	mov	w1, 44
	mov	x0, x21
	bl	__ZNSo3putEc
	ldr	x0, [x29, 184]
	mov	x22, x21
	cbnz	x0, L91
L121:
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
	b	L92
L120:
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
LEHE3:
	b	L88
L82:
	mov	x0, 16
	bl	___cxa_allocate_exception
	adrp	x1, lC0@PAGE
	mov	x20, x0
	add	x1, x1, lC0@PAGEOFF;
LEHB4:
	bl	__ZNSt13runtime_errorC1EPKc
LEHE4:
	adrp	x2, __ZNSt13runtime_errorD1Ev@GOTPAGE
	ldr	x2, [x2, __ZNSt13runtime_errorD1Ev@GOTPAGEOFF]
	mov	x0, x20
	adrp	x1, __ZTISt13runtime_error@GOTPAGE
	ldr	x1, [x1, __ZTISt13runtime_error@GOTPAGEOFF]
LEHB5:
	bl	___cxa_throw
LEHE5:
L109:
	mov	x19, x0
	b	L107
L110:
	mov	x19, x0
	mov	x0, x20
	bl	___cxa_free_exception
L107:
	ldr	x0, [x29, 176]
	mov	x1, 2304
	movk	x1, 0x3d, lsl 16
	bl	__ZdlPvm
	mov	x0, x19
LEHB6:
	bl	__Unwind_Resume
LEHE6:
LFE5686:
	.section __TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table0:
LLSDA5686:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 LLSDACSE5686-LLSDACSB5686
LLSDACSB5686:
	.uleb128 LEHB0-LFB5686
	.uleb128 LEHE0-LEHB0
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB1-LFB5686
	.uleb128 LEHE1-LEHB1
	.uleb128 L109-LFB5686
	.uleb128 0
	.uleb128 LEHB2-LFB5686
	.uleb128 LEHE2-LEHB2
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB3-LFB5686
	.uleb128 LEHE3-LEHB3
	.uleb128 L109-LFB5686
	.uleb128 0
	.uleb128 LEHB4-LFB5686
	.uleb128 LEHE4-LEHB4
	.uleb128 L110-LFB5686
	.uleb128 0
	.uleb128 LEHB5-LFB5686
	.uleb128 LEHE5-LEHB5
	.uleb128 L109-LFB5686
	.uleb128 0
	.uleb128 LEHB6-LFB5686
	.uleb128 LEHE6-LEHB6
	.uleb128 0
	.uleb128 0
LLSDACSE5686:
	.text
	.align	2
	.p2align 5,,15
	.globl __Z7measureIiLb1EEvPKcS1_
	.weak_definition __Z7measureIiLb1EEvPKcS1_
__Z7measureIiLb1EEvPKcS1_:
LFB5697:
	mov	x12, 4800
	mov	x2, 32557
	sub	sp, sp, x12
LCFI43:
	movk	x2, 0x4c95, lsl 16
	movk	x2, 0xf42d, lsl 32
	stp	x29, x30, [sp]
LCFI44:
	mov	x29, sp
LCFI45:
	movk	x2, 0x5851, lsl 48
	str	x0, [x29, 160]
	mov	x0, 50759
	movk	x0, 0x6, lsl 16
	stp	x19, x20, [sp, 16]
LCFI46:
	mov	x19, 1
	str	x1, [x29, 184]
	add	x1, x29, 2304
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	stp	x27, x28, [sp, 80]
LCFI47:
	str	x0, [x29, 2296]
L123:
	eor	x0, x0, x0, lsr 62
	madd	x0, x0, x2, x19
	add	x19, x19, 1
	str	x0, [x1], 8
	cmp	x19, 312
	bne	L123
	mov	x0, 2304
	str	x19, [x29, 4792]
	add	x23, x29, 2296
	movk	x0, 0x3d, lsl 16
	mov	x22, 3987079168
LEHB7:
	bl	__Znwm
LEHE7:
	add	x1, x0, 3997696
	mov	x20, x0
	str	wzr, [x0], 4
	add	x1, x1, 2304
	mov	x2, 2300
	movk	x22, 0x7fff, lsl 32
	mov	x21, 262645840084992
	movk	x2, 0x3d, lsl 16
	stp	x1, x20, [x29, 168]
	mov	w1, 0
	movk	x22, 0x71d6, lsl 48
	movk	x21, 0xfff7, lsl 48
	bl	_memset
	str	x23, [x29, 104]
	b	L125
L124:
	add	x19, x0, 1
	ldr	x0, [x23, x0, lsl 3]
	str	x19, [x29, 4792]
	lsr	x1, x0, 29
	and	x1, x1, 6148914691236517205
	eor	x0, x0, x1
	and	x1, x22, x0, lsl 17
	eor	x0, x0, x1
	and	x1, x21, x0, lsl 37
	eor	x0, x0, x1
	eor	x0, x0, x0, lsr 43
	ubfiz	w0, w0, 16, 8
	str	w0, [x20], 4
	ldr	x0, [x29, 168]
	cmp	x0, x20
	beq	L164
L125:
	mov	x0, x19
	cmp	x19, 311
	bls	L124
	mov	x0, x23
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
	ldr	x0, [x29, 4792]
	b	L124
L164:
	add	x23, x29, 240
	mov	x2, 2048
	adrp	x21, __ZSt4cout@GOTPAGE
	ldr	x21, [x21, __ZSt4cout@GOTPAGEOFF]
	mov	w1, 0
	mov	x0, x23
	bl	_memset
	adrp	x0, lC1@PAGE
	add	x27, x29, 2288
	ldr	q31, [x0, #lC1@PAGEOFF]
	add	x0, x29, 224
	str	x0, [x29, 112]
	add	x0, x29, 223
	stp	x0, xzr, [x29, 128]
	add	x0, x29, 222
	str	x0, [x29, 120]
	str	q31, [x29, 224]
L150:
	ldr	x1, [x29, 136]
	adrp	x0, _C.21.1@PAGE
	mov	w25, -1
	add	x0, x0, _C.21.1@PAGEOFF;
	ldr	w24, [x0, x1]
L149:
	ldp	x2, x19, [x29, 104]
	mov	x26, 16960
	movk	x26, 0xf, lsl 16
	add	x28, x19, 16
	mov	x0, x19
	mov	x1, x28
	bl	__ZSt7shuffleIPjRSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEEvT_S4_OT0_
	add	x0, x29, 221
	str	x0, [x29, 144]
L148:
	ldr	w20, [x19]
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 200]
	mov	w5, 16711680
	mov	w4, w20
	ldp	x1, x0, [x29, 168]
	mov	w3, w24
	mov	x2, x23
	bl	__Z5countIiLb1EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 192]
	mov	x1, 0
	mov	x0, x23
	.p2align 5,,15
L126:
	ldr	x2, [x0], 8
	add	x1, x1, x2
	cmp	x0, x27
	bne	L126
	cmp	x1, x26
	bne	L127
	cmp	w24, 15
	bgt	L128
	ldr	x0, [x23, w20, uxtw 3]
	cmp	x0, x26
	bne	L127
L128:
	cmn	w25, #1
	beq	L131
	ldr	x0, [x29, 160]
	cbz	x0, L165
	ldr	x22, [x29, 160]
	mov	x0, x22
	bl	_strlen
	mov	x2, x0
	mov	x1, x22
	mov	x0, x21
LEHB8:
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L133:
	mov	w0, 44
	strb	w0, [x29, 218]
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	x0, [x0, 16]
	cbz	x0, L134
	mov	x2, 1
	add	x1, x29, 218
	mov	x0, x21
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
	ldr	x0, [x29, 184]
	cbz	x0, L166
L136:
	ldr	x0, [x29, 184]
	bl	_strlen
	ldr	x1, [x29, 184]
	mov	x2, x0
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L137:
	mov	w0, 44
	strb	w0, [x29, 219]
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	x0, [x0, 16]
	cbz	x0, L138
	mov	x2, 1
	add	x1, x29, 219
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
L139:
	mov	w1, w24
	mov	x0, x22
	bl	__ZNSolsEi
	mov	w1, 44
	strb	w1, [x29, 220]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L140
	mov	x2, 1
	add	x1, x29, 220
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x2, x0
L141:
	uxtw	x1, w20
	mov	x0, x2
	bl	__ZNSo9_M_insertImEERSoT_
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 221]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L142
	ldr	x1, [x29, 144]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L143:
	mov	w1, w25
	mov	x0, x20
	bl	__ZNSolsEi
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 222]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L144
	ldr	x1, [x29, 120]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L145:
	ldp	x0, x1, [x29, 192]
	sub	x22, x0, x1
	mov	x0, x20
	scvtf	d0, x22
	bl	__ZNSo9_M_insertIdEERSoT_
	mov	w1, 10
	strb	w1, [x29, 223]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L146
	ldr	x1, [x29, 128]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
LEHE8:
L131:
	add	x19, x19, 4
	cmp	x28, x19
	bne	L148
	add	w25, w25, 1
	cmp	w25, 11
	bne	L149
	ldr	x0, [x29, 136]
	add	x0, x0, 4
	str	x0, [x29, 136]
	cmp	x0, 12
	bne	L150
	mov	x12, 4800
	mov	x1, 2304
	ldr	x0, [x29, 176]
	ldp	x19, x20, [sp, 16]
	movk	x1, 0x3d, lsl 16
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x27, x28, [sp, 80]
	ldp	x29, x30, [sp]
	add	sp, sp, x12
LCFI48:
LEHB9:
	b	__ZdlPvm
LEHE9:
L146:
LCFI49:
	mov	w1, 10
LEHB10:
	bl	__ZNSo3putEc
	b	L131
L144:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L145
L142:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L143
L140:
	mov	w1, 44
	str	x0, [x29, 152]
	bl	__ZNSo3putEc
	ldr	x2, [x29, 152]
	b	L141
L138:
	mov	w1, 44
	mov	x0, x22
	bl	__ZNSo3putEc
	b	L139
L134:
	mov	w1, 44
	mov	x0, x21
	bl	__ZNSo3putEc
	ldr	x0, [x29, 184]
	mov	x22, x21
	cbnz	x0, L136
L166:
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
	b	L137
L165:
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
LEHE10:
	b	L133
L127:
	mov	x0, 16
	bl	___cxa_allocate_exception
	adrp	x1, lC0@PAGE
	mov	x20, x0
	add	x1, x1, lC0@PAGEOFF;
LEHB11:
	bl	__ZNSt13runtime_errorC1EPKc
LEHE11:
	adrp	x2, __ZNSt13runtime_errorD1Ev@GOTPAGE
	ldr	x2, [x2, __ZNSt13runtime_errorD1Ev@GOTPAGEOFF]
	mov	x0, x20
	adrp	x1, __ZTISt13runtime_error@GOTPAGE
	ldr	x1, [x1, __ZTISt13runtime_error@GOTPAGEOFF]
LEHB12:
	bl	___cxa_throw
LEHE12:
L154:
	mov	x19, x0
	b	L152
L155:
	mov	x19, x0
	mov	x0, x20
	bl	___cxa_free_exception
L152:
	ldr	x0, [x29, 176]
	mov	x1, 2304
	movk	x1, 0x3d, lsl 16
	bl	__ZdlPvm
	mov	x0, x19
LEHB13:
	bl	__Unwind_Resume
LEHE13:
LFE5697:
	.section __TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table1:
LLSDA5697:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 LLSDACSE5697-LLSDACSB5697
LLSDACSB5697:
	.uleb128 LEHB7-LFB5697
	.uleb128 LEHE7-LEHB7
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB8-LFB5697
	.uleb128 LEHE8-LEHB8
	.uleb128 L154-LFB5697
	.uleb128 0
	.uleb128 LEHB9-LFB5697
	.uleb128 LEHE9-LEHB9
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB10-LFB5697
	.uleb128 LEHE10-LEHB10
	.uleb128 L154-LFB5697
	.uleb128 0
	.uleb128 LEHB11-LFB5697
	.uleb128 LEHE11-LEHB11
	.uleb128 L155-LFB5697
	.uleb128 0
	.uleb128 LEHB12-LFB5697
	.uleb128 LEHE12-LEHB12
	.uleb128 L154-LFB5697
	.uleb128 0
	.uleb128 LEHB13-LFB5697
	.uleb128 LEHE13-LEHB13
	.uleb128 0
	.uleb128 0
LLSDACSE5697:
	.text
	.align	2
	.p2align 5,,15
	.globl __Z7measureIxLb0EEvPKcS1_
	.weak_definition __Z7measureIxLb0EEvPKcS1_
__Z7measureIxLb0EEvPKcS1_:
LFB5698:
	mov	x12, 4800
	mov	x2, 32557
	sub	sp, sp, x12
LCFI50:
	movk	x2, 0x4c95, lsl 16
	movk	x2, 0xf42d, lsl 32
	stp	x29, x30, [sp]
LCFI51:
	mov	x29, sp
LCFI52:
	movk	x2, 0x5851, lsl 48
	str	x0, [x29, 160]
	mov	x0, 50763
	movk	x0, 0x6, lsl 16
	stp	x19, x20, [sp, 16]
LCFI53:
	mov	x19, 1
	str	x1, [x29, 184]
	add	x1, x29, 2304
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	stp	x27, x28, [sp, 80]
LCFI54:
	str	x0, [x29, 2296]
L168:
	eor	x0, x0, x0, lsr 62
	madd	x0, x0, x2, x19
	add	x19, x19, 1
	str	x0, [x1], 8
	cmp	x19, 312
	bne	L168
	mov	x0, 4608
	str	x19, [x29, 4792]
	add	x23, x29, 2296
	movk	x0, 0x7a, lsl 16
	mov	x22, 3987079168
LEHB14:
	bl	__Znwm
LEHE14:
	add	x1, x0, 7999488
	mov	x20, x0
	str	xzr, [x0], 8
	add	x1, x1, 512
	mov	x2, 4600
	movk	x22, 0x7fff, lsl 32
	mov	x21, 262645840084992
	movk	x2, 0x7a, lsl 16
	stp	x1, x20, [x29, 168]
	mov	w1, 0
	movk	x22, 0x71d6, lsl 48
	movk	x21, 0xfff7, lsl 48
	bl	_memset
	str	x23, [x29, 104]
	b	L170
L169:
	add	x19, x0, 1
	ldr	x0, [x23, x0, lsl 3]
	str	x19, [x29, 4792]
	lsr	x1, x0, 29
	and	x1, x1, 6148914691236517205
	eor	x0, x0, x1
	and	x1, x22, x0, lsl 17
	eor	x0, x0, x1
	and	x1, x21, x0, lsl 37
	eor	x0, x0, x1
	eor	x0, x0, x0, lsr 43
	ubfiz	x0, x0, 16, 8
	str	x0, [x20], 8
	ldr	x0, [x29, 168]
	cmp	x0, x20
	beq	L209
L170:
	mov	x0, x19
	cmp	x19, 311
	bls	L169
	mov	x0, x23
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
	ldr	x0, [x29, 4792]
	b	L169
L209:
	add	x23, x29, 240
	mov	x2, 2048
	adrp	x21, __ZSt4cout@GOTPAGE
	ldr	x21, [x21, __ZSt4cout@GOTPAGEOFF]
	mov	w1, 0
	mov	x0, x23
	bl	_memset
	adrp	x0, lC1@PAGE
	add	x27, x29, 2288
	ldr	q31, [x0, #lC1@PAGEOFF]
	add	x0, x29, 224
	str	x0, [x29, 112]
	add	x0, x29, 223
	stp	x0, xzr, [x29, 128]
	add	x0, x29, 222
	str	x0, [x29, 120]
	str	q31, [x29, 224]
L195:
	ldr	x1, [x29, 136]
	adrp	x0, _C.27.2@PAGE
	mov	w25, -1
	add	x0, x0, _C.27.2@PAGEOFF;
	ldr	w24, [x0, x1]
L194:
	ldp	x2, x19, [x29, 104]
	mov	x26, 16960
	movk	x26, 0xf, lsl 16
	add	x28, x19, 16
	mov	x0, x19
	mov	x1, x28
	bl	__ZSt7shuffleIPjRSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEEvT_S4_OT0_
	add	x0, x29, 221
	str	x0, [x29, 144]
L193:
	ldr	w20, [x19]
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 200]
	mov	x5, 0
	mov	w4, w20
	ldp	x1, x0, [x29, 168]
	mov	w3, w24
	mov	x2, x23
	bl	__Z5countIxLb0EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 192]
	mov	x1, 0
	mov	x0, x23
	.p2align 5,,15
L171:
	ldr	x2, [x0], 8
	add	x1, x1, x2
	cmp	x27, x0
	bne	L171
	cmp	x1, x26
	bne	L172
	cmp	w24, 15
	bgt	L173
	ldr	x0, [x23, w20, uxtw 3]
	cmp	x0, x26
	bne	L172
L173:
	cmn	w25, #1
	beq	L176
	ldr	x0, [x29, 160]
	cbz	x0, L210
	ldr	x22, [x29, 160]
	mov	x0, x22
	bl	_strlen
	mov	x2, x0
	mov	x1, x22
	mov	x0, x21
LEHB15:
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L178:
	mov	w0, 44
	strb	w0, [x29, 218]
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	x0, [x0, 16]
	cbz	x0, L179
	mov	x2, 1
	add	x1, x29, 218
	mov	x0, x21
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
	ldr	x0, [x29, 184]
	cbz	x0, L211
L181:
	ldr	x0, [x29, 184]
	bl	_strlen
	ldr	x1, [x29, 184]
	mov	x2, x0
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L182:
	mov	w0, 44
	strb	w0, [x29, 219]
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	x0, [x0, 16]
	cbz	x0, L183
	mov	x2, 1
	add	x1, x29, 219
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
L184:
	mov	w1, w24
	mov	x0, x22
	bl	__ZNSolsEi
	mov	w1, 44
	strb	w1, [x29, 220]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L185
	mov	x2, 1
	add	x1, x29, 220
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x2, x0
L186:
	uxtw	x1, w20
	mov	x0, x2
	bl	__ZNSo9_M_insertImEERSoT_
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 221]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L187
	ldr	x1, [x29, 144]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L188:
	mov	w1, w25
	mov	x0, x20
	bl	__ZNSolsEi
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 222]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L189
	ldr	x1, [x29, 120]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L190:
	ldp	x0, x1, [x29, 192]
	sub	x22, x0, x1
	mov	x0, x20
	scvtf	d0, x22
	bl	__ZNSo9_M_insertIdEERSoT_
	mov	w1, 10
	strb	w1, [x29, 223]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L191
	ldr	x1, [x29, 128]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
LEHE15:
L176:
	add	x19, x19, 4
	cmp	x28, x19
	bne	L193
	add	w25, w25, 1
	cmp	w25, 11
	bne	L194
	ldr	x0, [x29, 136]
	add	x0, x0, 4
	str	x0, [x29, 136]
	cmp	x0, 12
	bne	L195
	mov	x12, 4800
	mov	x1, 4608
	ldr	x0, [x29, 176]
	ldp	x19, x20, [sp, 16]
	movk	x1, 0x7a, lsl 16
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x27, x28, [sp, 80]
	ldp	x29, x30, [sp]
	add	sp, sp, x12
LCFI55:
LEHB16:
	b	__ZdlPvm
LEHE16:
L191:
LCFI56:
	mov	w1, 10
LEHB17:
	bl	__ZNSo3putEc
	b	L176
L189:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L190
L187:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L188
L185:
	mov	w1, 44
	str	x0, [x29, 152]
	bl	__ZNSo3putEc
	ldr	x2, [x29, 152]
	b	L186
L183:
	mov	w1, 44
	mov	x0, x22
	bl	__ZNSo3putEc
	b	L184
L179:
	mov	w1, 44
	mov	x0, x21
	bl	__ZNSo3putEc
	ldr	x0, [x29, 184]
	mov	x22, x21
	cbnz	x0, L181
L211:
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
	b	L182
L210:
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
LEHE17:
	b	L178
L172:
	mov	x0, 16
	bl	___cxa_allocate_exception
	adrp	x1, lC0@PAGE
	mov	x20, x0
	add	x1, x1, lC0@PAGEOFF;
LEHB18:
	bl	__ZNSt13runtime_errorC1EPKc
LEHE18:
	adrp	x2, __ZNSt13runtime_errorD1Ev@GOTPAGE
	ldr	x2, [x2, __ZNSt13runtime_errorD1Ev@GOTPAGEOFF]
	mov	x0, x20
	adrp	x1, __ZTISt13runtime_error@GOTPAGE
	ldr	x1, [x1, __ZTISt13runtime_error@GOTPAGEOFF]
LEHB19:
	bl	___cxa_throw
LEHE19:
L199:
	mov	x19, x0
	b	L197
L200:
	mov	x19, x0
	mov	x0, x20
	bl	___cxa_free_exception
L197:
	ldr	x0, [x29, 176]
	mov	x1, 4608
	movk	x1, 0x7a, lsl 16
	bl	__ZdlPvm
	mov	x0, x19
LEHB20:
	bl	__Unwind_Resume
LEHE20:
LFE5698:
	.section __TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table2:
LLSDA5698:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 LLSDACSE5698-LLSDACSB5698
LLSDACSB5698:
	.uleb128 LEHB14-LFB5698
	.uleb128 LEHE14-LEHB14
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB15-LFB5698
	.uleb128 LEHE15-LEHB15
	.uleb128 L199-LFB5698
	.uleb128 0
	.uleb128 LEHB16-LFB5698
	.uleb128 LEHE16-LEHB16
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB17-LFB5698
	.uleb128 LEHE17-LEHB17
	.uleb128 L199-LFB5698
	.uleb128 0
	.uleb128 LEHB18-LFB5698
	.uleb128 LEHE18-LEHB18
	.uleb128 L200-LFB5698
	.uleb128 0
	.uleb128 LEHB19-LFB5698
	.uleb128 LEHE19-LEHB19
	.uleb128 L199-LFB5698
	.uleb128 0
	.uleb128 LEHB20-LFB5698
	.uleb128 LEHE20-LEHB20
	.uleb128 0
	.uleb128 0
LLSDACSE5698:
	.text
	.align	2
	.p2align 5,,15
	.globl __Z7measureIxLb1EEvPKcS1_
	.weak_definition __Z7measureIxLb1EEvPKcS1_
__Z7measureIxLb1EEvPKcS1_:
LFB5703:
	mov	x12, 4800
	mov	x2, 32557
	sub	sp, sp, x12
LCFI57:
	movk	x2, 0x4c95, lsl 16
	movk	x2, 0xf42d, lsl 32
	stp	x29, x30, [sp]
LCFI58:
	mov	x29, sp
LCFI59:
	movk	x2, 0x5851, lsl 48
	str	x0, [x29, 160]
	mov	x0, 50763
	movk	x0, 0x6, lsl 16
	stp	x19, x20, [sp, 16]
LCFI60:
	mov	x19, 1
	str	x1, [x29, 184]
	add	x1, x29, 2304
	stp	x21, x22, [sp, 32]
	stp	x23, x24, [sp, 48]
	stp	x25, x26, [sp, 64]
	stp	x27, x28, [sp, 80]
LCFI61:
	str	x0, [x29, 2296]
L213:
	eor	x0, x0, x0, lsr 62
	madd	x0, x0, x2, x19
	add	x19, x19, 1
	str	x0, [x1], 8
	cmp	x19, 312
	bne	L213
	mov	x0, 4608
	str	x19, [x29, 4792]
	add	x23, x29, 2296
	movk	x0, 0x7a, lsl 16
	mov	x22, 3987079168
LEHB21:
	bl	__Znwm
LEHE21:
	add	x1, x0, 7999488
	mov	x20, x0
	str	xzr, [x0], 8
	add	x1, x1, 512
	mov	x2, 4600
	movk	x22, 0x7fff, lsl 32
	mov	x21, 262645840084992
	movk	x2, 0x7a, lsl 16
	stp	x1, x20, [x29, 168]
	mov	w1, 0
	movk	x22, 0x71d6, lsl 48
	movk	x21, 0xfff7, lsl 48
	bl	_memset
	str	x23, [x29, 104]
	b	L215
L214:
	add	x19, x0, 1
	ldr	x0, [x23, x0, lsl 3]
	str	x19, [x29, 4792]
	lsr	x1, x0, 29
	and	x1, x1, 6148914691236517205
	eor	x0, x0, x1
	and	x1, x22, x0, lsl 17
	eor	x0, x0, x1
	and	x1, x21, x0, lsl 37
	eor	x0, x0, x1
	eor	x0, x0, x0, lsr 43
	ubfiz	x0, x0, 16, 8
	str	x0, [x20], 8
	ldr	x0, [x29, 168]
	cmp	x0, x20
	beq	L254
L215:
	mov	x0, x19
	cmp	x19, 311
	bls	L214
	mov	x0, x23
	bl	__ZNSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EE11_M_gen_randEv
	ldr	x0, [x29, 4792]
	b	L214
L254:
	add	x23, x29, 240
	mov	x2, 2048
	adrp	x21, __ZSt4cout@GOTPAGE
	ldr	x21, [x21, __ZSt4cout@GOTPAGEOFF]
	mov	w1, 0
	mov	x0, x23
	bl	_memset
	adrp	x0, lC1@PAGE
	add	x27, x29, 2288
	ldr	q31, [x0, #lC1@PAGEOFF]
	add	x0, x29, 224
	str	x0, [x29, 112]
	add	x0, x29, 223
	stp	x0, xzr, [x29, 128]
	add	x0, x29, 222
	str	x0, [x29, 120]
	str	q31, [x29, 224]
L240:
	ldr	x1, [x29, 136]
	adrp	x0, _C.36.3@PAGE
	mov	w25, -1
	add	x0, x0, _C.36.3@PAGEOFF;
	ldr	w24, [x0, x1]
L239:
	ldp	x2, x19, [x29, 104]
	mov	x26, 16960
	movk	x26, 0xf, lsl 16
	add	x28, x19, 16
	mov	x0, x19
	mov	x1, x28
	bl	__ZSt7shuffleIPjRSt23mersenne_twister_engineIyLm64ELm312ELm156ELm31ELy13043109905998158313ELm29ELy6148914691236517205ELm17ELy8202884508482404352ELm37ELy18444473444759240704ELm43ELy6364136223846793005EEEvT_S4_OT0_
	add	x0, x29, 221
	str	x0, [x29, 144]
L238:
	ldr	w20, [x19]
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 200]
	mov	x5, 16711680
	mov	w4, w20
	ldp	x1, x0, [x29, 168]
	mov	w3, w24
	mov	x2, x23
	bl	__Z5countIxLb1EEvRKSt6vectorIT_SaIS1_EERSt5arrayImLm256EEijS1_S1_.isra.0
	bl	__ZNSt6chrono3_V212steady_clock3nowEv
	str	x0, [x29, 192]
	mov	x1, 0
	mov	x0, x23
	.p2align 5,,15
L216:
	ldr	x2, [x0], 8
	add	x1, x1, x2
	cmp	x27, x0
	bne	L216
	cmp	x1, x26
	bne	L217
	cmp	w24, 15
	bgt	L218
	ldr	x0, [x23, w20, uxtw 3]
	cmp	x0, x26
	bne	L217
L218:
	cmn	w25, #1
	beq	L221
	ldr	x0, [x29, 160]
	cbz	x0, L255
	ldr	x22, [x29, 160]
	mov	x0, x22
	bl	_strlen
	mov	x2, x0
	mov	x1, x22
	mov	x0, x21
LEHB22:
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L223:
	mov	w0, 44
	strb	w0, [x29, 218]
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	x0, [x0, 16]
	cbz	x0, L224
	mov	x2, 1
	add	x1, x29, 218
	mov	x0, x21
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
	ldr	x0, [x29, 184]
	cbz	x0, L256
L226:
	ldr	x0, [x29, 184]
	bl	_strlen
	ldr	x1, [x29, 184]
	mov	x2, x0
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
L227:
	mov	w0, 44
	strb	w0, [x29, 219]
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	x0, [x0, 16]
	cbz	x0, L228
	mov	x2, 1
	add	x1, x29, 219
	mov	x0, x22
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x22, x0
L229:
	mov	w1, w24
	mov	x0, x22
	bl	__ZNSolsEi
	mov	w1, 44
	strb	w1, [x29, 220]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L230
	mov	x2, 1
	add	x1, x29, 220
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x2, x0
L231:
	uxtw	x1, w20
	mov	x0, x2
	bl	__ZNSo9_M_insertImEERSoT_
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 221]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L232
	ldr	x1, [x29, 144]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L233:
	mov	w1, w25
	mov	x0, x20
	bl	__ZNSolsEi
	mov	w1, 44
	mov	x20, x0
	strb	w1, [x29, 222]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L234
	ldr	x1, [x29, 120]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
	mov	x20, x0
L235:
	ldp	x0, x1, [x29, 192]
	sub	x22, x0, x1
	mov	x0, x20
	scvtf	d0, x22
	bl	__ZNSo9_M_insertIdEERSoT_
	mov	w1, 10
	strb	w1, [x29, 223]
	ldr	x1, [x0]
	ldr	x1, [x1, -24]
	add	x1, x0, x1
	ldr	x1, [x1, 16]
	cbz	x1, L236
	ldr	x1, [x29, 128]
	mov	x2, 1
	bl	__ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l
LEHE22:
L221:
	add	x19, x19, 4
	cmp	x28, x19
	bne	L238
	add	w25, w25, 1
	cmp	w25, 11
	bne	L239
	ldr	x0, [x29, 136]
	add	x0, x0, 4
	str	x0, [x29, 136]
	cmp	x0, 12
	bne	L240
	mov	x12, 4800
	mov	x1, 4608
	ldr	x0, [x29, 176]
	ldp	x19, x20, [sp, 16]
	movk	x1, 0x7a, lsl 16
	ldp	x21, x22, [sp, 32]
	ldp	x23, x24, [sp, 48]
	ldp	x25, x26, [sp, 64]
	ldp	x27, x28, [sp, 80]
	ldp	x29, x30, [sp]
	add	sp, sp, x12
LCFI62:
LEHB23:
	b	__ZdlPvm
LEHE23:
L236:
LCFI63:
	mov	w1, 10
LEHB24:
	bl	__ZNSo3putEc
	b	L221
L234:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L235
L232:
	mov	w1, 44
	bl	__ZNSo3putEc
	b	L233
L230:
	mov	w1, 44
	str	x0, [x29, 152]
	bl	__ZNSo3putEc
	ldr	x2, [x29, 152]
	b	L231
L228:
	mov	w1, 44
	mov	x0, x22
	bl	__ZNSo3putEc
	b	L229
L224:
	mov	w1, 44
	mov	x0, x21
	bl	__ZNSo3putEc
	ldr	x0, [x29, 184]
	mov	x22, x21
	cbnz	x0, L226
L256:
	ldr	x0, [x22]
	ldr	x0, [x0, -24]
	add	x0, x22, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
	b	L227
L255:
	ldr	x0, [x21]
	ldr	x0, [x0, -24]
	add	x0, x21, x0
	ldr	w1, [x0, 32]
	orr	w1, w1, 1
	bl	__ZNSt9basic_iosIcSt11char_traitsIcEE5clearESt12_Ios_Iostate
LEHE24:
	b	L223
L217:
	mov	x0, 16
	bl	___cxa_allocate_exception
	adrp	x1, lC0@PAGE
	mov	x20, x0
	add	x1, x1, lC0@PAGEOFF;
LEHB25:
	bl	__ZNSt13runtime_errorC1EPKc
LEHE25:
	adrp	x2, __ZNSt13runtime_errorD1Ev@GOTPAGE
	ldr	x2, [x2, __ZNSt13runtime_errorD1Ev@GOTPAGEOFF]
	mov	x0, x20
	adrp	x1, __ZTISt13runtime_error@GOTPAGE
	ldr	x1, [x1, __ZTISt13runtime_error@GOTPAGEOFF]
LEHB26:
	bl	___cxa_throw
LEHE26:
L244:
	mov	x19, x0
	b	L242
L245:
	mov	x19, x0
	mov	x0, x20
	bl	___cxa_free_exception
L242:
	ldr	x0, [x29, 176]
	mov	x1, 4608
	movk	x1, 0x7a, lsl 16
	bl	__ZdlPvm
	mov	x0, x19
LEHB27:
	bl	__Unwind_Resume
LEHE27:
LFE5703:
	.section __TEXT,__gcc_except_tab
	.p2align	2
GCC_except_table3:
LLSDA5703:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 LLSDACSE5703-LLSDACSB5703
LLSDACSB5703:
	.uleb128 LEHB21-LFB5703
	.uleb128 LEHE21-LEHB21
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB22-LFB5703
	.uleb128 LEHE22-LEHB22
	.uleb128 L244-LFB5703
	.uleb128 0
	.uleb128 LEHB23-LFB5703
	.uleb128 LEHE23-LEHB23
	.uleb128 0
	.uleb128 0
	.uleb128 LEHB24-LFB5703
	.uleb128 LEHE24-LEHB24
	.uleb128 L244-LFB5703
	.uleb128 0
	.uleb128 LEHB25-LFB5703
	.uleb128 LEHE25-LEHB25
	.uleb128 L245-LFB5703
	.uleb128 0
	.uleb128 LEHB26-LFB5703
	.uleb128 LEHE26-LEHB26
	.uleb128 L244-LFB5703
	.uleb128 0
	.uleb128 LEHB27-LFB5703
	.uleb128 LEHE27-LEHB27
	.uleb128 0
	.uleb128 0
LLSDACSE5703:
	.text
	.cstring
	.align	3
lC2:
	.ascii "type,direction,shift,flip,round,ns\12\0"
	.align	3
lC3:
	.ascii "asc\0"
	.align	3
lC4:
	.ascii "int\0"
	.align	3
lC5:
	.ascii "desc\0"
	.align	3
lC6:
	.ascii "longLong\0"
	.section __TEXT,__text_startup,regular,pure_instructions
	.align	2
	.p2align 5,,15
	.globl _main
_main:
LFB5248:
	adrp	x1, lC2@PAGE
	stp	x29, x30, [sp, -48]!
LCFI64:
	mov	x29, sp
LCFI65:
	add	x1, x1, lC2@PAGEOFF;
	adrp	x0, __ZSt4cout@GOTPAGE
	ldr	x0, [x0, __ZSt4cout@GOTPAGEOFF]
	stp	x19, x20, [sp, 16]
LCFI66:
	adrp	x20, lC3@PAGE
	adrp	x19, lC5@PAGE
	bl	__ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	ldr	x1, [x0]
	mov	w2, -261
	ldr	x1, [x1, -24]
	add	x0, x0, x1
	ldr	w1, [x0, 24]
	and	w1, w1, w2
	orr	w1, w1, 4
	str	w1, [x0, 24]
	mov	x1, 3
	str	x1, [x0, 8]
	adrp	x0, lC4@PAGE
	add	x1, x20, lC3@PAGEOFF;
	add	x0, x0, lC4@PAGEOFF;
	str	x0, [x29, 40]
	bl	__Z7measureIiLb0EEvPKcS1_
	ldr	x0, [x29, 40]
	add	x1, x19, lC5@PAGEOFF;
	bl	__Z7measureIiLb1EEvPKcS1_
	add	x1, x20, lC3@PAGEOFF;
	adrp	x0, lC6@PAGE
	add	x0, x0, lC6@PAGEOFF;
	str	x0, [x29, 40]
	bl	__Z7measureIxLb0EEvPKcS1_
	ldr	x0, [x29, 40]
	add	x1, x19, lC5@PAGEOFF;
	bl	__Z7measureIxLb1EEvPKcS1_
	ldp	x19, x20, [sp, 16]
	mov	w0, 0
	ldp	x29, x30, [sp], 48
LCFI67:
	ret
LFE5248:
	.section	__TEXT,__StaticInit,regular,pure_instructions
	.align	2
	.p2align 5,,15
__GLOBAL__sub_I_histogram.cpp:
LFB6341:
	adrp	x1, __ZStL8__ioinit@PAGE
	stp	x29, x30, [sp, -32]!
LCFI68:
	mov	x29, sp
LCFI69:
	add	x1, x1, __ZStL8__ioinit@PAGEOFF;
	mov	x0, x1
	str	x1, [x29, 24]
	bl	__ZNSt8ios_base4InitC1Ev
	adrp	x2, ___dso_handle@PAGE
	ldr	x1, [x29, 24]
	add	x2, x2, ___dso_handle@PAGEOFF;
	ldp	x29, x30, [sp], 32
LCFI70:
	adrp	x0, __ZNSt8ios_base4InitD1Ev@GOTPAGE
	ldr	x0, [x0, __ZNSt8ios_base4InitD1Ev@GOTPAGEOFF]
	b	___cxa_atexit
LFE6341:
	.const
	.align	2
_C.36.3:
	.word	0
	.word	8
	.word	16
	.align	2
_C.27.2:
	.word	0
	.word	8
	.word	16
	.align	2
_C.21.1:
	.word	0
	.word	8
	.word	16
	.align	2
_C.2.0:
	.word	0
	.word	8
	.word	16
	.zerofill __DATA,__bss,__ZStL8__ioinit,1,0
	.literal16
	.align	4
lC1:
	.word	0
	.word	1
	.word	128
	.word	255
	.section __TEXT,__eh_frame,coalesced,no_toc+strip_static_syms+live_support
EH_frame1:
	.set L$set$0,LECIE1-LSCIE1
	.long L$set$0
LSCIE1:
	.long	0
	.byte	0x3
	.ascii "zPLR\0"
	.uleb128 0x1
	.sleb128 -8
	.uleb128 0x1e
	.uleb128 0x7
	.byte	0x9b
L_got_pcr0:
	.long	___gxx_personality_v0@GOT-L_got_pcr0
	.byte	0x10
	.byte	0x10
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LECIE1:
LSFDE1:
	.set L$set$1,LEFDE1-LASFDE1
	.long L$set$1
LASFDE1:
	.long	LASFDE1-EH_frame1
	.quad	LFB6347-.
	.set L$set$2,LFE6347-LFB6347
	.quad L$set$2
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$3,LCFI0-LFB6347
	.long L$set$3
	.byte	0xe
	.uleb128 0x40
	.byte	0x9d
	.uleb128 0x8
	.byte	0x9e
	.uleb128 0x7
	.byte	0x4
	.set L$set$4,LCFI1-LCFI0
	.long L$set$4
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$5,LCFI2-LCFI1
	.long L$set$5
	.byte	0x93
	.uleb128 0x6
	.byte	0x94
	.uleb128 0x5
	.byte	0x4
	.set L$set$6,LCFI3-LCFI2
	.long L$set$6
	.byte	0x95
	.uleb128 0x4
	.byte	0x4
	.set L$set$7,LCFI4-LCFI3
	.long L$set$7
	.byte	0xde
	.byte	0xdd
	.byte	0xd5
	.byte	0xd3
	.byte	0xd4
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE1:
LSFDE3:
	.set L$set$8,LEFDE3-LASFDE3
	.long L$set$8
LASFDE3:
	.long	LASFDE3-EH_frame1
	.quad	LFB6348-.
	.set L$set$9,LFE6348-LFB6348
	.quad L$set$9
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$10,LCFI5-LFB6348
	.long L$set$10
	.byte	0xe
	.uleb128 0x40
	.byte	0x9d
	.uleb128 0x8
	.byte	0x9e
	.uleb128 0x7
	.byte	0x4
	.set L$set$11,LCFI6-LCFI5
	.long L$set$11
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$12,LCFI7-LCFI6
	.long L$set$12
	.byte	0x93
	.uleb128 0x6
	.byte	0x94
	.uleb128 0x5
	.byte	0x4
	.set L$set$13,LCFI8-LCFI7
	.long L$set$13
	.byte	0x95
	.uleb128 0x4
	.byte	0x4
	.set L$set$14,LCFI9-LCFI8
	.long L$set$14
	.byte	0xde
	.byte	0xdd
	.byte	0xd5
	.byte	0xd3
	.byte	0xd4
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE3:
LSFDE5:
	.set L$set$15,LEFDE5-LASFDE5
	.long L$set$15
LASFDE5:
	.long	LASFDE5-EH_frame1
	.quad	LFB6349-.
	.set L$set$16,LFE6349-LFB6349
	.quad L$set$16
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$17,LCFI10-LFB6349
	.long L$set$17
	.byte	0xe
	.uleb128 0x30
	.byte	0x9d
	.uleb128 0x6
	.byte	0x9e
	.uleb128 0x5
	.byte	0x4
	.set L$set$18,LCFI11-LCFI10
	.long L$set$18
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$19,LCFI12-LCFI11
	.long L$set$19
	.byte	0x93
	.uleb128 0x4
	.byte	0x94
	.uleb128 0x3
	.byte	0x4
	.set L$set$20,LCFI13-LCFI12
	.long L$set$20
	.byte	0xde
	.byte	0xdd
	.byte	0xd3
	.byte	0xd4
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE5:
LSFDE7:
	.set L$set$21,LEFDE7-LASFDE7
	.long L$set$21
LASFDE7:
	.long	LASFDE7-EH_frame1
	.quad	LFB6350-.
	.set L$set$22,LFE6350-LFB6350
	.quad L$set$22
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$23,LCFI14-LFB6350
	.long L$set$23
	.byte	0xe
	.uleb128 0x30
	.byte	0x9d
	.uleb128 0x6
	.byte	0x9e
	.uleb128 0x5
	.byte	0x4
	.set L$set$24,LCFI15-LCFI14
	.long L$set$24
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$25,LCFI16-LCFI15
	.long L$set$25
	.byte	0x93
	.uleb128 0x4
	.byte	0x94
	.uleb128 0x3
	.byte	0x4
	.set L$set$26,LCFI17-LCFI16
	.long L$set$26
	.byte	0xde
	.byte	0xdd
	.byte	0xd3
	.byte	0xd4
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE7:
LSFDE9:
	.set L$set$27,LEFDE9-LASFDE9
	.long L$set$27
LASFDE9:
	.long	LASFDE9-EH_frame1
	.quad	LFB6076-.
	.set L$set$28,LFE6076-LFB6076
	.quad L$set$28
	.uleb128 0x8
	.quad	0
	.align	3
LEFDE9:
LSFDE11:
	.set L$set$29,LEFDE11-LASFDE11
	.long L$set$29
LASFDE11:
	.long	LASFDE11-EH_frame1
	.quad	LFB5908-.
	.set L$set$30,LFE5908-LFB5908
	.quad L$set$30
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$31,LCFI18-LFB5908
	.long L$set$31
	.byte	0xe
	.uleb128 0x20
	.byte	0x9d
	.uleb128 0x4
	.byte	0x9e
	.uleb128 0x3
	.byte	0x4
	.set L$set$32,LCFI19-LCFI18
	.long L$set$32
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$33,LCFI20-LCFI19
	.long L$set$33
	.byte	0xa
	.byte	0xde
	.byte	0xdd
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$34,LCFI21-LCFI20
	.long L$set$34
	.byte	0xb
	.align	3
LEFDE11:
LSFDE13:
	.set L$set$35,LEFDE13-LASFDE13
	.long L$set$35
LASFDE13:
	.long	LASFDE13-EH_frame1
	.quad	LFB5911-.
	.set L$set$36,LFE5911-LFB5911
	.quad L$set$36
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$37,LCFI22-LFB5911
	.long L$set$37
	.byte	0xe
	.uleb128 0x50
	.byte	0x9d
	.uleb128 0xa
	.byte	0x9e
	.uleb128 0x9
	.byte	0x4
	.set L$set$38,LCFI23-LCFI22
	.long L$set$38
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$39,LCFI24-LCFI23
	.long L$set$39
	.byte	0x97
	.uleb128 0x4
	.byte	0x98
	.uleb128 0x3
	.byte	0x4
	.set L$set$40,LCFI25-LCFI24
	.long L$set$40
	.byte	0x95
	.uleb128 0x6
	.byte	0x96
	.uleb128 0x5
	.byte	0x4
	.set L$set$41,LCFI26-LCFI25
	.long L$set$41
	.byte	0x94
	.uleb128 0x7
	.byte	0x93
	.uleb128 0x8
	.byte	0x4
	.set L$set$42,LCFI27-LCFI26
	.long L$set$42
	.byte	0x9a
	.uleb128 0x1
	.byte	0x99
	.uleb128 0x2
	.byte	0x4
	.set L$set$43,LCFI28-LCFI27
	.long L$set$43
	.byte	0xd4
	.byte	0xd3
	.byte	0x4
	.set L$set$44,LCFI29-LCFI28
	.long L$set$44
	.byte	0xda
	.byte	0xd9
	.byte	0x4
	.set L$set$45,LCFI30-LCFI29
	.long L$set$45
	.byte	0xa
	.byte	0xde
	.byte	0xdd
	.byte	0xd7
	.byte	0xd8
	.byte	0xd5
	.byte	0xd6
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$46,LCFI31-LCFI30
	.long L$set$46
	.byte	0xb
	.byte	0x4
	.set L$set$47,LCFI32-LCFI31
	.long L$set$47
	.byte	0x94
	.uleb128 0x7
	.byte	0x93
	.uleb128 0x8
	.byte	0x4
	.set L$set$48,LCFI33-LCFI32
	.long L$set$48
	.byte	0x9a
	.uleb128 0x1
	.byte	0x99
	.uleb128 0x2
	.byte	0x4
	.set L$set$49,LCFI34-LCFI33
	.long L$set$49
	.byte	0xd3
	.byte	0xd4
	.byte	0xd9
	.byte	0xda
	.byte	0x4
	.set L$set$50,LCFI35-LCFI34
	.long L$set$50
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0xd5
	.byte	0xd6
	.byte	0xd7
	.byte	0xd8
	.byte	0xdd
	.byte	0xde
	.align	3
LEFDE13:
LSFDE15:
	.set L$set$51,LEFDE15-LASFDE15
	.long L$set$51
LASFDE15:
	.long	LASFDE15-EH_frame1
	.quad	LFB5686-.
	.set L$set$52,LFE5686-LFB5686
	.quad L$set$52
	.uleb128 0x8
	.quad	LLSDA5686-.
	.byte	0x4
	.set L$set$53,LCFI36-LFB5686
	.long L$set$53
	.byte	0xe
	.uleb128 0x12c0
	.byte	0x4
	.set L$set$54,LCFI37-LCFI36
	.long L$set$54
	.byte	0x9d
	.uleb128 0x258
	.byte	0x9e
	.uleb128 0x257
	.byte	0x4
	.set L$set$55,LCFI38-LCFI37
	.long L$set$55
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$56,LCFI39-LCFI38
	.long L$set$56
	.byte	0x93
	.uleb128 0x256
	.byte	0x94
	.uleb128 0x255
	.byte	0x4
	.set L$set$57,LCFI40-LCFI39
	.long L$set$57
	.byte	0x95
	.uleb128 0x254
	.byte	0x96
	.uleb128 0x253
	.byte	0x97
	.uleb128 0x252
	.byte	0x98
	.uleb128 0x251
	.byte	0x99
	.uleb128 0x250
	.byte	0x9a
	.uleb128 0x24f
	.byte	0x9b
	.uleb128 0x24e
	.byte	0x9c
	.uleb128 0x24d
	.byte	0x4
	.set L$set$58,LCFI41-LCFI40
	.long L$set$58
	.byte	0xa
	.byte	0xdb
	.byte	0xdc
	.byte	0xd9
	.byte	0xda
	.byte	0xd7
	.byte	0xd8
	.byte	0xd5
	.byte	0xd6
	.byte	0xd3
	.byte	0xd4
	.byte	0xdd
	.byte	0xde
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$59,LCFI42-LCFI41
	.long L$set$59
	.byte	0xb
	.align	3
LEFDE15:
LSFDE17:
	.set L$set$60,LEFDE17-LASFDE17
	.long L$set$60
LASFDE17:
	.long	LASFDE17-EH_frame1
	.quad	LFB5697-.
	.set L$set$61,LFE5697-LFB5697
	.quad L$set$61
	.uleb128 0x8
	.quad	LLSDA5697-.
	.byte	0x4
	.set L$set$62,LCFI43-LFB5697
	.long L$set$62
	.byte	0xe
	.uleb128 0x12c0
	.byte	0x4
	.set L$set$63,LCFI44-LCFI43
	.long L$set$63
	.byte	0x9d
	.uleb128 0x258
	.byte	0x9e
	.uleb128 0x257
	.byte	0x4
	.set L$set$64,LCFI45-LCFI44
	.long L$set$64
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$65,LCFI46-LCFI45
	.long L$set$65
	.byte	0x93
	.uleb128 0x256
	.byte	0x94
	.uleb128 0x255
	.byte	0x4
	.set L$set$66,LCFI47-LCFI46
	.long L$set$66
	.byte	0x95
	.uleb128 0x254
	.byte	0x96
	.uleb128 0x253
	.byte	0x97
	.uleb128 0x252
	.byte	0x98
	.uleb128 0x251
	.byte	0x99
	.uleb128 0x250
	.byte	0x9a
	.uleb128 0x24f
	.byte	0x9b
	.uleb128 0x24e
	.byte	0x9c
	.uleb128 0x24d
	.byte	0x4
	.set L$set$67,LCFI48-LCFI47
	.long L$set$67
	.byte	0xa
	.byte	0xdb
	.byte	0xdc
	.byte	0xd9
	.byte	0xda
	.byte	0xd7
	.byte	0xd8
	.byte	0xd5
	.byte	0xd6
	.byte	0xd3
	.byte	0xd4
	.byte	0xdd
	.byte	0xde
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$68,LCFI49-LCFI48
	.long L$set$68
	.byte	0xb
	.align	3
LEFDE17:
LSFDE19:
	.set L$set$69,LEFDE19-LASFDE19
	.long L$set$69
LASFDE19:
	.long	LASFDE19-EH_frame1
	.quad	LFB5698-.
	.set L$set$70,LFE5698-LFB5698
	.quad L$set$70
	.uleb128 0x8
	.quad	LLSDA5698-.
	.byte	0x4
	.set L$set$71,LCFI50-LFB5698
	.long L$set$71
	.byte	0xe
	.uleb128 0x12c0
	.byte	0x4
	.set L$set$72,LCFI51-LCFI50
	.long L$set$72
	.byte	0x9d
	.uleb128 0x258
	.byte	0x9e
	.uleb128 0x257
	.byte	0x4
	.set L$set$73,LCFI52-LCFI51
	.long L$set$73
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$74,LCFI53-LCFI52
	.long L$set$74
	.byte	0x93
	.uleb128 0x256
	.byte	0x94
	.uleb128 0x255
	.byte	0x4
	.set L$set$75,LCFI54-LCFI53
	.long L$set$75
	.byte	0x95
	.uleb128 0x254
	.byte	0x96
	.uleb128 0x253
	.byte	0x97
	.uleb128 0x252
	.byte	0x98
	.uleb128 0x251
	.byte	0x99
	.uleb128 0x250
	.byte	0x9a
	.uleb128 0x24f
	.byte	0x9b
	.uleb128 0x24e
	.byte	0x9c
	.uleb128 0x24d
	.byte	0x4
	.set L$set$76,LCFI55-LCFI54
	.long L$set$76
	.byte	0xa
	.byte	0xdb
	.byte	0xdc
	.byte	0xd9
	.byte	0xda
	.byte	0xd7
	.byte	0xd8
	.byte	0xd5
	.byte	0xd6
	.byte	0xd3
	.byte	0xd4
	.byte	0xdd
	.byte	0xde
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$77,LCFI56-LCFI55
	.long L$set$77
	.byte	0xb
	.align	3
LEFDE19:
LSFDE21:
	.set L$set$78,LEFDE21-LASFDE21
	.long L$set$78
LASFDE21:
	.long	LASFDE21-EH_frame1
	.quad	LFB5703-.
	.set L$set$79,LFE5703-LFB5703
	.quad L$set$79
	.uleb128 0x8
	.quad	LLSDA5703-.
	.byte	0x4
	.set L$set$80,LCFI57-LFB5703
	.long L$set$80
	.byte	0xe
	.uleb128 0x12c0
	.byte	0x4
	.set L$set$81,LCFI58-LCFI57
	.long L$set$81
	.byte	0x9d
	.uleb128 0x258
	.byte	0x9e
	.uleb128 0x257
	.byte	0x4
	.set L$set$82,LCFI59-LCFI58
	.long L$set$82
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$83,LCFI60-LCFI59
	.long L$set$83
	.byte	0x93
	.uleb128 0x256
	.byte	0x94
	.uleb128 0x255
	.byte	0x4
	.set L$set$84,LCFI61-LCFI60
	.long L$set$84
	.byte	0x95
	.uleb128 0x254
	.byte	0x96
	.uleb128 0x253
	.byte	0x97
	.uleb128 0x252
	.byte	0x98
	.uleb128 0x251
	.byte	0x99
	.uleb128 0x250
	.byte	0x9a
	.uleb128 0x24f
	.byte	0x9b
	.uleb128 0x24e
	.byte	0x9c
	.uleb128 0x24d
	.byte	0x4
	.set L$set$85,LCFI62-LCFI61
	.long L$set$85
	.byte	0xa
	.byte	0xdb
	.byte	0xdc
	.byte	0xd9
	.byte	0xda
	.byte	0xd7
	.byte	0xd8
	.byte	0xd5
	.byte	0xd6
	.byte	0xd3
	.byte	0xd4
	.byte	0xdd
	.byte	0xde
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.byte	0x4
	.set L$set$86,LCFI63-LCFI62
	.long L$set$86
	.byte	0xb
	.align	3
LEFDE21:
LSFDE23:
	.set L$set$87,LEFDE23-LASFDE23
	.long L$set$87
LASFDE23:
	.long	LASFDE23-EH_frame1
	.quad	LFB5248-.
	.set L$set$88,LFE5248-LFB5248
	.quad L$set$88
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$89,LCFI64-LFB5248
	.long L$set$89
	.byte	0xe
	.uleb128 0x30
	.byte	0x9d
	.uleb128 0x6
	.byte	0x9e
	.uleb128 0x5
	.byte	0x4
	.set L$set$90,LCFI65-LCFI64
	.long L$set$90
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$91,LCFI66-LCFI65
	.long L$set$91
	.byte	0x93
	.uleb128 0x4
	.byte	0x94
	.uleb128 0x3
	.byte	0x4
	.set L$set$92,LCFI67-LCFI66
	.long L$set$92
	.byte	0xde
	.byte	0xdd
	.byte	0xd3
	.byte	0xd4
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE23:
LSFDE25:
	.set L$set$93,LEFDE25-LASFDE25
	.long L$set$93
LASFDE25:
	.long	LASFDE25-EH_frame1
	.quad	LFB6341-.
	.set L$set$94,LFE6341-LFB6341
	.quad L$set$94
	.uleb128 0x8
	.quad	0
	.byte	0x4
	.set L$set$95,LCFI68-LFB6341
	.long L$set$95
	.byte	0xe
	.uleb128 0x20
	.byte	0x9d
	.uleb128 0x4
	.byte	0x9e
	.uleb128 0x3
	.byte	0x4
	.set L$set$96,LCFI69-LCFI68
	.long L$set$96
	.byte	0xd
	.uleb128 0x1d
	.byte	0x4
	.set L$set$97,LCFI70-LCFI69
	.long L$set$97
	.byte	0xde
	.byte	0xdd
	.byte	0xc
	.uleb128 0x1f
	.uleb128 0
	.align	3
LEFDE25:
	.private_extern ___dso_handle
	.ident	"GCC: (Homebrew GCC 15.2.0_1) 15.2.0"
	.mod_init_func
_Mod.init:
	.align	3
	.xword	__GLOBAL__sub_I_histogram.cpp
	.subsections_via_symbols
