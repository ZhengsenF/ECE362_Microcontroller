.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.equ RCC,       0x40021000
.equ GPIOA,     0x48000000
.equ GPIOC,     0x48000800
.equ AHBENR,    0x14
.equ APB2ENR,   0x18
.equ APB1ENR,   0x1c
.equ IOPAEN,    0x20000
.equ IOPCEN,    0x80000
.equ SYSCFGCOMPEN, 1
.equ TIM3EN,    2
.equ MODER,     0x00
.equ OSPEEDR,   0x08
.equ PUPDR,     0x0c
.equ IDR,       0x10
.equ ODR,       0x14
.equ BSRR,      0x18
.equ BRR,       0x28
.equ PC8,       0x100

// SYSCFG control registers
.equ SYSCFG,    0x40010000
.equ EXTICR2,   0x0c

// NVIC control registers
.equ NVIC,      0xe000e000
.equ ISER,      0x100

// External interrupt control registers
.equ EXTI,      0x40010400
.equ IMR,       0
.equ RTSR,      0x8
.equ PR,        0x14

.equ TIM3,      0x40000400
.equ TIMCR1,    0x00
.equ DIER,      0x0c
.equ TIMSR,     0x10
.equ PSC,       0x28
.equ ARR,       0x2c

// Iinterrupt number for EXTI4_15 is 7.
.equ EXTI4_15_IRQn,  7
// Interrupt number for Timer 3 is 16.
.equ TIM3_IRQn,  16

//=====================================================================
// Q1
//=====================================================================
.global subdiv
subdiv:
// r0 = x
	push {r4-r7, lr}
	if1:
		cmp r0, #0
		bne endif1
	then1:
		movs r0, #0
		pop {r4-r7, pc}
	endif1:
	if2:
		movs r1, #1
		ands r1, r0
		cmp r1, #1
		bne endif2
	then2:
		subs r0, #1
		bl subdiv
		adds r0, #1
		pop {r4-r7, pc}
	endif2:
	lsrs r0, r0, #1
	bl subdiv
	adds r0, #1
	pop {r4-r7, pc}

