#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h> // for random()
#define MESSAGE_START 63
#define BIKER_POS 20 // position of biker

// The characters to be shown and their colors.
char scr[80][24];
char color[80][24];

// Data variables for the game...
int enter_name;
int highest_score = 0;
char highest_name[11] = "          ";
enum { SPLASH , RUNNING } phase = SPLASH;
enum { IDLE , JUMP1, JUMP2, JUMP3, JUMP4, JUMP5, JUMP6, JUMP7, JUMP8, JUMP9, JUMP10, JUMP11, JUMP12, JUMP13 } jumpState = IDLE;
int splash_ticks;
int score;
int probability; // probability that a stone will appear 0 - 100 %
int foot_height;

struct {
	int pointer;
	int pos[10];
} stone_queue;

void enque() {
	stone_queue.pos[stone_queue.pointer++] = 79;
}

void deque() {
	stone_queue.pointer--;
	for(int i = 0; i < stone_queue.pointer; i++) {
		stone_queue.pos[i] = stone_queue.pos[i + 1];
	}
}



void init(void);
void splash(void);
void animate(void);

void freeze(void);
int  get_seed(void);



// Print a message at screen coordinate x,y in color c.
void msg(int x, int y, int c, const char *s)
{
	int n;
	int len = strlen(s);
	for(n=0; n<len && n+x<80; n++) {
		color[x+n][y] = c;
		scr[x+n][y] = s[n];
	}
}

// Check two points, (x1,y1) and (x2,y2) and return a 1
// if they are within 5 character cells of each other.
int tooclose(int x1, int y1, int x2, int y2)
{
	int x = x1 - x2;
	int y = y1 - y2;
	if (x*x + y*y < 25)
		return 1;
	return 0;
}



void updateScore() {
	score++;
	char buf[100];
	sprintf(buf, "score:%9d", score);
	msg(MESSAGE_START,0,0xe0, buf);
}

// Indicate a collision with a big red X.
// Also display a "Game over" statement.
void collision()
{
	msg(BIKER_POS,foot_height,0x90, "X");
	msg(BIKER_POS-1,foot_height-1,0x90, "\\");
	msg(BIKER_POS+1,foot_height-1,0x90, "/");
	msg(BIKER_POS+1,foot_height+1,0x90, "\\");
	msg(BIKER_POS-1,foot_height+1,0x90, "/");
	if(score > highest_score) {
		enter_name = 1;
		highest_score = score;
	}

	msg(30,10, 0x0f, "   Game over   ");
	if(!enter_name) {
		msg(30,11, 0x0f," Press any key ");

	}

}

void update_high() {
	char buf[11];
	char get;
	int index = -1;
	int line = 11;
	msg(20,line, 0x0f,"please enter your name:");
	msg(43, line, 0xf, "          ");
	render();
	while(get != '\n') {
		get = getchar();
		if(get != 127 && get != '\0' && get != '!'){
			if(index < 10) {
				buf[++index] = get;
			}
		} else if(get == 127) {
			if (index != -1) {
				index--;
			}
		}
		for(int lcv = index + 1; lcv < 10; lcv++) {
			buf[lcv] = ' ';
		}
		buf[10] = '\0';
		msg(43, line, 0xf, buf);
		render();
	}
	for(int lcv = 0; lcv < 11; lcv++) {
		highest_name[lcv] = buf[lcv];
	}

	msg(MESSAGE_START, 2, 0xe0, "Highest scores:");
	msg(MESSAGE_START, 3, 0xe0, highest_name);
	sprintf(buf, "  %d", score);
	msg(MESSAGE_START, 4, 0xe0, buf);
	msg(30,++line, 0x0f, " Press any key ");
	render();
}

void drawBiker(int foot, int height) {
	msg(foot - 7, height, 0xa0, "(_)/(_)" );
	msg(foot - 7, height - 1, 0xa0, " _ \\<_ " );
	msg(foot - 7, height - 2, 0xa0, "   __o " );
}

void clearBiker(int foot, int height) {
	msg(foot - 7, height, 0xa0, "       " );
	msg(foot - 7, height - 1, 0xa0, "       " );
	msg(foot - 7, height - 2, 0xa0, "       " );
}


void drawSky() {
	int line = 1;
	msg(1, line++, 0xe0, ",--.");
	msg(1, line++, 0xe0, "    )");
	msg(1,line++, 0xe0, "  _'-. _                                 ,--.");
	msg(1, line++, 0xe0, " (    ) ),--.                           (    )");
	msg(1,line++, 0xe0, "             )-._                        `--' ");
	msg(1,line++, 0xe0, "_________________)");
}

