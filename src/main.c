#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#define PADDING_X 15
#define PADDING_Y 1

int cx, cy, last_cx, last_cy, rows, cols, px, py, palette_mode, current_col;

void initNCurses() {
    setlocale(LC_CTYPE, ""); 
    initscr();
    start_color();
    cbreak(); 
    noecho(); 
	curs_set(0);
    keypad(stdscr, TRUE);
}

void fillRect(int x, int y, int w, int h, int col) {

	attron(COLOR_PAIR(col));

	for(int i=0; i<h; i++) {
		move(y+i, x);
		for(int j=0; j<w; j++)
			addch(' ');
	}

	attroff(COLOR_PAIR(col));
	move(cy, cx);
	
}

void fillCell(int x, int y, int col) {
	attron(COLOR_PAIR(col));
	mvaddch(x, y, ' ');
	move(cy, cx);
}

void fillLine(int x1, int y1, int x2, int y2, int col) {

	if(x1 > x2) {int temp = x1; x1 = x2; x2 = temp;}
	if(y1 > y2) {int temp = y1; y1 = y2; y2 = temp;}

	attron(COLOR_PAIR(col));
	if(x1 == x2)
		for(int y=y1; y<=y2; y++)
			mvaddch(y, x1, ' ');
	else if(y1 == y2)
		for(int x=x1; x<= x2; x++)
			mvaddch(y1, x, ' ');
	move(cy, cx);
}

int getCol(int x, int y) {

	chtype cell_content = mvwinch(stdscr, y, x);
	return PAIR_NUMBER(cell_content & A_COLOR);

}

void updateCursor(int x, int y) {

	int inBoundsCanvas = PADDING_X <= x && x < cols-PADDING_X && PADDING_Y <= y && y < rows-PADDING_Y;
	int inBoundsPalette = px <= x && x < px + 9 && py <= y && y < py + 9 && (x != px+7 || y != py+7);

	if((inBoundsCanvas && !palette_mode) || (inBoundsPalette && palette_mode)) { 
		// Erase previous cursor
		int col = getCol(cx, cy);
		attron(COLOR_PAIR(col));
		mvaddch(cy, cx, ' ');
		refresh();

		// Update position
		cx = x;
		cy = y;

		// Draw new one
		int new_col = getCol(cx, cy);
		attron(COLOR_PAIR(new_col));
		mvaddch(cy, cx, '*');
		refresh();

		move(cy, cx);
	}

}

void initColorPairs() {

	// Text
	init_pair(1, COLOR_WHITE, COLOR_RED);
	init_pair(2, COLOR_WHITE, COLOR_YELLOW);
	init_pair(3, COLOR_WHITE, COLOR_GREEN);
	init_pair(4, COLOR_WHITE, COLOR_CYAN);
	init_pair(5, COLOR_WHITE, COLOR_BLUE);
	init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	init_pair(8, COLOR_BLACK, COLOR_WHITE);

}

void displayTools() {
	
	fillRect(cols-12, 10, 9, 9, 1);

	// Line tool
	attron(COLOR_PAIR(7));
	mvaddch(13, cols-11, ' ');
	mvaddch(12, cols-10, ' ');
	mvaddch(11, cols-9, ' ');
	move(cy, cx);
	

}

void drawInitialScreen(int rows, int cols) {
	fillRect(0, 0, cols, rows, 2); // Background

	// Canvas
	int canvas_width = cols-2*PADDING_X;
	
	attron(COLOR_PAIR(2));
	move(PADDING_Y-1, PADDING_X-1);
	char* top = (char*)malloc(canvas_width+2);
	top[0] = '@';
	/*
	for(int i=0; i<canvas_width; i++) 
		addch('─');
		*/

	fillRect(PADDING_X, PADDING_Y, canvas_width, rows-2*PADDING_Y, 8); // Canvas
	
	// Palette
	px = cols - 12;
	py = 4;

	// Draw background
	attron(COLOR_PAIR(8));
	mvprintw(py-1, px-1, "┌─────────┐");

	for(int i=0; i<9; i++)
		mvprintw(py+i, px-1, "│         │");

	mvprintw(py+9, px-1, "└─────────┘");
	

	// Draw colors
	for(int i=0; i<9; i++)
		fillRect(px + 3*(i%3), py + 3*(i/3), 3, 3, i+1);
	fillRect(px+6, py+6, 3, 3, 8);
	
}

void togglePaletteMode() {
	palette_mode = !palette_mode;
	if(palette_mode) {
		last_cx = cx;
		last_cy = cy;
		updateCursor(px+1, py+1);
	}
	else
		updateCursor(last_cx, last_cy);
	
}

void debug() {

	attron(COLOR_PAIR(8));
	mvprintw(20, cols-10, "test");

}


int main() {

	initNCurses();
	initColorPairs();
    getmaxyx(stdscr, rows, cols);
	drawInitialScreen(rows, cols);
	updateCursor(cols / 2, rows / 2);

	palette_mode = 0;
	current_col = 7;
	last_cx = cx;
	last_cy = cy;

	// Input loop
	char ch;
	while((ch = getch()) != 'q') {
		int move_dist = palette_mode ? 3 : 1;
		switch(ch) {
			case 'h':
				updateCursor(cx-move_dist, cy);
				break;
			case 'j':
				updateCursor(cx, cy+move_dist);
				break;
			case 'k':
				updateCursor(cx, cy-move_dist);
				break;
			case 'l':
				updateCursor(cx+move_dist, cy);
				break;
			case 'p':
				togglePaletteMode();
				break;
			case 'e':
				current_col = 8;
				break;
			case ' ':
				if(palette_mode) {
					current_col = getCol(cx, cy);
					togglePaletteMode();
				}
				else {
					attron(COLOR_PAIR(current_col));
					addch('*');
					move(cy, cx);
				}
				break;
		}
        refresh();
	}


    endwin();



	return 0;
}
