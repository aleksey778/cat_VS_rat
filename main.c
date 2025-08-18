#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>


void drawing_map(char **map, int rows, int cols)
{
	for (int yy = 0; yy < rows; yy++)
		for (int xx = 0; xx < cols; xx++) {
			mvaddch(yy, xx, '#');
			map[yy][xx] = '#';
		}
	for (int yy = 5; yy < rows-10; yy++)
		for (int xx = 5; xx < cols-10; xx++) {
			mvaddch(yy, xx, ' ');
			map[yy][xx] = ' ';
		}
}


void move_me(char **map, int *last_x, int *last_y, int *x, int *y,
const int *my_key_input, char my_player)
{
	*last_x = *x, *last_y = *y;
	if (*my_key_input == KEY_UP)
		(*y)--;
	else if (*my_key_input == KEY_DOWN)
		(*y)++;
	else if (*my_key_input == KEY_LEFT)
		(*x)--;
	else if (*my_key_input == KEY_RIGHT)
		(*x)++;
	if (map[*y][*x] == '#' && my_player != 'r') { //крыса может проедать стены
		*x = *last_x;
		*y = *last_y;
	}
	
	mvaddch(*last_y, *last_x, ' '); //clear last position
	map[*last_y][*last_x] = ' ';
	mvaddch(*y, *x, my_player); //print new position
	map[*y][*x] = 'c';

}


int main()
{
	//init
	initscr();
	//set
	keypad(stdscr, 1);
	noecho();
	curs_set(0);
	
	//переменные для управление курсором
	int c = 0;
	int x = 11, y = 11;
	int last_x = x, last_y = y;
	char my_player;
	
	//масштаб карты
	int cols, rows;
	getmaxyx(stdscr, rows, cols); //для кроссплатф.
	
	//карта(база данных) - динамический двумерный массив
	char **map = (char**)malloc(rows * sizeof(int*));
	if (map == NULL) {
		return 0;
	}
	for (int i = 0; i < rows; i++) {
		map[i] = (char*)malloc(cols * sizeof(int));
		if (map == NULL) {
			return 0;
		}
	}
	
	
	
	//выбор персонажа: кот или крыса
	mvprintw(rows/2, cols/2, "Press key\n'1' - cat\n'2' - rat\n");
	c = getch();
	my_player = ((c == '1') ? 'c' : 'r');
	
	//прорисовка карты
	drawing_map(map, rows, cols);
		
	
	//Главный цикл
	do {
		move_me(map, &last_x, &last_y, &x, &y, &c, my_player);
		
	} while ((c = getch()) && c != 'q' && c != 27); //27 - ESC
	
		
		
		
		
	for (int i = 0; i < rows; i++) 
		free(map[i]);
	free(map);
	endwin();
	return 0;
}