int generate_boolean() {
//	int seed = get_seed();
//	srand(seed);
	return ((rand() % 300) < probability);
}

// Initialize the game data structures.
void init(void)
{
	int x,y;
	for(y=0; y<24; y++)
		for(x=0; x<80; x++) {
			scr[x][y] = ' ';
			color[x][y] = 0xf0;
		}

	for(x=0; x<80; x++) {
		if (x==0 || x==79) {
			msg(x,0,0xb0,"+");
			msg(x,23,0xb0,"+");
		} else {
			msg(x,0,0xb0,"-");
			msg(x,23,0xb0,"-");
		}
	}
	for(y=1; y<23; y++) {
		msg(0,y, 0xb0, "|");
		msg(79,y, 0xb0, "|");
	}


	drawBiker(BIKER_POS, 22);
	drawSky();
	foot_height = 22;
	stone_queue.pointer = 0;
	stone_queue.pos[0] = 79;
	probability = 10;
	score = 0;
	//	highest_money = 0;

	//	newportal();
	//	newmoney();
	char buf[30];
	msg(MESSAGE_START, 2, 0xe0, "Highest scores:");
	msg(MESSAGE_START, 3, 0xe0, highest_name);
	sprintf(buf, "  %d", highest_score);
	msg(MESSAGE_START, 4, 0xe0, buf);
}

// Dump the scr and color arrays to the terminal screen.
void render(void)
{
	int x,y;
	home();
	int col = color[0][0];
	fgbg(col);
	for(y=0; y<24; y++) {
		setpos(0,y);
		for(x=0; x<80; x++) {
			if (color[x][y] != col) {
				col = color[x][y];
				fgbg(col);
			}
			putchar(scr[x][y]);
		}
	}
	fflush(stdout);
}

// Display the initial splash screen.
void splash(void)
{
	clear();
	int x,y;
	for(y=0; y<24; y++)
		for(x=0; x<80; x++) {
			scr[x][y] = ' ';
			color[x][y] = 0x70;
		}
	msg(30,8, 0x0a, "                 ");
	msg(30,9, 0x0a, "       Biker     ");
	msg(30,10,0x0a, "  Press Any Key  ");
	msg(30,11,0x0a, "                 ");
	render();
}



void jump() {
	switch(jumpState)
	{
	case JUMP1: foot_height = 21; clearBiker(BIKER_POS, 22); break;
	case JUMP2: foot_height = 20; clearBiker(BIKER_POS, 21); break;
	case JUMP3: foot_height = 19; clearBiker(BIKER_POS, 20); break;
	case JUMP4: foot_height = 18; clearBiker(BIKER_POS, 19); break;
	case JUMP5: foot_height = 17; clearBiker(BIKER_POS, 18); break;
	case JUMP6: foot_height = 17; clearBiker(BIKER_POS, 17); break;
	case JUMP7: foot_height = 17; clearBiker(BIKER_POS, 17); break;
	case JUMP8: foot_height = 17; clearBiker(BIKER_POS, 17); break;
	case JUMP9: foot_height = 18; clearBiker(BIKER_POS, 17); break;
	case JUMP10: foot_height = 19; clearBiker(BIKER_POS, 18); break;
	case JUMP11: foot_height = 20; clearBiker(BIKER_POS, 19); break;
	case JUMP12: foot_height = 21; clearBiker(BIKER_POS, 20); break;
	case JUMP13: foot_height = 22; clearBiker(BIKER_POS, 21); break;
	default: foot_height = 22;
	}

	drawBiker(BIKER_POS, foot_height);

	switch(jumpState)
	{
	case JUMP1: jumpState = JUMP2; break;
	case JUMP2: jumpState = JUMP3; break;
	case JUMP3: jumpState = JUMP4; break;
	case JUMP4: jumpState = JUMP5; break;
	case JUMP5: jumpState = JUMP6; break;
	case JUMP6: jumpState = JUMP7; break;
	case JUMP7: jumpState = JUMP8; break;
	case JUMP8: jumpState = JUMP9; break;
	case JUMP9: jumpState = JUMP10; break;
	case JUMP10: jumpState = JUMP11; break;
	case JUMP11: jumpState = JUMP12; break;
	case JUMP12: jumpState = JUMP13; break;
	default: jumpState = IDLE; break;

	}

}

