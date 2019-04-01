#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DELAY 70000			// opoznienie
#define BALLS 10			// ilosc kulek
#define SPD_RNG_START 1		// poczatek zakresu losowych predkosci
#define SPD_RNG_END 2		// koniec zakresu losowych predkosci
#define CONSTANT_SPD false 	// czy predkosc ma byc stala
#define SPEED 1				// wartosc stalej predkosci

// globalne rozmiary ekranu do kontruktora
int max_y = 0, max_x = 0;
bool stop = false;

class ball {
public:
	int x, y;
	int next_x, next_y;
	int direction_x, direction_y;
	int speed;

	ball(){

		next_x = 0;
		next_y = 0;
	}
	void setSpeed(int spd) {
		speed = spd;
	}
	void setBall(int max_x, int max_y) {
		x = rand()%max_x;
		y = rand()%max_y;

		if(rand()%100 > 49) {
			direction_y = 1 * speed;
		}else{
			direction_y = -1 * speed;
		}

		if(rand()%100 > 49) {
			direction_x = 1 * speed;
		}else{
			direction_x = -1 * speed;
		}
	}
	~ball();
	
};

ball * arr = new ball[BALLS];

void* balls(void* id) {
	while(!stop) {
		for(int i = 0; i < BALLS; i++) {
			arr[i].next_x = arr[i].x + arr[i].direction_x;
			arr[i].next_y = arr[i].y + arr[i].direction_y;

			if (arr[i].next_x >= max_x || arr[i].next_x < 0) {
 				arr[i].direction_x*= -1;
 			} else {
 				arr[i].x += arr[i].direction_x;
 			}

 			if (arr[i].next_y >= max_y || arr[i].next_y < 0) {
 				arr[i].direction_y*= -1;
 			} else {
 				arr[i].y += arr[i].direction_y;
 			}
		}
		usleep(DELAY);
	}
	return NULL;
}

// atexit przyjmuje tylko void, a endwin jest int
void endwinVoid () { endwin(); }

// rysowanie kulek
void draw() {
	while(!stop) {

		// zakoncz program na keypressie
		if(getch() != ERR) {
			atexit(endwinVoid);
			exit(0);
		}
		clear();
		for(int i = 0; i < BALLS; i++) {
 			mvprintw(arr[i].y, arr[i].x, "o");
 			refresh();
		}

		usleep(DELAY);
	}
}

int main(int argc, char *argv[]) {
	srand (time(NULL));
	pthread_t id;
	int ret;

	initscr();
	noecho();
	curs_set(FALSE);
	nodelay(stdscr, TRUE);
 	getmaxyx(stdscr, max_y, max_x);

 	// ustawienia kulek (predkosc i obszar)
 	for(int i = 0; i < BALLS; i++) {
 		if(CONSTANT_SPD) arr[i].setSpeed(SPEED);
 		else arr[i].setSpeed(rand()%SPD_RNG_END+SPD_RNG_START);
 		arr[i].setBall(max_x, max_y);
 	}

	ret = pthread_create(&id, NULL, balls, NULL);
 	draw();

 endwin();
}