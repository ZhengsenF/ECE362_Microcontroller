.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

//=============================================================================
// ECE 362 Lab Experiment 5
// Timers
//
//=============================================================================

.equ  RCC,      0x40021000
.equ  APB1ENR,  0x1C
.equ  AHBENR,   0x14
.equ  TIM6EN,	0x10
.equ  GPIOCEN,  0x00080000
.equ  GPIOBEN,  0x00040000
.equ  GPIOAEN,  0x00020000
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  GPIOA,    0x48000000
.equ  MODER,    0x00
.equ  PUPDR,    0x0c
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  PC0,      0x01
.equ  PC1,      0x04
.equ  PC2,      0x10
.equ  PC3,      0x40
.equ  PIN8,     0x00000100

// NVIC control registers...
.equ NVIC,		0xe000e000
.equ ISER, 		0x100
.equ ICER, 		0x180
.equ ISPR, 		0x200
.equ ICPR, 		0x280

// TIM6 control registers
.equ  TIM6, 	0x40001000
.equ  CR1,		0x00
.equ  DIER,		0x0C
.equ  PSC,		0x28
.equ  ARR,		0x2C
.equ  TIM6_DAC_IRQn, 17
.equ  SR,		0x10

//=======================================================
// 6.1: Configure timer 6
//=======================================================
.global init_TIM6
init_TIM6:
	push {lr}
	// enable clock to timer 6
	ldr r0, =RCC
	ldr r1, [r0, #APB1ENR]
	ldr r2, =TIM6EN
	orrs r1, r2
	str r1, [r0, #APB1ENR]

	// set PSC and ARR
	ldr r0, =TIM6
	ldr r1, =480-1
	str r1, [r0, #PSC]

	ldr r1, = 100-1
	str r1, [r0, #ARR]

	// enable UIE in DIER
	ldr r1, [r0, #DIER]
	ldr r2, =1
	orrs r1, r2
	str r1, [r0, #DIER]

	//enable CEN in CR1
	ldr r1, [r0, #CR1]
	ldr r2, =1
	orrs r1, r2
	str r1, [r0, #CR1]

	// enable interupt in NVIC's ISER
	ldr r0, =NVIC
	ldr r1, =ISER
	ldr r2, =(1<<TIM6_DAC_IRQn)
	str r2, [r0,r1]



	pop {pc}

//=======================================================
// 6.2: Confiure GPIO
//=======================================================
.global init_GPIO
init_GPIO:
	push {r4-r7, lr}
	// enable clock to port c and a
	ldr r0, =RCC
	ldr r1, [r0, #AHBENR]
	ldr r2, =GPIOCEN
	orrs r1, r2
	ldr r2, =GPIOAEN
	orrs r1, r2
	str r1, [r0, #AHBENR]

	// set PC0, PC1, PC2, PC3 and PC8 as outputs
	ldr r0, =GPIOC
	ldr r1, [r0, #MODER]
	ldr r2, =1 // PC0
	orrs r1, r2
	ldr r2, =4 // PC1
	orrs r1, r2
	ldr r2, =16 // PC2
	orrs r1, r2
	ldr r2, =64 // PC3
	orrs r1, r2
	ldr r2, =65536 // PC8
	orrs r1, r2
	str r1, [r0, #MODER]

	// Set PA0, PA1, PA2 and PA3 as outputs
	ldr r0, =GPIOA
	ldr r1, [r0, #MODER]
	ldr r2, =1 // PA0
	orrs r1, r2
	ldr r2, =4 // PA1
	orrs r1, r2
	ldr r2, =16 // PA2
	orrs r1, r2
	ldr r2, =64 // PA3
	orrs r1, r2
	str r1, [r0, #MODER]

	// Set up a pull down resistance on pins PA4, PA5, PA6 and PA7.
	ldr r1, [r0, #PUPDR]
	ldr r2, =512 // 4
	orrs r1, r2
	ldr r2, =2048 // 5
	orrs r1, r2
	ldr r2, =8192 // 6
	orrs r1, r2
	ldr r2, =32768 // 7
	orrs r1, r2
	str r1, [r0, #PUPDR]

	pop {r4-r7, pc}

//=======================================================
// 6.3 Blink blue LED using Timer 6 interrupt
// Write your interrupt service routine below.
//=======================================================
.type TIM6_DAC_IRQHandler, %function
.global TIM6_DAC_IRQHandler
TIM6_DAC_IRQHandler:
	push {r4-r7,lr}
	// acknowledge
	ldr  r0,=TIM6
	ldr  r1,[r0,#SR]// read status reg
	ldr  r2,=1
	bics r1,r2// turn off UIF
	str  r1,[r0,#SR]// write it

	//increment tick
	ldr r0, =tick
	ldr r1, [r0]
	adds r1, #1


	// check
	if1:
		ldr r7, =1000
		cmp r1, r7
		bne endif1
	then1:
		ldr r2, =1 << 8
		ldr r4, =GPIOC
		ldr r5, [r4, #ODR]
		eors r5, r2
		str r5, [r4, #ODR]
		movs r1, #0
	endif1:
	str r1, [r0]


	// 6.5.3
	// r1 = col
	ldr r0, =col
	ldr r1, [r0]
	adds r1, #1
	ldr r2, =3
	ands r1, r2 // (col + 1) & 3
	str r1, [r0]
	movs r2, #1
	lsls r2, r2, r1 // r2 = 1 << col
	ldr r3, =GPIOA
	str r2, [r3, #ODR]

	// r0 = history[0]
	// r2 = index
	// r3 = history[index]
	ldr r0, =history
	ldr r3, =2
	movs r2, r1
	lsls r2, r2, r3 // r2 = col << 2
	movs r1, r2

	ldrb r3, [r0, r2] //index
	lsls r3, r3, #1
	strb r3, [r0, r2]

	adds r2, #1 //index + 1
	ldrb r3, [r0, r2]
	lsls r3, r3, #1
	strb r3, [r0, r2]

	adds r2, #1 //index + 2
	ldrb r3, [r0, r2]
	lsls r3, r3, #1
	strb r3, [r0, r2]

	adds r2, #1 //index + 3
	ldrb r3, [r0, r2]
	lsls r3, r3, #1
	strb r3, [r0, r2]

	// read row
	// r0 = history[0]
	// r2 = index
	// r3 = history[index]
	// r4 = row

	ldr r5, =GPIOA
	ldr r5, [r5, #IDR] // r5 = GPIOA -> IDR
	lsrs r5, r5, #4
	ldr r6, =0xf
	ands r5, r6
	movs r4, r5
	movs r2, r1 //r2 = index

	ldrb r3, [r0, r2] //index
	movs r5, r4
	movs r6, #1
	ands r5, r6
	orrs r3, r5
	strb r3, [r0, r2]

	adds r2, #1
	ldrb r3, [r0, r2] //index + 1
	movs r5, r4
	lsrs r5, r5, #1 // row >> 1
	movs r6, #1
	ands r5, r6
	orrs r3, r5
	strb r3, [r0, r2]

	adds r2, #1
	ldrb r3, [r0, r2] //index + 2
	movs r5, r4
	lsrs r5, r5, #2
	movs r6, #1
	ands r5, r6
	orrs r3, r5
	strb r3, [r0, r2]

	adds r2, #1
	ldrb r3, [r0, r2] //index + 3
	movs r5, r4
	lsrs r5, r5, #3
	movs r6, #1
	ands r5, r6
	orrs r3, r5
	strb r3, [r0, r2]

	pop {r4-r7,pc}

//=======================================================
// 6.5 Debounce keypad
//=======================================================
.global getKeyPressed
getKeyPressed:
	// r0 = address of history[0]
	// r1 = i
	push {r4-r7, lr}
	ldr r0, =history
	for1:
		movs r1, #0
	check1:
		cmp r1, #16
		bge endfor1
	forbody1:
		// r2 = history[i]
		ldrb r2, [r0, r1]
		if2:
			cmp r2, #1
			bne endif2
		then2:
			ldr r3, =tick
			movs r4, #0
			str r4, [r3]
			movs r0, r1
			pop {r4-r7, pc}
		endif2:
	next1:
		adds r1, #1
		b check1
	endfor1:
	b for1
	pop {r4-r7, pc}

.global getKeyReleased
getKeyReleased:
	// r0 = address of history[0]
	// r1 = i
	push {r4-r7, lr}
	ldr r0, =history
	for2:
		movs r1, #0
	check2:
		cmp r1, #16
		bge endfor2
	forbody2:
		// r2 = history[i]
		ldrb r2, [r0, r1]
		if3:
			ldr r7, =0xfe
			cmp r2, r7
			bne endif3
		then3:
			ldr r3, =tick
			movs r4, #0
			str r4, [r3]
			movs r0, r1
			pop {r4-r7, pc}
		endif3:
	next2:
		adds r1, #1
		b check2
	endfor2:
	b for2
	pop {r4-r7, pc}