void drawStone(int x) {
	msg(x-5, 22,0xa0, "STONE");
	msg(x-4, 21,0xa0, "TON");

}

void clearStone(int x) {
	msg(x-5, 22,0xa0, "     ");
	msg(x-5, 21,0xa0, "     ");
}

void updateStone() {
	if(stone_queue.pos[0] <= 6 && stone_queue.pointer > 0) {
		clearStone(6);
		deque();
	}
	for(int i = 0; i < stone_queue.pointer; i++){
		clearStone(stone_queue.pos[i]--);
		drawStone(stone_queue.pos[i]);

	}
	int  result = generate_boolean();
	if(result && ((stone_queue.pos[stone_queue.pointer - 1] < 70) || stone_queue.pointer == 0)) {
		enque();
		drawStone(stone_queue.pos[stone_queue.pointer - 1]);
	}
}

// Interpret a key press and update the data structures.
void update(char in)
{

	if(in == 'w' && jumpState == IDLE) {
		jumpState = JUMP1;
	}


	updateScore();

	if(jumpState != IDLE) {
		jump();
	} else {
		drawBiker(BIKER_POS, 22);
	}

	updateStone();

	if(scr[BIKER_POS - 1][foot_height] != ')') {
		phase = SPLASH;
		collision();
	}

}

void animate(void)
{
	if (phase == SPLASH) {
		if(enter_name) {
			update_high();
			enter_name = 0;
		}
		if (splash_ticks < 20) {
			while (available())
				getchar();
			splash_ticks++;
			return;
		}

		// Stall waiting for a key.
		while (!available())
			;
		getchar();
		// Get the timer counter value for the random seed.
		int seed=get_seed();
		srandom(seed);
		init();
		clear();
		phase = RUNNING;
	}

	char in=' ';
	if (phase == RUNNING) {
		while(available()) {
			in = getchar();
		}
		if (in == 'q') {
#ifdef __linux__
			cursor_on();
			cooked_mode();
			exit(0);
#else
			splash();
			render();
			phase = SPLASH;
			splash_ticks = 0;
#endif
		}
		if (in == 'p') {
			freeze();
		}
		update(in);
		render();
		return;
	}
}

void TIM3_IRQHandler(void)
{
    TIM3->SR &= ~0x1;
    animate();
}

//===================================
// Disable timer 2.  Wait for a key press.
void freeze(void)
{
    TIM3 -> CR1 &= ~TIM_CR1_CEN;
    while(!available())
        ;
    getchar();
    TIM3 -> CR1 |= TIM_CR1_CEN;
}

int get_seed(void)
{
    return TIM3 -> CNT;
}

//============================================================================
// Enable clock to timer2.
// Setup prescalar and arr so that the interrupt is triggered every 125 ms.
// Enable the timer 2 interrupt.
// Start the timer.
void setup_timer3() {
    // Enable timer 2 here.
    // Make it generate an interrupt 8 times per second.
    // Set the prescaler to 48 so that the free-running
    // counter goes from 0 ... 124999.

	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3 -> CR1 &= ~TIM_CR1_CEN; //disable
    TIM3 -> PSC = 48 - 1;
    TIM3 -> ARR = 125000 - 1; // set it to 8Hz
    TIM3 -> DIER |= TIM_DIER_UIE; // enable interrupt
    NVIC -> ISER[0] = 1 << TIM3_IRQn;
    NVIC_SetPriority(TIM3_IRQn, 2);
    TIM3 -> CR1 |= TIM_CR1_CEN; //enable


    // Also, make sure TIM2 has lower priority than USART1
    // by using the NVIC_SetPriority() function.  Use a higher
    // number (lower priority) than you did for USART1.
}




int game_init(void)
{
    tty_init();
    raw_mode();
    cursor_off();
    init();
    splash();
    setup_timer3();
    for(;;)
        asm("wfi");
    return 0;
}


// screen.h
// Put the cursor in the upper left.
void home(void);

// Clear the screen.
void clear(void);

// Set the cursor position to x,y (zero-based indexing)
void setpos(int x, int y);

// Turn the cursor off.
void cursor_off(void);

// Turn the cursor on.
void cursor_on(void);

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void fg(int r, int g, int b);

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void bg(int r, int g, int b);

// Set the foreground and background to the byte specified by n.
// The most significant nibble is the foreground.
// The least significant nibble is the background.
// The bits in each nibble specifie:
//  IBGR
//  I    High intensity
//   B   Blue
//    G  Green
//     R Red
//
void fgbg(int n);

// screen.c

// CSI is the "Control Sequence Introducer" (Escape left-square-bracket)
// The terminal recognizes this as the start of a cursor control sequence.
// See this article for more information:
//   https://en.wikipedia.org/wiki/ANSI_escape_code
#define CSI "\e["

// Print a string with no newline.
static void putstr(const char *s)
{
    while(*s)
        putchar(*s++);
}

// Print an integer.
static void putint(int x) {
        char buf[20];
        int off;
        int sign = 1;
        if (x < 0) {
                sign = -1;
                x = -x;
        }
        for(off=0; off < 18; off++) {
                buf[off] = (x % 10) + '0';
                x = x / 10;
                if (x == 0)
                        break;
        }
        if (sign == -1) {
                buf[++off] = '-';
        }
        for(     ; off >= 0; off--)
                putchar(buf[off]);
}

// Put the cursor in the upper left.
void home()
{
  putstr(CSI "1;1H");
}

// Clear the screen.
void clear()
{
  home();
  fgbg(0xf0);
  int x;
  for(x=0; x<4000; x++)
    putchar(' ');
}

// Set the cursor position to (x,y) [zero-based indexing]
void setpos(int x, int y)
{
  putstr(CSI); putint(y+1); putstr(";"); putint(x+1); putstr("H");
}

// Make the cursor invisible.
void cursor_off()
{
  putstr(CSI "?25l");
}

// Make the cursor visible.
void cursor_on()
{
  putstr(CSI "?25h");
}

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void fg(int r, int g, int b)
{
  putstr(CSI "38;5;"); putint(16 + 36 * r + 6*g + b);
}

// Set the foreground color to rgb.
// Each component must have the value 0 ... 5.
void bg(int r, int g, int b)
{
  putstr(CSI "48;5;"); putint(16 + 36 * r + 6*g + b);
}

// Set the foreground and background to the byte specified by n.
// The most significant nibble is the foreground.
// The least significant nibble is the background.
// The bits in each nibble specifie:
//  IBGR
//  I    High intensity
//   B   Blue
//    G  Green
//     R Red
//
void fgbg(int n)
{
  int f = (n>>4) & 0xf;
  int b = (n>>0) & 0xf;
  if (f < 8)
    f += 30;
  else
    f += 90 - 8;

  if (b < 8)
    b += 40;
  else
    b += 100 - 8;

  putstr(CSI); putint(f); putstr(";"); putint(b); putstr("m");
}

// fifo.h
struct fifo {
    char buffer[128]; // circular buffer for the fifo
    volatile uint8_t head; // the first thing to remove from the fifo
    volatile uint8_t tail; // the next place to insert a new character
    volatile uint8_t newline; // offset of last-entered newline
};

int fifo_empty(const struct fifo *f);
int fifo_full(const struct fifo *f);
void fifo_insert(struct fifo *f, char ch);
char fifo_uninsert(struct fifo *f);
int fifo_newline(const struct fifo *f);
char fifo_remove(struct fifo *f);

// fifo.c
//============================================================================
// Return 1 if the fifo holds no characters to remove.  Otherwise 0.
//============================================================================
int fifo_empty(const struct fifo *f) {
    if (f->head == f->tail)
        return 1;
    else
        return 0;
}

//============================================================================
// Return 1 if the fifo cannot hold any more characters.  Otherwise 0.
//============================================================================
int fifo_full(const struct fifo *f) {
    uint8_t next = (f->tail + 1) % sizeof f->buffer;
    // We can't let the tail reach the head.
    if (next == f->head)
        return 1;
    else
        return 0;
}

//============================================================================
// Append a character to the tail of the fifo.
// If the fifo is already full, drop the character.
//============================================================================
void fifo_insert(struct fifo *f, char ch) {
    if (fifo_full(f))
        return; // FIFO is full.  Just drop the new character.
    uint8_t next = (f->tail + 1) % sizeof f->buffer;
    if (ch == '\n')
        f->newline = f->tail; // a newline has been inserted
    else if (f->newline == f->tail)
        f->newline = next; // no newline detected yet
    f->buffer[f->tail] = ch;
    f->tail = next;
}