//=====================================================================
// Q2
//=====================================================================
.global enable_porta
enable_porta:
	push {lr}
	ldr r0, =RCC
	ldr r1, [r0, #AHBENR]
	ldr r2, =IOPAEN
	orrs r1, r2
	str r1, [r0, #AHBENR]
	pop {pc}

//=====================================================================
// Q3
//=====================================================================
.global enable_portc
enable_portc:
	push {lr}
	ldr r0, =RCC
	ldr r1, [r0, #AHBENR]
	ldr r2, =IOPCEN
	orrs r1, r2
	str r1, [r0, #AHBENR]
	pop {pc}


//=====================================================================
// Q4
//=====================================================================
.global setup_pa4
setup_pa4:
	ldr r0, =GPIOA
	ldr r1, [r0, #MODER]
	movs r2, #3
	lsls r2, r2, #8
	bics r1, r2
	str r1, [r0, #MODER]

	//pull-up resistor enable
	ldr r1, [r0, #PUPDR]
	ldr r2, =1
	lsls r2, r2, #9
	bics r1, r2
	ldr r2, =1
	lsls r2, r2, #8
	orrs r1, r2
	str r1, [r0, #PUPDR]

	bx lr

//=====================================================================
// Q5
//=====================================================================
.global setup_pa5
setup_pa5:
	// input
	ldr r0, =GPIOA
	ldr r1, [r0, #MODER]
	ldr r2, =3
	lsls r2, r2, #10
	bics r1, r2
	str r1, [r0, #MODER]

	//pull down
	ldr r1, [r0, #PUPDR]
	ldr r2, =1
	lsls r2, r2, #11
	orrs r1, r2
	ldr r2, =1
	lsls r2, r2, #10
	bics r1, r2
	str r1, [r0, #PUPDR]

	bx lr

//=====================================================================
// Q6
//=====================================================================
.global setup_pc8
setup_pc8:
	//output
	ldr r0, = GPIOC
	ldr r1, [r0, #MODER]
	movs r2, #1
	lsls r2, r2, #17
	bics r1, r2
	movs r2, #1
	lsls r2, r2, #16
	orrs r1, r2
	str r1, [r0, #MODER]

	// high speed
	ldr r1, [r0, #OSPEEDR]
	movs r2, #3
	lsls r2, r2, #16
	orrs r1, r2
	str r1, [r0, #OSPEEDR]
	bx lr


//=====================================================================
// Q7
//=====================================================================
.global setup_pc9
setup_pc9:
	// output
	ldr r0, = GPIOC
	ldr r1, [r0, #MODER]
	movs r2, #1
	lsls r2, r2, #19
	bics r1, r2
	movs r2, #1
	lsls r2, r2, #18
	orrs r1, r2
	str r1, [r0, #MODER]

	// medium speed
	ldr r1, [r0, #OSPEEDR]
	movs r2, #1
	lsls r2, r2, #19
	bics r1, r2
	movs r2, #1
	lsls r2, r2, #18
	orrs r1, r2
	str r1, [r0, #OSPEEDR]
	bx lr

//=====================================================================
// Q8
//=====================================================================
.global action8
action8:
	push {r4-r7,lr}
	ldr r0, =GPIOA
	ldr r1, [r0, #IDR] //r1 = IDR
	movs r2, #1 // r2 = mask
	lsls r2, r2, #4
	ands r2, r1 // r2 = IDR4
	lsrs r2, r2, #4

	movs r3, #1 // r3 = mask
	lsls r3, r3, #5
	ands r3, r1
	lsrs r3, r3, #5 // r3 = IDR5

	if3:
	cmp r2, #1
	bne else3

	cmp r3, #0
	bne else3
	then3:
		ldr r0, =GPIOC
		ldr r1, [r0, #ODR]
		movs r2, #1
		lsls r2, #8
		bics r1, r2
		str r1, [r0, #ODR]
		b endif3
	else3:
		ldr r0, =GPIOC
		ldr r1, [r0, #ODR]
		movs r2, #1
		lsls r2, #8
		orrs r1, r2
		str r1, [r0, #ODR]
	endif3:

	pop {r4-r7,pc}

//=====================================================================
// Q9
//=====================================================================
.global action9
action9:
	push {r4-r7,lr}
	ldr r0, =GPIOA
	ldr r1, [r0, #IDR] //r1 = IDR
	movs r2, #1 // r2 = mask
	lsls r2, r2, #4
	ands r2, r1 // r2 = IDR4
	lsrs r2, r2, #4

	movs r3, #1 // r3 = mask
	lsls r3, r3, #5
	ands r3, r1
	lsrs r3, r3, #5 // r3 = IDR5

	if4:
	cmp r2, #0
	bne else4

	cmp r3, #1
	bne else4
	then4:
		ldr r0, =GPIOC
		ldr r1, [r0, #ODR]
		movs r2, #1
		lsls r2, #9
		bics r1, r2
		str r1, [r0, #ODR]
		b endif4
	else4:
		ldr r0, =GPIOC
		ldr r1, [r0, #ODR]
		movs r2, #1
		lsls r2, #9
		orrs r1, r2
		str r1, [r0, #ODR]
	endif4:


//=====================================================================
// Q10
//=====================================================================
.type EXTI4_15_IRQHandler, %function
.global EXTI4_15_IRQHandler
EXTI4_15_IRQHandler:
	ldr r0, =EXTI
	ldr r1, [r0, #PR]
	movs r2, #1
	lsls r2, r2, #4
	orrs r1, r2
	str r1, [r0, #PR]

	ldr r0, =counter
	ldr r1, [r0]
	adds r1, #1
	str r1, [r0]

	bx lr
//=====================================================================
// Q11
//=====================================================================
.global enable_exti4
enable_exti4:
	push {r4-r7, lr}
	//enable the 48 MHz system clock to the SYSCFG subsystem.
	ldr r0, =RCC
	ldr r1, [r0, #APB2ENR]
	ldr r2, =SYSCFGCOMPEN
	orrs r1, r2
	str r1, [r0, #APB2ENR]

	//set up the SYSCFG external interrupt configuration register 2 to use pin PA4 for the interrupt source
	ldr r0, =SYSCFG
	ldr r1, [r0, #EXTICR2]
	movs r2, #15
	bics r1, r2
	str r1, [r0, #EXTICR2]

	//configure the EXTI_RTSR (see the FRM, page 224) to trigger on the rising edge of PA4
	ldr r0, =EXTI
	ldr r1, [r0, #RTSR]
	movs r2, #1
	lsls r2, r2, #4
	orrs r1, r2
	str r1, [r0, #RTSR]

	//set the EXTI_IMR to not ignore pin number 4
	ldr r1, [r0, #IMR]
	movs r2, #1
	lsls r2, r2, #4
	orrs r1, r2
	str r1, [r0, #IMR]

	//configure the NVIC to enable the EXTI4_15_IRQHandler
	ldr r0, =NVIC
	movs r1, #1
	lsls r1, r1, #EXTI4_15_IRQn
	ldr r2, =ISER
	str r1, [r0, r2]

	pop {r4-r7, pc}

//=====================================================================
// Q12
//=====================================================================
.type TIM3_IRQHandler, %function
.global TIM3_IRQHandler
TIM3_IRQHandler:
	ldr r0, =GPIOC
	ldr r1, [r0, #ODR]
	movs r2, #1
	lsls r2, r2, #9
	eors r1, r2
	str r1, [r0, #ODR]

	ldr r0, =TIM3
	ldr r1, [r0, #TIMSR]
	movs r2, #1
	bics r1, r2
	str r1, [r0, #TIMSR]

	bx lr


//=====================================================================
// Q13
//=====================================================================
.global enable_tim3
enable_tim3:

