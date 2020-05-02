#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h> // for random()
#include "screen.h"
#include "tty.h"
#include "snake.h"
#define MESSAGE_START 63

// The characters to be shown and their colors.
char scr[80][24];
char color[80][24];

// Data variables for the game...
int px;
int py;
int dx;
int dy;
int prevdx;
int prevdy;
int money;
int enter_name;
int highest_money = 0;
char highest_name[11] = "          ";
enum { SPLASH, RUNNING } phase = SPLASH;
int splash_ticks;
int health;
int line;

#define MAXLEN 40
struct {
	int8_t x;
	int8_t y;
} body[MAXLEN];

struct {
	int x1;
	int x2;
	int y1;
	int y2;
} portal;
int bodylen;

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

// Put a dollar sign in a random location of the screen.
// Make sure that there is nothing there though.
// Also, make sure it is not too close to the snake's head.
// Let's not make the game too easy.
void newmoney(void)
{
	int x,y;
	do {
		x = (random() % 60) + 1;
		y = (random() % 22) + 1;
	} while(scr[x][y] != ' ' || tooclose(x,y, px,py) || tooclose(x,y, portal.x1, portal.y1) || tooclose(x,y, portal.x2, portal.y2));
	scr[x][y] = '$';
	color[x][y] = 0xe0;
}

void newportal(void)
{
	int x,y;
	do {
		x = (random() % 60) + 1;
		y = (random() % 22) + 1;
	} while(scr[x][y] != ' ' || tooclose(x,y, px,py));
	scr[x][y] = 'O';
	color[x][y] = 0xe0;
	portal.x1 = x;
	portal.y1 = y;
	do {
		x = (random() % 60) + 1;
		y = (random() % 22) + 1;
	} while(scr[x][y] != ' ' || tooclose(x,y, px,py) || tooclose(x,y, portal.x1, portal.y1));
	scr[x][y] = 'O';
	color[x][y] = 0xe0;
	portal.x2 = x;
	portal.y2 = y;

}

// Eat some money.  Increment the money counter.
// Display the total money in the upper right of the screen.
void getmoney(void)
{
	money++;
	health = 100;
	char buf[30];
	sprintf(buf, "Money:%d", money);
	msg(MESSAGE_START,5, 0xe0, buf);
	newmoney();
}

void updateHealth() {
	health--;
	char buf[100];
	sprintf(buf, "Health:%2d", health);
	msg(MESSAGE_START,9,0xe0, buf);
}

// Indicate a collision with a big red X.
// Also display a "Game over" statement.
void collision(int bankrupt)
{
	msg(px,py,0x90, "X");
	msg(px-1,py-1,0x90, "\\");
	msg(px+1,py-1,0x90, "/");
	msg(px+1,py+1,0x90, "\\");
	msg(px-1,py+1,0x90, "/");
	if(money > highest_money) {
		enter_name = 1;
		highest_money = money;
	}
	if (py > 12) {
		line = 7;
		if(bankrupt) {
			msg(30,line++, 0x0f, "    bankrupt   ");
		}
		msg(30,line++, 0x0f, "   Game over   ");
		if(!enter_name) {
			msg(30,line++, 0x0f," Press any key ");
		}
//		msg(20,line, 0x0f,"please enter your name:");
	} else {
		line = 14;
			if(bankrupt) {
				msg(30,line++, 0x0f, "    bankrupt   ");
			}
		msg(30,line++, 0x0f,"   Game over   ");
		if(!enter_name) {
			msg(30,line++, 0x0f," Press any key ");
		}
//		msg(20,line, 0x0f,"please enter your name:");
	}

}