//============================================================================
// Remove a character from the *tail* of the fifo.
// In other words, undo the last insertion.
//============================================================================
char fifo_uninsert(struct fifo *f) {
    if (fifo_empty(f))
        return '$'; // something unexpected
    int prev = (f->tail == 0) ? sizeof f->buffer - 1 : f->tail - 1;
    if (f->newline == f->tail)
        f->newline = prev;
    f->tail = prev;
    char ch = f->buffer[f->tail];
    f->buffer[f->tail] = '@'; // something unexpected
    return ch;
}

//============================================================================
// Return 1 if the fifo contains at least one newline.  Otherwise 0.
//============================================================================
int fifo_newline(const struct fifo *f) {
    if (fifo_empty(f))
        return 0;
    if (f->newline != f->tail)
        return 1;
    return 0;
}

//============================================================================
// Remove a character from the head of the fifo.
// If the fifo is empty, you get a tilde (~).
//============================================================================
char fifo_remove(struct fifo *f) {
    if (fifo_empty(f))
        return '!'; // something unexpected.
    char ch = f->buffer[f->head];
    if (f->newline == f->head)
        f->newline = f->tail; // We just read the last newline.  Clear nl.
    f->buffer[f->head] = '~'; // overwrite removed char with something weird
    uint8_t next = (f->head + 1) % sizeof f->buffer;
    f->head = next;
    return ch;
}

//tty.h

void tty_init(void);
void tty_uninit(void);
int  available(void);
int  getkey(void);
void raw_mode(void);
void cooked_mode(void);

//tty.c

#define UNK -1
#define NON_INTR 0
#define INTR 1
int interrupt_mode = UNK;

int __io_putchar(int ch);
static int putchar_nonirq(int ch);
static struct fifo input_fifo;  // input buffer
static struct fifo output_fifo; // output buffer
int echo_mode = 1;              // should we echo input characters?
int line_mode = 1;              // should we wait for a newline?

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
static void putstr_tty(const char *s) {
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
                putstr_tty("\b\b  \b\b");
            else
                putstr_tty("\b \b");
        }
        return; // Don't put a backspace into buffer.
    } else if (ch == '\n') {
        __io_putchar('\n');
    } else if (ch == 0){
        putstr_tty("^0");
    } else if (ch == 28) {
        putstr_tty("^\\");
    } else if (ch < 32) {
        __io_putchar('^');
        __io_putchar('A'-1+ch);
    } else {
        __io_putchar(ch);
    }
    fifo_insert(&input_fifo, ch);
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
	NVIC_SetPriority(USART1_IRQn,1);
    // REMEMBER TO SET THE USART1 INTERRUPT PRIORITY HERE!
}

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
	if(line_mode) {
		while(!fifo_newline(&input_fifo)) {
			while(!(USART1 -> ISR & USART_ISR_RXNE));
			insert_echo_char(USART1 -> RDR);
		}
	}
	return fifo_remove(&input_fifo);
    // REMEMBER TO NOT CHECK FOR NEWLINES WHEN line_mode IS NOT SET!
}

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

    // REMEMBER TO REMOVE THE CKECK FOR USART_ISR_RXNE FROM THE WARNING
    // THAT INVOKES safe_printf().
}

// See lab document for description
static int getchar_irq(void) {
    // Student code goes here...
	if(line_mode) {
		while(!fifo_newline(&input_fifo)) {
			asm("wfi");
		}
	}
	return fifo_remove(&input_fifo);
    // REMEMBER TO CHECK FOR A NEWLINE ONLY WHEN line_mode IS SET!
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
    if (interrupt_mode == INTR)
        return getchar_irq();
    else
        return getchar_nonirq();
}

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
   USART1 -> BRR = 0xD0;
   enable_tty_irq();
   USART1 -> CR1 |= USART_CR1_TE;
   USART1 -> CR1 |= USART_CR1_RE;

   USART1 -> CR1 |= USART_CR1_UE;

   while(!(USART1 -> ISR & USART_ISR_TEACK));
   while(!(USART1 -> ISR & USART_ISR_REACK));
   interrupt_mode = NON_INTR;
   enable_tty_irq();
    // REMEMBER TO CALL enable_tty_irq() HERE!
}

void raw_mode(void)
{
    line_mode = 0;
    echo_mode = 0;
}

void cooked_mode(void)
{
    line_mode = 1;
    echo_mode = 1;
}

int available(void)
{
    if (interrupt_mode == INTR) {
        if (fifo_empty(&input_fifo))
            return 0;
        return 1;
    } else {
        if ((USART1->ISR & USART_ISR_RXNE) == 0)
            return 0;
        return 1;
    }
}
