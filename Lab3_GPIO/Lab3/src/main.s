.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

//===================================================================
// ECE 362 Lab Experiment 3
// General Purpose I/O
//===================================================================

.equ  RCC,      0x40021000
.equ  AHBENR,   0x14
.equ  GPIOCEN,  0x00080000
.equ  GPIOBEN,  0x00040000
.equ  GPIOAEN,  0x00020000
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  GPIOA,    0x48000000
.equ  MODER,    0x00
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  PC0,      0x01
.equ  PC1,      0x04
.equ  PC2,      0x10
.equ  PC3,      0x40
.equ  PIN8,     0x00000100


//===========================================================
// Enable Ports B and C in the RCC AHBENR
// No parameters.
// No expected return value.
.global enable_ports
enable_ports:
    push    {lr}
    // Student code goes here

	// r0 = addr RCC
	// r1 = offset
	// r2 = value hold by register
	// r3 = value to orrs with
	ldr r0, =RCC
	ldr r1, =AHBENR
	ldr r2, [r0, r1]
	ldr r3, =0x000c0000
	orrs r2, r3
	str r2, [r0, r1]

    // End of student code
    pop     {pc}

//===========================================================
// Set bits 0-3 of Port C to be outputs.
// No parameters.
// No expected return value.
.global port_c_output
port_c_output:
    push    {lr}
    // Student code goes here
	// r0 = GPIOCEN
	// r1 = offset
	// r2 = valdue hold by register
	// r3 = orr value
	ldr r0, =GPIOC
	ldr r1, =MODER
	ldr r2, [r0, r1]
	ldr r3, =0x00000055
	orrs r2, r3
	str r2, [r0, r1]


    // End of student code
    pop     {pc}

//===========================================================
// Set the state of a single output pin to be high.
// Do not affect the other bits of the port.
// Parameter 1 is the GPIOx base address.
// Parameter 2 is the bit number of the pin.
// No expected return value.
.global setpin
setpin:
    push    {r4,lr}
    // Student code goes here
	// r0 = base addr of GPIOx
	// r1 = bit number
	// r2 = offset of ODR
	// r3 = value hold by register
	// r4 = bit to orr



	ldr r2, =ODR
	ldr r3, [r0, r2]
	movs r4, #1
	lsls r4, r1
	orrs r3, r4
	str r3, [r0, r2]


    // End of student code
    pop     {r4,pc}

//===========================================================
// Set the state of a single output pin to be low.
// Do not affect the other bits of the port.
// Parameter 1 is the GPIOx base address.
// Parameter 2 is the bit number of the pin.
// No expected return value.
.global clrpin
clrpin:
    push    {r4,lr}
    // Student code goes here
	// r0 = base addr of GPIOx
	// r1 = bit number
	// r2 = offset of ODR
	// r3 = value hold by register
	// r4 = bit to orr
	ldr r2, =ODR
	ldr r3, [r0, r2]
	movs r4, #1
	lsls r4, r1
	bics r3, r4
	str r3, [r0, r2]

    // End of student code
    pop     {r4, pc}

//===========================================================
// Get the state of the input data register of
// the specified GPIO.
// Parameter 1 is GPIOx base address.
// Parameter 2 is the bit number of the pin.
// The subroutine should return 0x1 if the pin is high
// or 0x0 if the pin is low.
.global getpin
getpin:
    push    {lr}
    // Student code goes here
    // r0 = GPIOx base addr
    // r1 = number of bit
    // r2 = value hold by register
    // r3 = bit
	ldr r2, [r0, #IDR]
	movs r3, #1
	lsls r3, r1
	ands r3, r2
	lsrs r3, r1
	movs r0, r3



    // End of student code
    pop     {pc}

//===========================================================
// Get the state of the input data register of
// the specified GPIO.
// Parameter 1 is GPIOx base address.
// Parameter 2 is the direction of the shift
//
// Perfroms the following logic
// 1) Read the current content of GPIOx-ODR
// 2) If R1 = 1
//      (a) Left shift the content by 1
//      (b) Check if value exceeds 8
//      (c) If so set it to 0x1
// 3) If R1 != 0
//      (a) Right shift the content by 1
//      (b) Check if value is 0
//      (c) If so set it to 0x8
// 4) Store the new value in ODR
// No return value
.global seq_led
seq_led:
    push    {r4-r7,lr}
    // Student code goes here
	// r0 = GPIOx addr
	// r1 = direction
	// r6 = GPIOx addr
	// r7 = direction
	movs r6, r0
	movs r7, r1
	ldr r1, [r6, #ODR] // r1 = content hold by odr
	if1:
		cmp r7, #1
		bne else1
	ifbody1:
		lsls r1, r1, #1
		if2:
			cmp r1, #8
			ble endif2
		ifbody2:
			movs r1, #0x1
		endif2:
		b endif1
	else1:
		lsrs r1, r1, #1
		if3:
			cmp r1, #0
			bne endif3
		ifbody3:
			movs r1, #0x8
		endif3:
	endif1:
	str r1, [r6, #ODR]

    // End of student code
    pop     {r4-r7,pc}

.global main
main:
    // Uncomment the line below to test "enable_ports"
    //bl  test_enable_ports

    // Uncomment the line below to test "port_c_output"
    //bl  test_port_c_output

    // Uncomment the line below to test "setpin and clrpin"
    //bl  test_set_clrpin

    // Uncomment the line below to test "getpin"
    //bl  test_getpin

    // Uncomment the line below to test "getpin"
    //bl  test_wiring

    // Uncomment to run the LED sequencing program
    bl run_seq

inf_loop:
    b inf_loop

