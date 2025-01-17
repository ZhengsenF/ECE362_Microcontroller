#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "fifo.h"

#define UNK -1
#define NON_INTR 0
#define INTR 1

int __io_putchar(int ch);
static int putchar_nonirq(int ch);

void prob3(void);
void prob4(void);
void prob5(void);

static struct fifo input_fifo;  // input buffer
static struct fifo output_fifo; // output buffer
int interrupt_mode = UNK;   // which version of putchar/getchar to use.
int echo_mode = 1;          // should we echo input characters?

#define RATE 100000
#define N 1000
short int wavetable[N];

//=============================================================================
// This is a version of printf() that will disable interrupts for the
// USART and write characters directly.  It is intended to handle fatal
// exceptional conditions.
// It's also an example of how to create a variadic function.
static void safe_printf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    char buf[80];
    int len = vsnprintf(buf, sizeof buf, format, ap);
    int saved_txeie = USART1->CR1 & USART_CR1_TXEIE;
    USART1->CR1 &= ~USART_CR1_TXEIE;
    int x;
    for(x=0; x<len; x++) {
        putchar_nonirq(buf[x]);
    }
    USART1->CR1 |= saved_txeie;
    va_end(ap);
}

//=======================================================================
// Simply write a string one char at a time.
//=======================================================================
static void putstr(const char *s) {
    while(*s)
        __io_putchar(*s++);
}

//=======================================================================
// Insert a character and echo it.
// (or, if it's a backspace, remove a char and erase it from the line).
// If echo_mode is turned off, just insert the character and get out.
//=======================================================================
static void insert_echo_char(char ch) {
    if (ch == '\r')
        ch = '\n';
    if (!echo_mode) {
        fifo_insert(&input_fifo, ch);
        return;
    }
    if (ch == '\b' || ch == '\177') {
        if (!fifo_empty(&input_fifo)) {
            char tmp = fifo_uninsert(&input_fifo);
            if (tmp == '\n')
                fifo_insert(&input_fifo, '\n');
            else if (tmp < 32)
                putstr("\b\b  \b\b");
            else
                putstr("\b \b");
        }
        return; // Don't put a backspace into buffer.
    } else if (ch == '\n') {
        __io_putchar('\n');
    } else if (ch == 0){
        putstr("^0");
    } else if (ch == 28) {
        putstr("^\\");
    } else if (ch < 32) {
        __io_putchar('^');
        __io_putchar('A'-1+ch);
    } else {
        __io_putchar(ch);
    }
    fifo_insert(&input_fifo, ch);
}


//-----------------------------------------------------------------------------
// Section 6.2
//-----------------------------------------------------------------------------
// This should should perform the following
// 1) Enable clock to GPIO port A
// 2) Configure PA9 and PA10 to alternate function to use a USART
//    Note: Configure both MODER and AFRL registers
// 3) Enable clock to the USART module, it is up to you to determine
//    which RCC register to use
// 4) Disable the USART module (hint UE bit in CR1)
// 5) Configure USART for 8 bits, 1 stop bit and no parity bit
// 6) Use 16x oversampling
// 7) Configure for 115200 baud rate
// 8) Enable the USART for both transmit and receive
// 9) Enable the USART
// 10) Wait for TEACK and REACK to be set by hardware in the ISR register
// 11) Set the 'interrupt_mode' variable to NON_INTR
void tty_init(void) {
    // Disable buffers for stdio streams.  Otherwise, the first use of
    // each stream will result in a *malloc* of 2K.  Not good.
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);

    // Student code goes here...
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA -> MODER |= GPIO_MODER_MODER9_1;
    GPIOA -> MODER |= GPIO_MODER_MODER10_1;
    GPIOA -> AFR[1] |= 0x11 << 4;

    RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
    USART1 -> CR1 &= ~USART_CR1_UE;
    USART1 -> CR1 &= ~USART_CR1_M; //8 bit
    USART1 -> CR1 &= ~USART_CR1_M << (28 - 12); //8 bit
    USART1 -> CR1 &= ~USART_CR1_PCE; //parity control
    USART1 -> CR2 &= ~USART_CR2_STOP; //stop bit
    USART1 -> CR1 &= ~USART_CR1_OVER8; //16x oversampling
    USART1 -> BRR = 0x1A1;
    USART1 -> CR1 |= USART_CR1_TE;
    USART1 -> CR1 |= USART_CR1_RE;
    USART1 -> CR1 |= USART_CR1_UE;

    while(!(USART1 -> ISR & USART_ISR_TEACK));
    while(!(USART1 -> ISR & USART_ISR_REACK));
    interrupt_mode = NON_INTR;

}

