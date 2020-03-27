.syntax unified
.cpu cortex-m0
.thumb
.fpu softvfp

//===============================================================
// Arithmetic operations
//
// Exercise 1
// Description:
//    R0, R1, R2, and R3 will be initialized with values.
//    Fill in the provided subroutine body with assembly
//    language instructions to set the value in R0 to be
//    (R0 - R3) * R2 + R1. Don't change any registers other
//    than R0 - R3 to accomplish this.
//===============================================================
.global codeSegment1
codeSegment1:
        // Student code goes below
	subs r0, r0, r3
	muls r0, r2, r0
	adds r0, r0, r1
	// Student code goes above
	bx lr

//===============================================================
// Exercise 2
// Description:
//    R0, R1, R2, and R3 will be initialized with values.
//    Fill in the provided subroutine body with assembly
//    language instructions to set the value in R0 to be
//    ((-R0) * (R1 - R3)) - R2.  Don't change any registers
//    other than R0 - R3 to accomplish this.
//===============================================================
.global codeSegment2
codeSegment2:
        // Student code goes below
	rsbs r0, #0
	subs r1, r3
	muls r0, r1
	subs r0, r2
	// Student code goes above
	bx lr

//===============================================================
// Exercise 3
// Description:
//    R0, R1, R2, and R3 will be initialized with values.
//    Fill in the provided subroutine body with assembly
//    language instructions to set the value in R0 to be
//    R3 - R2 - R1 - R0.  Don't change any registers other
//    than R0 - R3 to accomplish this.
//===============================================================
.global codeSegment3
codeSegment3:
        // Student code goes below
	subs r3, r3, r2
	subs r3, r3, r1
	subs r3, r3, r0
	movs r0, r3
	// Student code goes above
	bx lr

//===============================================================
// Logical operations
// Exercise 4
// Description:
//    R0, R1, R2, and R3 will be initialized with values.
//    Fill in the provided subroutine body with assembly
//    language instructions to set the value in R0 to be
//    (R1 ^ R3) & (R2 | R0). Don't change any registers
//    other than R0 - R3 to accomplish this.
//===============================================================
.global codeSegment4
codeSegment4:
        // Student code goes below
	eors r1, r3
	orrs r2, r0
	ands r1, r2
	movs r0, r1
	// Student code goes above
	bx lr

//===============================================================
// Exercise 5
// Description:
//    R0, R1, R2, and R3 will be initialized with values.
//    Fill in the provided subroutine body with assembly
//    language instructions to set the value in R0 to be
//    (R3 << R2) | (R0 >> R1).  Don't change any registers
//    other than R0 - R3 to accomplish this.
//===============================================================
.global codeSegment5
codeSegment5:
        // Student code goes below
	lsls r3, r2
	lsrs r0, r1
	orrs r0, r3
	// Student code goes above
	bx lr

//===============================================================
// Immediate load operations
// Exercise 6
// Description:
//    R0, R1, R2, and R3 are to be set to the values
//    specified.  Use MOVS immediate instructions as well
//    as arithmetic and logical instructions to accomplish
//    the initialization of the registers:
//    set R0 to Data1   (see the definition below)
//    set R1 to 255 (base 10)
//    set R2 to 510 (base 10)
//    set R3 to 0
//===============================================================
.equ Data1, 0x6 // This is the definition of Data1
.global codeSegment6
codeSegment6:
        // Student code goes below
	movs r0, Data1
	movs r1, #200
	ldr r2, =400
	movs r3, #0
	// Student code goes above
	bx lr

//===============================================================
// Put all of the .equ statements for codeSegment7 below:
//===============================================================
.equ RCC, 0x40021000
.equ AHBENR, 0x14
.equ GPIOCEN, 0x00080000
.equ GPIOC, 0x48000800
.equ MODER, 0x00
.equ ODR, 0x14
.equ PIN9MOD, 0x00040000
.equ PIN9, 0x00000200




//===============================================================
// Fill in codeSegment7
//===============================================================
.global codeSegment7
codeSegment7:
	// Student code goes below

	ldr r3, =RCC
	ldr r2, =AHBENR
	ldr r1, =GPIOCEN
	ldr r0, [r3, r2]
	orrs r0, r1
	str r0, [r3, r2]


	ldr r3, =GPIOC
	ldr r2, =MODER
	ldr r1, = PIN9MOD
	ldr r0, [r3, r2]
	orrs r0, r1
	str r0, [r3, r2]

	ldr r2, =GPIOC
	ldr r3, =ODR
	ldr r1, =PIN9
	ldr r0, [r3, r2]

	ldr r4, =PIN9
	orrs r0, r4
	str r0, [r3, r2]

endless_loop:
	bl delay
	eors r0, r4
	str r0, [r3, r2]
	b endless_loop

delay:
	ldr r1, =2000000
delay_loop:
	subs r1, r1, #1
	bne delay_loop

	bx lr

	// Student code goes above
