#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "fifo.h"

#define FAIL -1
#define SUCCESS 0
#define WR 0
#define RD 1

void serial_init(void);
void test_prob3(void);
void check_config(void);
void test_wiring(void);
void micro_wait(unsigned int);
uint8_t read_EEPROM(uint16_t);

//===========================================================================
// Check wait for the bus to be idle.
void I2C1_waitidle(void) {
    while ((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);  // while busy, wait.
}

int I2C1_checknack(void) {
    if (I2C1->ISR & I2C_ISR_NACKF)
        return 1;
    return 0;
}

void I2C1_clearnack(void) {
    I2C1->ICR |= I2C_ICR_NACKCF;
}

//===========================================================================
// Subroutines for step 2.
//===========================================================================
// Initialize I2C1
/*
1.  Enable clock to GPIOB
2.  Configure PB6 and PB7 to alternate functions I2C1_SCL and I2C1_SDA
3.  Enable clock to I2C1
4.  Set I2C1 to 7 bit mode
5.  Enable NACK generation for I2C1
6.  Configure the I2C1 timing register so that PSC is 6, SCLDEL is 3 and SDADEL is 1 and SCLH is 3 and SCLL is 1
7.  Disable own address1 and own address 2 and set the 7 bit own address to 1
8.  Enable I2C1
 */
void init_I2C1() {
    // Student code goes here
	RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;

	GPIOB -> MODER |= GPIO_MODER_MODER6_1;
	GPIOB -> MODER |= GPIO_MODER_MODER7_1;
	GPIOB -> AFR[0] |= 1 << (4 * 6) | 1 << (4 * 7);

	RCC -> APB1ENR |= RCC_APB1ENR_I2C1EN;

	I2C1 -> CR1 &= ~I2C_CR1_PE;
	I2C1 -> CR2 &= ~I2C_CR2_ADD10; // 7 bit
	I2C1 -> CR2 |= I2C_CR2_NACK;

	I2C1 -> TIMINGR = 0;
	I2C1 -> TIMINGR |= 4 << 28;
	I2C1 -> TIMINGR |= 3 << 20;
	I2C1 -> TIMINGR |= 1 << 16;
	I2C1 -> TIMINGR |= 3 << 8;
	I2C1 -> TIMINGR |= 9;

	I2C1 -> OAR1 &= ~I2C_OAR1_OA1EN;
	I2C1 -> OAR1  = I2C_OAR1_OA1EN | 0x2;
	I2C1 -> OAR2 &= ~I2C_OAR2_OA2EN;

	I2C1 -> CR1 |= I2C_CR1_PE;

    //---------End-----------
}


//===========================================================================
// Subroutines for step 3.
//===========================================================================
// See lab document for description
void I2C1_start(uint8_t addr, uint32_t dir) {
    // Student code goes here
	I2C1 -> CR2 &= ~I2C_CR2_SADD;
	I2C1 -> CR2 |= addr  << 1;

	if(dir == RD) {
		I2C1 -> CR2 |= I2C_CR2_RD_WRN;
	} else {
		I2C1 -> CR2 &= ~I2C_CR2_RD_WRN;
	}

	I2C1 -> CR2 |= I2C_CR2_START;
    //---------End-----------
}

// See lab document for description
void I2C1_stop() {
    // Student code goes here
	if(I2C1 -> ISR & I2C_ISR_STOPF) {
		return;
	}
	I2C1 -> CR2 |= I2C_CR2_STOP;
	while(!(I2C1 -> ISR & I2C_ISR_STOPF));
	I2C1 -> ICR = I2C_ICR_STOPCF;
    //---------End-----------
}

// See lab document for description
int I2C1_senddata(uint8_t* data, uint32_t size) {
    // Student code goes here
	I2C1 -> CR2 &= ~ I2C_CR2_NBYTES;
	I2C1 -> CR2 |= (size << 16) & I2C_CR2_NBYTES;

	int timeout;
	for(int lcv = 0; lcv < size; lcv++) {
		timeout = 0;
		while(!(I2C1 -> ISR & I2C_ISR_TXIS)){
			timeout++;
			if(timeout > 1000000) {
				return FAIL;
			}
		}
		I2C1 -> TXDR = data[lcv];
	}
	while(!((I2C1 -> ISR & I2C_ISR_TC) || (I2C1 -> ISR & I2C_ISR_NACKF)));
	if(I2C1 -> ISR & I2C_ISR_NACKF) {
		return FAIL;
	}
	return SUCCESS;
    //---------End-----------
}

// See lab document for description
int I2C1_readdata(int8_t* data, uint32_t size) {
    // Student code goes here
	I2C1 -> CR2 &= ~ I2C_CR2_NBYTES;
	I2C1 -> CR2 |= size << 16;
	int timeout;

	for(int lcv = 0; lcv < size; lcv++) {
		timeout = 0;
		while(!(I2C1 -> ISR & I2C_ISR_RXNE)){
			timeout++;
			if(timeout > 1000000) {
				return FAIL;
			}
		}
		data[lcv] = I2C1 -> RXDR;
	}
	while(!((I2C1 -> ISR & I2C_ISR_TC) || (I2C1 -> ISR & I2C_ISR_NACKF)));
	return SUCCESS;
    //---------End-----------
}

//===========================================================================
// Subroutines for step 4.
//===========================================================================
// See lab document for description
void write_EEPROM(uint16_t wr_addr, uint8_t data) {
    // Student code goes here
	uint8_t write_buf[3] = {wr_addr >> 8, wr_addr, data};
	I2C1_waitidle();
	I2C1_start(0b1010000, WR);
	I2C1_senddata(write_buf, 3);
	I2C1_stop();

	micro_wait(5);

    //---------End-----------
}

//===========================================================================
// Subroutines for step 5.
//===========================================================================
// See lab document for description
void set_gpio(int pin) {
    // Student code goes here
	//IODIR
	uint8_t write_buf[2] = {0x00, 0b1 << pin};
	I2C1_waitidle();
	I2C1_start(0b0100000, WR);
	I2C1_senddata(write_buf, 2);
	I2C1_stop();

	// GPIO
	write_buf[0] = 0x00;
	write_buf[1] = 0b1 << pin;
	I2C1_waitidle();
	I2C1_start(0b0100000, WR);
	I2C1_senddata(write_buf, 2);
	I2C1_stop();
    //---------End-----------
}

// See lab document for description
void clear_gpio(int pin) {
    // Student code goes here
	uint8_t write_buf[2] = {0x00, 0};
		I2C1_waitidle();
		I2C1_start(0b0100000, WR);
		I2C1_senddata(write_buf, 2);
		I2C1_stop();

		// GPIO
		write_buf[0] = 0x00;
		write_buf[1] = 0;
		I2C1_waitidle();
		I2C1_start(0b0100000, WR);
		I2C1_senddata(write_buf, 2);
		I2C1_stop();
    //---------End-----------
}

//===========================================================================
// End of student code.
//===========================================================================

void prob2() {
    init_I2C1();
    check_config();
}

void prob3() {
    test_prob3();
}

int eeprom_addr = 0x00;

// Act on a command read by prob4().
static void action(char **words) {
    if (words[0] != 0) {
        if (strcasecmp(words[0],"mv") == 0) {
            eeprom_addr = strtol(words[1], NULL, 16);
            printf("Address 0x%x\n", eeprom_addr);
            return;
        }
        if (strcasecmp(words[0],"rd") == 0) {
        	printf("Read value is: 0x%x\n", read_EEPROM(eeprom_addr));
            return;
        }
        if (strcasecmp(words[0],"wr") == 0) {
			int data = strtol(words[1], NULL, 16);
			write_EEPROM(eeprom_addr, data);
			printf("Cursor at 0x%x\n", eeprom_addr);
			printf("Writing successful\n");
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
void prob4(void) {
	init_I2C1();
    printf("\nSTM32 EEPROM testbench.\n");
    printf("Interactive EEPROM read write tool\n");
    printf("- mv 0x\"address\", to move the cursor to move the cursor to address(hex)\n");
    printf("- rd to read data at cursor address\n");
    printf("- wr 0x\"data\" to write data at cursor\n");
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
            if (i==0 && strcasecmp(words[0], "mv") == 0) {
                words[1] = strtok(cp, ""); // words[1] is rest of string
                break;
            }
            if (i==0 && strcasecmp(words[0], "rd") == 0) {
                break;
            }
            if (i==0 && strcasecmp(words[0], "wr") == 0) {
                words[1] = strtok(cp, ""); // words[1] is rest of string
                break;
            }
        }
        action(words);
    }
}

void prob5() {
	init_I2C1();
	while(1) {
		set_gpio(0);
		micro_wait(1000000);
		clear_gpio(0);
		micro_wait(1000000);
	}
}

int main(void)
{
	serial_init();

	//Open the serial terminal to see messages for each part
//    test_wiring();
//    prob2();
//    prob3();
//    prob4();
    prob5();
	while(1)
	    ;
}