//=======================================================================
// Enable the USART RXNE interrupt.
// Remember to enable the right bit in the NVIC registers
//=======================================================================
void enable_tty_irq(void) {
    // Student code goes here...
	USART1 -> CR1 |= USART_CR1_RXNEIE;
	NVIC -> ISER[0] = 1 << USART1_IRQn;
	interrupt_mode = INTR;
}

//-----------------------------------------------------------------------------
// Section 6.3
//-----------------------------------------------------------------------------
//=======================================================================
// This method should perform the following
// Transmit 'ch' using USART1, remember to wait for transmission register to be
// empty. Also this function must check if 'ch' is a new line character, if so
// it must transmit a carriage return before transmitting 'ch' using USART1.
// Think about this, why must we add a carriage return, what happens otherwise?
//=======================================================================
static int putchar_nonirq(int ch) {
    // Student code goes here...
	if(ch == '\n') {
		while(!(USART1 -> ISR & USART_ISR_TXE));
		USART1 -> TDR = '\r';
	}
	while(!(USART1 -> ISR & USART_ISR_TXE));
	USART1 -> TDR = ch;
	return ch;
}

//-----------------------------------------------------------------------------
// Section 6.4
//-----------------------------------------------------------------------------
// See lab document for description
static int getchar_nonirq(void) {
    // Student code goes here...
	if(USART1 -> ISR & USART_ISR_ORE) {
		USART1 -> ICR |= USART_ICR_ORECF;
	}
	while(!fifo_newline(&input_fifo)) {
		while(!(USART1 -> ISR & USART_ISR_RXNE));
		insert_echo_char(USART1 -> RDR);
	}
	return fifo_remove(&input_fifo);
}

//-----------------------------------------------------------------------------
// Section 6.5
//-----------------------------------------------------------------------------
// See lab document for description
//=======================================================================
// IRQ invoked for USART1 activity.
void USART1_IRQHandler(void) {
    // Student code goes here...
	if(USART1 -> ISR & USART_ISR_RXNE) {
		char new = USART1 -> RDR;
		insert_echo_char(new);
	}
	if(USART1 -> ISR & USART_ISR_TXE) {
		if(fifo_empty(&output_fifo)) {
			USART1 -> CR1 &= ~USART_CR1_TXEIE;
		}
		else {
			USART1 -> TDR = fifo_remove(&output_fifo);
		}
	}

    //-----------------------------------------------------------------
    // Leave this checking code here to make sure nothing bad happens.
    if (USART1->ISR & (USART_ISR_RXNE|
            USART_ISR_ORE|USART_ISR_NE|USART_ISR_FE|USART_ISR_PE)) {
        safe_printf("Problem in USART1_IRQHandler: ISR = 0x%x\n", USART1->ISR);
    }
}

// See lab document for description
static int getchar_irq(void) {
    // Student code goes here...
	while(!fifo_newline(&input_fifo)) {
		asm("wfi");
	}
	return fifo_remove(&input_fifo);
}

// See lab document for description
static int putchar_irq(char ch) {
    // Student code goes here...
	while (fifo_full(&output_fifo)) {
		asm("wfi");
	}
	if(ch == '\n') {
		fifo_insert(&output_fifo, '\r');
	}
	else {
		fifo_insert(&output_fifo, ch);
	}
	if (!(USART1-> CR1 & USART_CR1_TXEIE)) {
		USART1-> CR1 |= USART_CR1_TXEIE;
		USART1_IRQHandler();
	}
	if(ch == '\n') {
		while (fifo_full(&output_fifo)) {
			asm("wfi");
		}
		fifo_insert(&output_fifo, '\n');
	}
	return ch;
}

//=======================================================================
// Called by the Standard Peripheral library for a write()
int __io_putchar(int ch) {
    if (interrupt_mode == INTR)
        return putchar_irq(ch);
    else
        return putchar_nonirq(ch);
}

//=======================================================================
// Called by the Standard Peripheral library for a read()
int __io_getchar(void) {
    // Choose the right implementation.
    if (interrupt_mode == INTR)
        return getchar_irq();
    else
        return getchar_nonirq();
}

//-----------------------------------------------------------------------------
// Section 6.6
//-----------------------------------------------------------------------------
//===========================================================================
// This function
// 1) enables clock to port A,
// 2) sets PA4 to analog mode
void setup_gpio() {
    // Student code goes here...
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA -> MODER &= ~GPIO_MODER_MODER4;
	GPIOA -> MODER |= GPIO_MODER_MODER4_1;
}

