.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

//============================================================================
// Q1: hello
//============================================================================
.global hello
hello:
	push {lr}
	ldr r0, =hello_string
	bl printf
	pop {pc}

hello_string:
	.string "Hello, World!\n"
	.align 2
//============================================================================
// Q2: add2
//============================================================================
.global add2
add2:
	// r0 = a
	// r1 = b
	push {lr}
	adds r3, r0, r1
	movs r2, r1
	movs r1, r0
	ldr r0, =format1
	bl printf

	pop {pc}

format1:
	.string "%d + %d = %d\n"
	.align 2
//============================================================================
// Q3: add3
//============================================================================
.global add3
add3:
	// r0 = a
	// r1 = b
	// r2 = c
	push {r4-r7, lr}
	movs r3, r2
	movs r2, r1
	movs r1, r0
	adds r4, r1, r2
	adds r4, r3
	ldr r0, =format2

	sub sp, #4
	str r4, [sp, #0]

	bl printf

	add sp, #4
	pop {r4-r7, pc}
format2:
 .string "%d + %d + %d = %d\n"
 .align 2

//============================================================================
// Q4: rotate6
//============================================================================
.global rotate6
rotate6:
// r0 = a
// r1 = b
// r2 = c
// r3 = d
// 20 = e
// 24 = f
	push {r4-r7, lr}
	if1:
		cmp r0, #0
		beq endif1
	then1:
		ldr r4, [sp, #20] // r4 = e
		ldr r5, [sp, #24] // r5 = f
		//rotate
		movs r6, r5 // r6 = f
		movs r5, r4 // r5 = e
		movs r4, r3 // r4 = d
		movs r3, r2 // r3 = c
		movs r2, r1 // r2 = b
		movs r1, r0 // r1 = a
		movs r0, r6 // r0 = f
		sub sp, #8
		str r4, [sp, #0]
		str r5, [sp, #4]
		bl rotate6
		add sp, #8
		pop {r4-r7, pc}
	endif1:
	ldr r4, [sp, #20] // r4 = e
	ldr r5, [sp, #24] // r5 = f
	subs r5, r4
	subs r5, r3
	subs r5, r2
	subs r5, r1
	subs r5, r0
	movs r0, r5
	pop {r4-r7, pc}


//============================================================================
// Q5: low_pattern
//============================================================================
.type compare, %function  // You knew you needed this line.  Of course you did!
compare:
        ldr  r0,[r0]
        ldr  r1,[r1]
        subs r0,r1
        bx lr

.global low_pattern
low_pattern:
	// r0 = nth
	// r1 = beginning of the arr
	// r2 = x
	push {r4-r7, lr}
	sub sp, #400 // allocate 100 int
	sub sp, #400 // allocate 100 int
	mov r1, sp

	for1:
		movs r2, #0
	check1:
		cmp r2, #200
		bge endfor1
	forbody1:
		movs r3, r2
		adds r3, #1 // r3 = x + 1
		ldr r4, =255
		muls r3, r4 // r3 = (x+1)*255
		ldr r4, =0xff
		ands r3, r4 // & 0xff
		movs r4, r2
		lsls r4, r4, #2 // index offset
		str r3, [r1, r4]

	next1:
		adds r2, #1
		b check1
	endfor1:
	movs r4, r0 //r4 = nth
	movs r5, r1 // r5 = arr addr
	movs r0, r1
	ldr r1, =200
	movs r2, #4
	ldr r3, =compare
	bl qsort
	lsls r4, r4, #2
	ldr r0, [r5, r4]
	ldr r1, =800
	add sp, r1 // deallocate
	pop {r4-r7, pc}

//============================================================================
// Q6: get_name
//============================================================================
.global get_name
get_name:
	// r4 = buffer addr
	push {r4-r7,lr}
	sub sp, #100 //allocate 100 char
	mov r4, sp
	ldr r0, =get_name_string1
	bl printf
	movs r0, r4
	bl gets
	movs r1, r4
	ldr r0, =get_name_string2
	bl printf

	add sp, #100 //deallocate
	pop {r4-r7, pc}
get_name_string1:
	.string "Enter your name: "
	.align 2

get_name_string2:
	.string "Hello, %s\n"
	.align 2
//============================================================================
// Q7: random_sum
//============================================================================
.global random_sum
random_sum:
	// r4 = arr
	// r5 = x
	// r6 = sum
	push {r4-r7, lr}
	sub sp, #80
	mov r4, sp
	movs r6, #0
	bl random
	str r0, [r4]

	for2:
		movs r5, #1
	check2:
		cmp r5, #20
		bge endfor2
	forbody2:
		bl random // r0 = random()
		movs r1, r5 // r1 = x
		subs r1, #1 // x-1
		lsls r1, r1, #2
		ldr r2, [r4, r1] // r2 = arr[x-1]
		subs r2, r0 // r2 = arr[x-1] - random()
		movs r1, r5 // r1 = x
		lsls r1, r1, #2
		str r2, [r4, r1]
	next2:
		adds r5, #1
		b check2
	endfor2:


	for3:
		movs r5, #0
	check3:
		cmp r5, #20
		bge endfor3
	forbody3:
		movs r1, r5
		lsls r1, r1, #2
		ldr r3, [r4, r1]
		adds r6, r3
	next3:
		adds r5, #1
		b check3
	endfor3:
	movs r0, r6

	add sp, #80
	pop {r4-r7, pc}


//============================================================================
// Q8: fibn
//============================================================================
.global fibn
fibn:
	// r4 = arr addr
	// r5 = x
	// r6 = n
	push {r4-r7, lr}
	movs r6, r0
	sub sp, #480
	mov r4, sp
	movs r0, #0
	str r0, [r4]
	movs r0, #1
	str r0, [r4, #4]
	for4:
		movs r5, #2
	check4:
		ldr r0, =120
		cmp r5, r0
		bge endfor4
	forbody4:
		movs r0, r5 // r0 = x
		subs r0, #1
		lsls r0, r0, #2
		ldr r1, [r4, r0] // r1 = arr[x-1]
		movs r0, r5 // r0 = x
		subs r0, #2
		lsls r0, r0, #2
		ldr r2, [r4, r0] // r1 = arr[x-2]
		adds r1, r2 // r1 = arr[x-1] + arr[x-2]
		movs r0, r5
		lsls r0, r0, #2
		str r1, [r4, r0]
	next4:
		adds r5, #1
		b check4
	endfor4:
	lsls r6, r6, #2
	ldr r0, [r4, r6]

	add sp, #480
	pop {r4-r7, pc}

//============================================================================
// Q9: fun
//============================================================================
.global fun
fun:
	// r0 = a
	// r1 = b
	// r2 = arr addr
	// r3 = x
	// r4 = sum

	push {r4-r7, lr}
	sub sp, #400
	mov r2, sp

	movs r5, #0
	str r5, [r2]

	for5:
		movs r3, #1
	check5:
		cmp r3, #100
		bge endfor5
	forbody5:
		movs r5, r3
		subs r5, #1 // x-1
		lsls r5, r5, #2
		ldr r6, [r2, r5] // r6 = arr[x-1]
		movs r4, r3
		adds r4, #7
		movs r5, #37
		muls r4, r5 // r4 = 37 * (x + 7)
		adds r6, r4

		movs r5, r3
		lsls r5, r5, #2
		str r6, [r2, r5]
	next5:
		adds r3, #1
		b check5
	endfor5:

	movs r4, #0

	for6:
		movs r3, r0
	check6:
		cmp r3, r1
		bgt endfor6
	forbody6:
		movs r5,r3
		lsls r5, r5, #2
		ldr r0, [r2, r5]
		adds r4, r0
	next6:
		adds r3, #1
		b check6
	endfor6:

	movs r0, r4
	add sp, #400
	pop {r4-r7, pc}

//============================================================================
// Q10: sick
//============================================================================
.global sick
sick:
	// input
	// r0 = start
	// r1 = end
	// r2 = add
	// r3 = mul
	push {r4-r7, lr}
	ldr r4, [sp, #20]
	ldr r5, =step
	str r4, [r5]
	ldr r5, =start
	str r0, [r5]
	ldr r5, =end
	str r1, [r5]

	// r0 = x
	// r2 = add
	// r3 = mul
	// r4 = arr[]
	sub sp, #400
	mov r4, sp

	movs r5, #0
	str r5, [r4]
	for7:
		movs r0, #1
	check7:
		cmp r0, #100
		bge endfor7
	forbody7:
		movs r1, r0
		subs r1, #1
		lsls r1, r1, #2
		ldr r1, [r4, r1] // r1 = arr[x-1]

		movs r5, r0
		adds r5, r2
		muls r5, r3 // r5 = (x + add) * mul
		adds r1, r5

		//store back
		movs r5, r0
		lsls r5, r5, #2
		str r1, [r4, r5]
	next7:
		adds r0, #1
		b check7
	endfor7:

	// r0 = x
	// r1 = end
	// r2 = step
	// r3 = sum
	// r4 = arr[]

	ldr r5, =start
	ldr r5, [r5]
	ldr r1, =end
	ldr r1, [r1]
	ldr r2, =step
	ldr r2, [r2]
	movs r3, #0
	for8:
		movs r0, r5
	check8:
		cmp r0, r1
		bgt endfor8
	forbody8:
		movs r5, r0
		lsls r5, r5, #2
		ldr r5, [r4, r5]
		adds r3, r5
	next8:
		adds r0, r2
		b check8
	endfor8:

	movs r0, r3
	add sp, #400
	pop {r4-r7, pc}

.data
start: .word 2
end: .word 2
step: .word 2