void update_high() {
	char buf[11];
	char get;
	int index = -1;
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

	msg(MESSAGE_START, 11, 0xe0, "Highest scores:");
	msg(MESSAGE_START, 12, 0xe0, highest_name);
	sprintf(buf, "  %d", money);
	msg(MESSAGE_START, 13, 0xe0, buf);
	msg(30,++line, 0x0f, " Press any key ");
	render();
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
	// drawing horizontal boundaries
	for(x=0; x<62; x++) {
		if (x==0 || x==61) {
			msg(x,0,0xb0,"+");
			msg(x,23,0xb0,"+");
		} else {
			msg(x,0,0xb0,"-");
			msg(x,23,0xb0,"-");
		}
	}
	// drawing vertical boundaries
	for(y=1; y<23; y++) {
		msg(0,y, 0xb0, "|");
		msg(61,y, 0xb0, "|");
	}

	// initiate snake
	px=40;
	py=12;

	scr[px][py] = '@';
	color[px][py] = 0xa0;
	bodylen = 6;
	body[0].x = px;
	body[0].y = py;
	for(x=1; x<6; x++) {
		scr[px-x][py] = '-';
		color[px-x][py] = 0xa0;
		body[x].x = px-x;
		body[x].y = py;
	}

	dx=1;
	dy=0;
	prevdx = dx;
	prevdy = dy;
	health = 100;
	money = 0;
//	highest_money = 0;

	newportal();
	newmoney();
	char buf[30];
	msg(MESSAGE_START, 11, 0xe0, "Highest scores:");
	msg(MESSAGE_START, 12, 0xe0, highest_name);
	sprintf(buf, "  %d", highest_money);
	msg(MESSAGE_START, 13, 0xe0, buf);
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
	msg(30,9, 0x0a, "  Hungry Snake   ");
	msg(30,10,0x0a, "  Press Any Key  ");
	msg(30,11,0x0a, "                 ");
	render();
}

// Extend the snake into the new position.
void extend(void)
{
	scr[px][py] = '@'; // draw new head
	color[px][py] = 0xa0;
	if (dx != 0) {
		if (prevdx != 0) // Did we turn a corner?
			scr[body[0].x][body[0].y] = '-'; // no
		else
			scr[body[0].x][body[0].y] = '+'; // yes
	} else {
		if (prevdy != 0) // Did we turn a corner?
			scr[body[0].x][body[0].y] = '|'; // no
		else
			scr[body[0].x][body[0].y] = '+'; // yes
	}
	int n;
	for(n=bodylen; n>=0; n--)
		body[n] = body[n-1];
	body[0].x = px;
	body[0].y = py;
	bodylen += 1;
	char buf[100];
	sprintf(buf, "Length:%d", bodylen);
	msg(MESSAGE_START,7,0xe0,buf);
}

// Move the snake into the new position.
// And erase the last character of the tail.
void move(int color_change)
{
	scr[body[bodylen-1].x][body[bodylen-1].y] = ' '; // erase end of tail
	scr[px][py] = '@'; // draw new head
	color[px][py] = 0xa0;
	if(color_change) {
		color[px][py] = 0xc0;
	}
	if (dx != 0) {
		if (prevdx != 0) // Did we turn a corner?
			scr[body[0].x][body[0].y] = '-'; // no
		else
			scr[body[0].x][body[0].y] = '+'; // yes
	} else {
		if (prevdy != 0) // Did we turn a corner?
			scr[body[0].x][body[0].y] = '|'; // no
		else
			scr[body[0].x][body[0].y] = '+'; // yes
	}
	int n;
	for(n=bodylen-1; n>0; n--) {
		body[n] = body[n-1];
	}
	body[0].x = px;
	body[0].y = py;
}

// Interpret a key press and update the data structures.
void update(char in)
{
	prevdx = dx;
	prevdy = dy;
	switch(in) {
		case 'a':
		case 'h': dx=-1; dy=0; break;
		case 's':
		case 'j': dx=0; dy=1; break;
		case 'w':
		case 'k': dx=0; dy=-1; break;
		case 'd':
		case 'l': dx=1; dy=0; break;
		default: break;
	}

	px += dx;
	py += dy;
	updateHealth();

	if (scr[px][py] == '$') {
		getmoney();
		if (bodylen == MAXLEN-1)
			msg(26,0,0x90,"Omega-Snake-Status-Achieved");
		if (bodylen < MAXLEN)
			extend();
		else
			move(0);
	} else if(scr[px][py] == 'O') {
		if(px == portal.x1) {
			px = portal.x2 + dx;
			py = portal.y2 + dy;
		} else {
			px = portal.x1 + dx;
			py = portal.y1 + dy;
		}
		move(0);
	} else if (health < 1) {
		collision(1);
		phase = SPLASH;
		splash_ticks = 0;
	} else if (scr[px][py] == ' ') {
		if(health < 50) {
			move(1);
		} else {
			move(0);
		}
	}else {
		collision(0);
		phase = SPLASH;
		splash_ticks=0;
	}

}

void animate(void)
{
	if (phase == SPLASH) {
		if(enter_name) {
			update_high();
			enter_name = 0;
		}
		if (splash_ticks < 10) {
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
