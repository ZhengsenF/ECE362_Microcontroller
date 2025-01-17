.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.data
i: .word 0
size: .word 4
sum: .word 0
source:
.word 3
.word 4
.word 7
.word 11
.word 18
.word 29
.word 47
.word 76
.word 123
.word 199
.word 322

str: .string "hello, 01234 world 56789"


.text
.global main
main:
	// r0 = sum
	// r1 = size
	// r2 = i
	movs r2, #0

	ldr r1, =size
	ldr r1, [r1] // load size into r1

	movs r0, #0 // sum = 0

	for1:
		movs r2, #0
	check1:
		cmp r2, r1
		bge endfor1
	body1:
		if1:
			movs r3, #25
			cmp r0, r3
			bge else1
		then1:
			// r7 = addr of source[i]
			ldr r7, =source
			// r3 = i * 4
			lsls r3, r2, #2
			ldr r4, [r7, r3] //r4 = source[i]
			adds r3, #4
			ldr r5, [r7, r3] //r5 = source[i+1]
			adds r4, r5
			adds r0, r4
			b endif1
		else1:
			// r7 = addr of source[i]
			ldr r7, =source
			// r3 = i * 4
			lsls r3, r2, #2
			ldr r4, [r7, r3] //r4 = source[i]
			lsls r4, r4, #1 // source[i] * 2
			subs r0, r4
		endif1:
	next1:
		adds r2, #1
		b check1
	endfor1:
	ldr r4, =sum
	str r0, [r4]

// part 2
// r0 = x
// r1 = tmp
// r2 = addr of str[0]


	ldr r2, =str
	for2:
		movs r0, #0
	check2:
		movs r3, r0
		ldrb r4, [r2, r3] // r4 = str[x]
		adds r3, #1
		ldrb r5, [r2, r3] // r5 = str[x+1]
		forand1:
		movs r3, #0x00
		cmp r4, r3
		beq endfor2
		forand2:
		cmp r5, r3
		beq endfor2

	body2:
		if2:
			movs r3, r0
			ldrb r4, [r2, r3] // r4 = str[x]
			adds r3, #1
			ldrb r5, [r2, r3] // r5 = str[x+1]
			ifand1:
			movs r3, #0x61 // r3 = 'a'
			cmp r4, r3
			blt endif2
			ifand2:
			movs r3, #0x7a // r3 = 'z'
			cmp r4, r3
			bgt endif2
		then2:
			movs r3, r0
			strb r5, [r2,r3] // store value of str[x+1] into str[x]
			adds r3, #1
			strb r4, [r2,r3] // store value of str[x] into str[x+1]
		endif2:
	next2:
		adds r0, #2
		b check2
	endfor2:






    wfi
