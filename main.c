#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>


void drawing_map(char **map, int rows, int cols)
{
    //границы
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++) {
            mvaddch(y, x, '%');
            map[y][x] = '%';
        }
    
    //cтены
    for (int y = 1; y < rows-1; y++)
        for (int x = 1; x < cols-1; x++) {
            mvaddch(y, x, '#');
            map[y][x] = '#';
        }
    //пустые места
    int  r_x = (rand() % (cols / 3)) + 1; //[1; (cols / 3)]
    int r_y = (rand() % (rows / 3)) + 1; //[1; (rows / 3)]
    
    for (int y = r_y; y < rows-r_y; y++)
        for (int x = r_x; x < cols-r_x; x++) {
            mvaddch(y, x, ' ');
            map[y][x] = ' ';
        }
}


void clear_map(char **map, int rows, int cols)
{
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++) {
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
    
 
    //если шаг на стену/границу - возвращение персонажа на исходную позицию
    if (map[*py][*px] == '%' || (my_role == 'c' && map[*py][*px] == '#')) {
        *px = *last_x;
        *py = *last_y;    
    } //только крыса может проедать стены(но не границы)
    
    
    mvaddch(*last_y, *last_x, ' '); //clear last position
    map[*last_y][*last_x] = ' ';
    mvaddch(*py, *px, my_role); //print new position
    map[*py][*px] = my_role;
}


void move_enemy(char **map, int *last_x, int *last_y, int *ex, int *ey,
int px, int py, char enemy_role)
{
    *last_x = *ex, *last_y = *ey;
    
    // для кота
    if (enemy_role == 'c') {
        int r = rand() % 2; //[0; 1]
        //примечение: если равны по х, то не трогать х, если ==у, то у
        //X преследование
        if ((r || *ey == py) && *ex != px) {
            if ((*ex <= px) && (map[*ey][(*ex)+1] != '%' && map[*ey][(*ex)+1] != '#'))
                (*ex)++;
            else if ((*ex >= px) && (map[*ey][(*ex)-1] != '%' && map[*ey][(*ex)-1] != '#'))
                (*ex)--;
        }
        //Y преследование
        else {
            if ((*ey <= py) && (map[(*ey)+1][*ex] != '%' && map[(*ey)+1][*ex] != '#'))
                (*ey)++;
            else if ((*ey >= py) && (map[(*ey)-1][*ex] != '%' && map[(*ey)-1][*ex] != '#'))
                (*ey)--;
        }
    }
    //для крысы
    else if (enemy_role == 'r') {
        int r = rand() % 2; //[0; 1]
        //X побег
        if (r && *ey != py) {
            if ((*ex <= px) && (map[*ey][(*ex)-1] != '%' && map[*ey][(*ex)-1] != 'c'))
                (*ex)--;
            else if ((*ex >= px) && (map[*ey][(*ex)+1] != '%' && map[*ey][(*ex)+1] != 'c'))
                (*ex)++;
        }
        //Y побег
        else {
            if ((*ey <= py) && (map[(*ey)-1][*ex] != '%' && map[(*ey)-1][*ex] != 'c'))
                (*ey)--;
            else if ((*ey >= py) && (map[(*ey)+1][*ex] != '%' && map[(*ey)+1][*ex] != 'c'))
                (*ey)++;
        }
    }
    
    mvaddch(*last_y, *last_x, ' '); //clear last position
    map[*last_y][*last_x] = ' ';
    
    mvaddch(*ey, *ex, enemy_role); //print new position
    map[*ey][*ex] = enemy_role;    
}


bool fight_if_collision(char **map, int px, int py, int ex, int ey,
int rows, int cols, char my_role)
{
    bool collision = (px == ex) && (py == ey);
    if (collision) {
        const char *end_text = (my_role == 'c') ? "win" : "lose";
        clear_map(map, rows, cols);
        mvprintw(rows/2, cols/2, "You %s\n", end_text);
        getch();
    }
    
    return collision;
}


int main()
{
    //init
    initscr();
    //set
    keypad(stdscr, 1);
    noecho();
    curs_set(0);
    //seed
    srand(time(NULL));
    
    
    //переменные моего персонажа
    int c = 0;
    int px, py;
    int last_x, last_y;
    char my_role; //c - cat, r - rat
    //переменные противника
    int ex, ey;
    int last_x_enemy, last_y_enemy;
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
    
    
    //рандомное место появления (спавн)
    do {
        px = rand() % cols;
        py = rand() % rows;
    } while (map[py][px] != ' ');
    do {
        ex = rand() % cols;
        ey = rand() % rows;
    } while (map[ey][ex] != ' ');
    
    
    //Главный цикл
    do {
        
        move_me(map, &last_x, &last_y, &px, &py, &c, my_role);
        if (fight_if_collision(map, px, py, ex, ey, rows, cols, my_role) == true)
            break;
        
        move_enemy(map, &last_x_enemy, &last_y_enemy, &ex, &ey,
        px, py, enemy_role);
        if (fight_if_collision(map, px, py, ex, ey, rows, cols, my_role) == true)
            break;
        
    } while ((c = getch()) && c != 'q' && c != 27); //27 - ESC
    
        
        
        
        
    for (int i = 0; i < rows; i++) 
        free(map[i]);
    free(map);
    endwin();
    return 0;
}