// This function should enable the clock to the
// onboard DAC, enable trigger,
// setup software trigger and finally enable the DAC.
void setup_dac() {
    // Student code goes here...
	RCC -> APB1ENR |= RCC_APB1ENR_DACEN;
	DAC -> CR &= ~DAC_CR_EN1;
	DAC -> CR &= ~DAC_CR_BOFF1;
	DAC -> CR |= DAC_CR_TEN1;
	DAC -> CR |= DAC_CR_TSEL1;
	DAC -> CR |= DAC_CR_EN1;
}

// This function should,
// enable clock to timer6,
// setup pre scalar and arr so that the interrupt is triggered every
// 10us, enable the timer 6 interrupt, and start the timer.
void setup_timer6() {
    // Student code goes here...
	RCC -> APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6 -> PSC = 24 - 1;
	TIM6 -> ARR = 20 - 1;
	TIM6 -> DIER |= TIM_DIER_UIE;
	NVIC -> ISER[0] = 1 << TIM6_DAC_IRQn;
	TIM6 -> CR1 |= TIM_CR1_CEN;
}


int offset = 0;
int step = 698 * N / 100000.0 * (1 << 16);
void TIM6_DAC_IRQHandler() {
    // Student code goes here...
	TIM6 -> SR &= ~TIM_SR_UIF;
	DAC -> SWTRIGR |= DAC_SWTRIGR_SWTRIG1;

	offset += step;
	if((offset >> 16) >= N)
		offset -= N << 16;
	int sample = wavetable[offset >> 16];

	DAC -> DHR12R1 = sample;
}

void init_wavetable(void)
{
    // Student code goes here...
	int x;
	for(x=0; x<N; x++){
		wavetable[x] = 32767 * sin(2 * M_PI * x / N);
	}
}

// Act on a command read by testbench().
static void action(char **words) {
    if (words[0] != 0) {
        if (strcasecmp(words[0],"alpha") == 0) {
            // Print the alphabet repeatedly until you press <Enter>.
            char buf[81];
            for(int x=0; x<80; x++)
                buf[x] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[x%26];
            buf[80] = '\0';
            echo_mode = 0;
            for(;;) {
                putstr(buf);
                if (fifo_newline(&input_fifo)) {
                    echo_mode = 1;
                    return;
                }
            }
        }
        if (strcasecmp(words[0],"init") == 0) {
            if (strcasecmp(words[1],"lcd") == 0) {
                printf("lcd command not implemenented yet\n");
                return;
            }
            if (strcasecmp(words[1],"dac") == 0) {
				printf("initialize dac\n");
				setup_gpio();
				setup_timer6();
				setup_dac();
				init_wavetable();
				return;
			}
        }
        if (strcasecmp(words[0],"display1") == 0) {
            printf("display1 command not implemented yet\n");
            return;
        }
        if (strcasecmp(words[0],"display2") == 0) {
            printf("display2 command not implemented yet\n");
            return;
        }
        if (strcasecmp(words[0],"display2") == 0) {
            printf("display2 command not implemented yet\n");
            return;
        }
        if (strcasecmp(words[0],"gen") == 0) {
			printf("generate wave with frequency %s\n", words[1]);
			step = atoi(words[1]) * N / 100000.0 * (1 << 16);
//			printf("%d\n", step);
			return;
		}

        printf("Unrecognized command: %s\n", words[0]);
    }
}

//===========================================================================
// Interact with the hardware.
// This subroutine waits for a line of input, breaks it apart into an
// array of words, and passes that array of words to the action()
// subroutine.
// The "display1" and "display2" are special words that tell it to
// keep everything after the first space together into words[1].
//
void testbench(void) {
    printf("STM32 testbench.\n");
    for(;;) {
        char buf[60];
        printf("> ");
        fgets(buf, sizeof buf - 1, stdin);
        int sz = strlen(buf);
        if (sz > 0)
            buf[sz-1] = '\0';
        char *words[7] = { 0,0,0,0,0,0,0 };
        int i;
        char *cp = buf;
        for(i=0; i<6; i++) {
            // strtok tokenizes a string, splitting it up into words that
            // are divided by any characters in the second argument.
            words[i] = strtok(cp," \t");
            // Once strtok() is initialized with the buffer,
            // subsequent calls should be made with NULL.
            cp = 0;
            if (words[i] == 0)
                break;
            if (i==0 && strcasecmp(words[0], "display1") == 0) {
                words[1] = strtok(cp, ""); // words[1] is rest of string
                break;
            }
            if (i==0 && strcasecmp(words[0], "display2") == 0) {
                words[1] = strtok(cp, ""); // words[1] is rest of string
                break;
            }
        }
        action(words);
    }
}


int main(void)
{
    tty_init();
//    prob3();
//    prob4();
//    prob5();
    testbench();

    for(;;)
        asm("wfi");
}

