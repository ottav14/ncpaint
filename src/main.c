#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define PADDING_X 15
#define PADDING_Y 1

int cx, cy, rows, cols, palette_x, palette_y, palette_mode;

void initNCurses() {
    initscr();
    start_color();
    cbreak(); 
    noecho(); 
	curs_set(0);
    keypad(stdscr, TRUE);
}

int fillRect(int x, int y, int w, int h, int col) {

	attron(COLOR_PAIR(col));

	for(int i=0; i<h; i++) {
		for(int j=0; j<w; j++) {
			mvaddch(y + i, x + j, ' ');
		}
	}

	attroff(COLOR_PAIR(col));
	
	return 0;
}

int getCol(int x, int y) {

	chtype cell_content = mvwinch(stdscr, y, x);
	return PAIR_NUMBER(cell_content & A_COLOR);

}

void updateCursor(int x, int y) {

	int inBoundsCanvas = PADDING_X <= x && x < cols-PADDING_X && PADDING_Y <= y && y < rows-PADDING_Y;
	int inBoundsPalette = palette_x <= x && x < palette_x + 3 && palette_y <= y && y < palette_y + 3;

	if((inBoundsCanvas && !palette_mode) || (inBoundsPalette && palette_mode)) { 
		// Erase previous cursor
		int col = getCol(cx, cy) - 8;
		attron(COLOR_PAIR(col));
		mvaddch(cy, cx, ' ');
		refresh();

		// Update position
		cx = x;
		cy = y;

		// Draw new one
		int new_col = getCol(cx, cy) + 8;
		attron(COLOR_PAIR(new_col));
		mvaddch(cy, cx, '*');
		refresh();

		move(cy, cx);
	}

}

void initColorPairs() {

	// Solid
	init_pair(1, COLOR_RED, COLOR_RED);
	init_pair(2, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(3, COLOR_GREEN, COLOR_GREEN);
	init_pair(4, COLOR_CYAN, COLOR_CYAN);
	init_pair(5, COLOR_BLUE, COLOR_BLUE);
	init_pair(6, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(7, COLOR_BLACK, COLOR_BLACK);
	init_pair(8, COLOR_WHITE, COLOR_WHITE);

	// Text
	init_pair(9, COLOR_WHITE, COLOR_RED);
	init_pair(10, COLOR_WHITE, COLOR_YELLOW);
	init_pair(11, COLOR_WHITE, COLOR_GREEN);
	init_pair(12, COLOR_WHITE, COLOR_CYAN);
	init_pair(13, COLOR_WHITE, COLOR_BLUE);
	init_pair(14, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(15, COLOR_WHITE, COLOR_BLACK);
	init_pair(16, COLOR_BLACK, COLOR_WHITE);

}

void drawInitialScreen(int rows, int cols) {
	fillRect(0, 0, cols, rows, 2); // Background
	attron(COLOR_PAIR(2));
	mvaddch(0, 0, ' ');
	fillRect(PADDING_X, PADDING_Y, cols-2*PADDING_X, rows-2*PADDING_Y, 8); // Canvas
	
	// Palette
	palette_x = cols - 8;
	palette_y = 4;
	fillRect(palette_x - 1, palette_y - 1, 5, 5, 8);
	for(int i=1; i<9; i++) {
		attron(COLOR_PAIR(i));
		mvaddch(palette_y + (i-1)/3, palette_x + (i-1)%3, ' ');
		attroff(COLOR_PAIR(i));
	}
}

void togglePaletteMode() {
	palette_mode = !palette_mode;
	if(palette_mode)
		updateCursor(cols - 8, 4);
	else
		updateCursor(cols / 2, rows / 2);
}


int main() {

	initNCurses();
	initColorPairs();
    getmaxyx(stdscr, rows, cols);
	drawInitialScreen(rows, cols);
	updateCursor(cols / 2, rows / 2);

	palette_mode = 0;
	int current_col = 7;

	// Input loop
	char ch;
	while((ch = getch()) != 'q') {
		if(ch == 'h') 
			updateCursor(cx-1, cy);
		else if(ch == 'j') 
			updateCursor(cx, cy+1);
		else if(ch == 'k') 
			updateCursor(cx, cy-1);
		else if(ch == 'l') 
			updateCursor(cx+1, cy);
		else if(ch == 'p') { 
			togglePaletteMode();
		}
		else if(ch == ' ' && palette_mode) {
			current_col = getCol(cx, cy);
		}
		else if(ch == ' ') {
			attron(COLOR_PAIR(current_col + 8));
			addch('*');
			move(cy, cx);
		}
        refresh();
	}

    endwin();



	return 0;
}
