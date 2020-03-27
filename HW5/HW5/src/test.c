#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

// This is used by the interrupt service routine.
volatile int counter = 0;

int euclidean(int, int);
void enable_porta(void);
void enable_portc(void);
void setup_pa4(void);
void setup_pa5(void);
void setup_pc8(void);
void setup_pc9(void);
void action8(void);
void action9(void);
void EXTI4_15_IRQHandler(void);
void enable_exti4(void);
void enable_tim3(void);

//====================================================================
// The fail subroutine is invoked when a test fails.
// The test variable indicates which test failed.
// Use the debugger to walk back up the stack to find
// the point at which fail() was called.
//====================================================================
void fail(int test) {
    for(;;)
        asm("bkpt");
}

//====================================================================
// This will try out all the subroutines you wrote in hw5.s.
//====================================================================
int main(void)
{
    int x;

    // Q1: subdiv
    x = subdiv(10);    // 1
    if (x != 5)
        fail(1);
    x = subdiv(35); // 3
    if (x != 8)
        fail(1);
    x = subdiv(49); // 5
    if (x != 8)
        fail(1);
    x = subdiv(255); // 4
    if (x != 15)
        fail(1);

    // Q2: enable_porta
    RCC->AHBENR &= ~RCC_AHBENR_GPIOAEN;
    enable_porta();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            RCC_AHBENR_GPIOAEN)
        fail(2);
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    enable_porta();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOCEN))
        fail(2);

    // Q3: enable_portc
    RCC->AHBENR &= ~RCC_AHBENR_GPIOAEN;
    RCC->AHBENR &= ~RCC_AHBENR_GPIOCEN;
    enable_portc();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            RCC_AHBENR_GPIOCEN)
        fail(3);
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    enable_portc();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOCEN))
        fail(3);

    // Q4: setup_pa4
    GPIOA->MODER |= 3<<(2*4);  // Sometimes, we deliberately mess with the initial values
    GPIOA->PUPDR |= 3<<(2*4);  // so that you get practice fully clearing/setting them.
    setup_pa4();
    if (GPIOA->MODER != 0x28000000)
        fail(4);
    if (GPIOA->PUPDR != 0x24000100)
        fail(4);
    GPIOA->MODER &= ~(3<<(2*4));
    GPIOA->PUPDR &= ~(3<<(2*4));
    setup_pa4();
    if (GPIOA->MODER != 0x28000000)
        fail(4);
    if (GPIOA->PUPDR != 0x24000100)
        fail(4);

    // Q5: setup_pa5
    GPIOA->MODER |= 3<<(2*5);  // Again, messing up these values.
    GPIOA->PUPDR |= 3<<(2*5);
    setup_pa5();
    if (GPIOA->MODER != 0x28000000)
        fail(5);
    if (GPIOA->PUPDR != 0x24000900)
        fail(5);
    GPIOA->MODER &= ~(3<<(2*5));
    GPIOA->PUPDR &= ~(3<<(2*5));
    setup_pa5();
    if (GPIOA->MODER != 0x28000000)
        fail(5);
    if (GPIOA->PUPDR != 0x24000900)
        fail(5);

    // Q6: setup_pc8
    GPIOC->MODER   |= 3<<(2*8);
    GPIOC->OSPEEDR |= 3<<(2*8);
    GPIOC->MODER |= 1;
    GPIOC->OSPEEDR |= 1;
    int old_moder = GPIOC->MODER;
    int old_ospeedr = GPIOC->OSPEEDR;
    int new_moder = (old_moder & ~(3<<(2*8))) | (1<<2*8);
    int new_ospeedr = old_ospeedr | (3<<2*8);
    setup_pc8();
    if (GPIOC->MODER != new_moder)
        fail(6);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(6);
    GPIOC->MODER   &= ~(3<<(2*8));
    GPIOC->OSPEEDR &= ~(3<<(2*8));
    GPIOC->MODER &= ~1;
    GPIOC->OSPEEDR &= ~1;
    old_moder = GPIOC->MODER;
    old_ospeedr = GPIOC->OSPEEDR;
    new_moder = (old_moder & ~(3<<(2*8))) | (1<<2*8);
    new_ospeedr = old_ospeedr | (3<<2*8);
    setup_pc8();
    if (GPIOC->MODER != new_moder)
        fail(6);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(6);

    new_moder |= 1<<(2*9);
    new_ospeedr |= 1<<(2*9);
    setup_pc9();
    if (GPIOC->MODER != new_moder)
        fail(7);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(7);
    GPIOC->MODER |= 3<<(2*9);
    GPIOC->OSPEEDR |= 3<<(2*9);
    setup_pc9();
    if (GPIOC->MODER != new_moder)
        fail(7);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(7);

    // Q8: action8
    // You did all that work to configure PA4 and PA5 as inputs, and now
    // we're just going to reconfigure them as outputs just so we can test
    // action8 easily.  Remember that you can still read from the IDR when
    // the pin is configured as output.  Here, the action8 subroutine will
    // read the value that we write to the ODR.  This is our basic means of
    // evaluating the GPIO pins.
    GPIOA->MODER &= ~(3<<(2*4)) & ~(3<<(2*5));
    GPIOA->MODER |= (1<<(2*4)) + (1<<(2*5));
    for(x=0; x<4; x++) {
        GPIOA->BRR = 0x3<<4;
        GPIOA->BSRR = x<<4;
        const static int check[] = { 1, 0, 1, 1 };
        action8();
        if (((GPIOC->IDR >> 8) & 1) != check[x])
            fail(8);
    }

    // Q9: action9
    for(x=0; x<4; x++) {
        GPIOA->BRR = 0x3<<4;
        GPIOA->BSRR = x<<4;
        const static int check[] = { 1, 1, 0, 1 };
        action9();
        if (((GPIOC->IDR >> 9) & 1) != check[x])
            fail(9);
    }

    // Reset PA4 and PA5 to be inputs before anyone notices.
    setup_pa4();
    setup_pa5();

    // Q10: EXTI4_15_IRQHandler
    counter = 6;
    EXTI4_15_IRQHandler();
    if (counter != 7)
        fail(10);

    // Q11: enable_exti4
    enable_exti4();
    if ((RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN) == 0)
        fail(11);
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[1] |= 0xf<<(4*(4-4));
    EXTI->RTSR &= ~(1<<4);
    EXTI->IMR &= ~(1<<4);
    NVIC->ICER[0] = 1<<EXTI4_15_IRQn;
    enable_exti4();
    if ((SYSCFG->EXTICR[1] & 0xf) != 0)
        fail(11);
    if (EXTI->RTSR != 1<<4)
        fail(11);
    if (EXTI->IMR != 0x0f940000 + (1<<4))
        fail(11);
    if (!(NVIC->ISER[0] & (1<<EXTI4_15_IRQn)))
        fail(11);
    counter = 0;
    // Try it: Post a pending interrupt for EXTI4.
    NVIC->ISPR[0] = 1<<EXTI4_15_IRQn;
    for(x=0; x<100; x++)
      GPIOC->BSRR = 0; // stall for a while to make sure ISR runs.
    if (counter != 1)
        fail(11);

    // Q12:
    GPIOC->BRR = 1<<9;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    NVIC->ISER[0] = 1<<TIM3_IRQn;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->EGR |= TIM_EGR_UG;
    for(x=0; x<100; x++)
        GPIOC->BSRR = 0; // stall for a while to make sure ISR runs.
    if ((GPIOC->ODR & (1<<9)) == 0)
        fail(12);
    GPIOC->BSRR = 1<<9;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->EGR |= TIM_EGR_UG;
    for(x=0; x<100; x++)
        GPIOC->BSRR = 0; // stall for a while to make sure ISR runs.
    if ((GPIOC->ODR & (1<<9)) != 0)
        fail(12);

    // Q13:
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->ARR = 1;
    TIM3->PSC = 1;
    TIM3->DIER = 0;
    NVIC->ICER[0] = 1<<TIM3_IRQn;
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
    enable_tim3();
    if ((RCC->APB1ENR & RCC_APB1ENR_TIM3EN) == 0)
        fail(13);
    if (TIM3->ARR == 0)
        fail(13);
    if ((TIM3->ARR+1)*(TIM3->PSC+1) != 24000000)
        fail(13);
    if ((TIM3->DIER & TIM_DIER_UIE) == 0)
        fail(13);
    if ((TIM3->CR1 & TIM_CR1_CEN) == 0)
        fail(13);
    if ((NVIC->ISER[0] & (1<<TIM3_IRQn)) == 0)
        fail(13);

    // Total success!
    // If you made it this far through the tests, the
    // subroutines you wrote in hw5.s are working properly.
    for(;;)
        asm("bkpt"); // Success.

    return 0;
}

