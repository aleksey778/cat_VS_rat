#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>


void drawing_map(char **map, int rows, int cols)
{
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++) {
            mvaddch(y, x, '#');
            map[y][x] = '#';
        }
    for (int y = 5; y < rows-10; y++)
        for (int x = 5; x < cols-10; x++) {
            mvaddch(y, x, ' ');
            map[y][x] = ' ';
        }
}


void move_me(char **map, int *last_x, int *last_y, int *px, int *py,
const int *my_key_input, char my_role)
{
    *last_x = *px, *last_y = *py;
    if (*my_key_input == KEY_UP)
        (*py)--;
    else if (*my_key_input == KEY_DOWN)
        (*py)++;
    else if (*my_key_input == KEY_LEFT)
        (*px)--;
    else if (*my_key_input == KEY_RIGHT)
        (*px)++;
    if (map[*py][*px] == '#' && my_role != 'r') { //крыса может проедать стены
        *px = *last_x;
        *py = *last_y;
    }
    
    mvaddch(*last_y, *last_x, ' '); //clear last position
    map[*last_y][*last_x] = ' ';
    mvaddch(*py, *px, my_role); //print new position
    map[*py][*px] = my_role;
}


void move_enemy(char **map, int *last_x, int *last_y, int *ex, int *ey, char enemy_role)
{
    *last_x = *ex, *last_y = *ey;
    
    mvaddch(*last_y, *last_x, ' '); //clear last position
    map[*last_y][*last_x] = ' ';
    
    mvaddch(*ey, *ex, enemy_role); //print new position
    map[*ey][*ex] = enemy_role;    
}


int main()
{
    //init
    initscr();
    //set
    keypad(stdscr, 1);
    noecho();
    curs_set(0);
    
    //переменные моего персонажа
    int c = 0;
    int px = 11, py = 11;
    int last_x = px, last_y = py;
    char my_role; //c - cat, r - rat
    //переменные противника
    int ex = 12, ey = 12;
    int last_x_enemy = ex, last_y_enemy = ey;
    char enemy_role; //c - cat, r - rat
    
    
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
    my_role = ((c == '1') ? 'c' : 'r');
    enemy_role = ((c == '1') ? 'r' : 'c');
    
    //прорисовка карты
    drawing_map(map, rows, cols);
        
    
    //Главный цикл
    do {
        move_me(map, &last_x, &last_y, &px, &py, &c, my_role);
        move_enemy(map, &last_x_enemy, &last_y_enemy, &ex, &ey, enemy_role);
        
    } while ((c = getch()) && c != 'q' && c != 27); //27 - ESC
    
        
        
        
        
    for (int i = 0; i < rows; i++) 
        free(map[i]);
    free(map);
    endwin();
    return 0;
}