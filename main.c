#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <ncurses.h>



struct player {
    int x, y;
    int last_x, last_y;
    char role;
};


void print_map(char **map, int rows, int cols)
{
    for (int y = 0; y < rows-1; y++)
        for (int x = 0; x < cols; x++)
            mvaddch(y, x, map[y][x]);
}


void drawing_map(char **map, int rows, int cols)
{
    //границы
    for (int y = 0; y < rows-1; y++)
        for (int x = 0; x < cols; x++)
            map[y][x] = '%';
      
    
    //cтены
    for (int y = 1; y < rows-2; y++)
        for (int x = 1; x < cols-1; x++)
            map[y][x] = '#';
            
        
    //пустые места
    int  r_x = (rand() % (cols / 3)) + 1; //[1; (cols / 3)]
    int r_y = (rand() % (rows / 3)) + 1; //[1; (rows / 3)]
    
    for (int y = r_y; y < rows-r_y-1; y++)
        for (int x = r_x; x < cols-r_x; x++)
            map[y][x] = ' ';
}


void clear_map(char **map, int rows, int cols)
{
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++) {
            mvaddch(y, x, ' ');
            map[y][x] = ' ';
        }
}


void move_me(char **map, struct player *my_player, const int *my_key_input, int *my_cheese)
{
    int px = my_player->x;
    int py = my_player->y;
    int my_role = my_player->role;
    int last_x = my_player->last_x;
    int last_y = my_player->last_y;
    
    last_x = px, last_y = py;
    if (*my_key_input == KEY_UP)
        py--;
    else if (*my_key_input == KEY_DOWN)
        py++;
    else if (*my_key_input == KEY_LEFT)
        px--;
    else if (*my_key_input == KEY_RIGHT)
        px++;
    
 
    //если шаг на стену/границу - возвращение персонажа на исходную позицию
    if (map[py][px] == '%' || (my_role == 'c' && map[py][px] == '#')) {
        px = last_x;
        py = last_y;    
    } //только крыса может проедать стены(но не границы)
    
    //съесть сыр
    if (map[py][px] == '*')
        (*my_cheese)++;
    
    mvaddch(last_y, last_x, ' '); //clear last position
    map[last_y][last_x] = ' ';
    mvaddch(py, px, my_role); //print new position
    map[py][px] = my_role;
    
    my_player->x = px;
    my_player->y = py;
    my_player->role = my_role;
    my_player->last_x = last_x;
    my_player->last_y = last_y;
}


void move_enemy(char **map, struct player *enemy_person, int px, int py)
{
    int ex = enemy_person->x;
    int ey = enemy_person->y;
    int last_x = enemy_person->last_x;
    int last_y = enemy_person->last_y;
    char enemy_role = enemy_person->role;
    
    last_x = ex, last_y = ey;
    
    
    // для кота
    if (enemy_role == 'c') {
        bool up_not_board = (map[(ey)-1][ex] == ' ' || map[(ey)-1][ex] == 'r') ? true : false;
        bool down_not_board = (map[(ey)+1][ex] == ' ' || map[(ey)+1][ex] == 'r') ? true : false;
        bool left_not_board = (map[ey][(ex)-1] == ' ' || map[ey][(ex)-1] == 'r') ? true : false;
        bool right_not_board = (map[ey][(ex)+1] == ' ' || map[ey][(ex)+1] == 'r') ? true : false;
        
        double up_distance_to_rat = hypot(px-(ex), py-((ey)-1));
        double down_distance_to_rat = hypot(px-(ex), py-((ey)+1));
        double left_distance_to_rat = hypot(px-((ex)-1), py-(ey));
        double right_distance_to_rat = hypot(px-((ex)+1), py-(ey));
        
        //усл. вверх
        bool up_less_down = (down_not_board == true) ?
         (up_distance_to_rat <= down_distance_to_rat) : true;
        bool up_less_left = (left_not_board == true) ?
         (up_distance_to_rat <= left_distance_to_rat) : true;
        bool up_less_right = (right_not_board == true) ?
         (up_distance_to_rat <= right_distance_to_rat) : true;
        //усл. вниз
        bool down_less_up = (up_not_board == true) ?
         (down_distance_to_rat <= up_distance_to_rat) : true;
        bool down_less_left = (left_not_board == true) ?
         (down_distance_to_rat <= left_distance_to_rat) : true;
        bool down_less_right = (right_not_board == true) ?
         (down_distance_to_rat <= right_distance_to_rat) : true;
        //усл. влево
        bool left_less_up = (up_not_board == true) ?
         (left_distance_to_rat <= up_distance_to_rat) : true;
        bool left_less_down = (down_not_board == true) ?
         (left_distance_to_rat <= down_distance_to_rat) : true;
        bool left_less_right = (right_not_board == true) ?
         (left_distance_to_rat <= right_distance_to_rat) : true;
        //усл.вправо
        bool right_less_up = (up_not_board == true) ?
         (right_distance_to_rat <= up_distance_to_rat) : true;
        bool right_less_down = (down_not_board == true) ?
         (right_distance_to_rat <= down_distance_to_rat) : true;
        bool right_less_left = (right_not_board == true) ?
         (right_distance_to_rat <= left_distance_to_rat) : true;
     
     
        
        //вверх
        if ((up_not_board == true) && (up_less_down && up_less_left && up_less_right))
            ey--;
        //вниз
        else if ((down_not_board == true) && (down_less_up && down_less_left && down_less_right))
            ey++;
        //влево
        else if ((left_not_board == true) && (left_less_up && left_less_down && left_less_right))
            ex--;
        //вправо
        else if ((right_not_board == true) && (right_less_up && right_less_down && right_less_left))
            ex++;
    }
    //для крысы
    else if (enemy_role == 'r') {
        int r = rand() % 2; //[0; 1]
        //X побег
        if (r && ey != py) {
            if ((ex <= px) && (map[ey][(ex)-1] != '%' && map[ey][(ex)-1] != 'c'))
                (ex)--;
            else if ((ex >= px) && (map[ey][(ex)+1] != '%' && map[ey][(ex)+1] != 'c'))
                (ex)++;
        }
        //Y побег
        else {
            if ((ey <= py) && (map[(ey)-1][ex] != '%' && map[(ey)-1][ex] != 'c'))
                (ey)--;
            else if ((ey >= py) && (map[(ey)+1][ex] != '%' && map[(ey)+1][ex] != 'c'))
                (ey)++;
        }
    }
    
    mvaddch(last_y, last_x, ' '); //clear last position
    map[last_y][last_x] = ' ';
    
    mvaddch(ey, ex, enemy_role); //print new position
    map[ey][ex] = enemy_role;
    
    enemy_person->x = ex;
    enemy_person->y = ey;
    enemy_person->last_x = last_x;
    enemy_person->last_y = last_y;
    enemy_person->role = enemy_role;
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


void print_output_panel(int lvl, int max_lvl, int balls, int balls_to_next_lvl,
int rows, int cols)
{
    mvprintw(rows-1, 1, "lvl: %d/%d                         %d/%d",
     lvl, max_lvl, balls, balls_to_next_lvl);

}


void get_random_xy_in_void_place(char **map, int rows, int cols, int *x, int *y)
{
    do {
            *x = (rand() % (cols - 1)) + 1; //[1; (cols - 1)]
            *y = (rand() % (rows - 1)) + 1; //[1; (rows - 1)]
    } while (map[*y][*x] != ' ');
}


void spawn_cheese(char **map, int number, int rows, int cols)
{
    int r_x, r_y;
    for (int i = 0; i < number; i++) {
        get_random_xy_in_void_place(map, rows, cols, &r_x, &r_y);
        map[r_y][r_x] = '*';
    }

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
    struct player my_player;
    //cтатистика
    int my_lvl = 1;
    int max_lvl = 10;
    int my_balls = 0;
    int balls_to_next_lvl = 5;
    //противники
    int enemy_number = 1;
    struct player enemy_list[enemy_number];
    
    
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
    my_player.role = ((c == '1') ? 'c' : 'r');
    //назначение роли противникам(которая противоположна роли игрока)
    char enemy_role = ((c == '1') ? 'r' : 'c');
    for (int i = 0; i < enemy_number; i++)
        enemy_list[i].role = enemy_role;
    
    
    //прорисовка карты
    drawing_map(map, rows, cols);
    spawn_cheese(map, balls_to_next_lvl, rows, cols);
    print_map(map, rows, cols);
    
    //рандомное место появления (спавн)
    do {
        my_player.x = rand() % cols;
        my_player.y = rand() % rows;
    } while (map[my_player.y][my_player.x] != ' ');
    for (int i = 0; i < enemy_number; i++) {
        do {
            enemy_list[i].x = rand() % cols;
            enemy_list[i].y = rand() % rows;
        } while (map[enemy_list[i].y][enemy_list[i].x] != ' ');
    }
    
    
    
    //точка "следующий уровень" - >
    int x_lvl_point, y_lvl_point;
    get_random_xy_in_void_place(map, rows, cols, &x_lvl_point, &y_lvl_point);
    
    
    //Главный цикл
    do {
        //прорисовка точки "следующий уровень" - >
        mvaddch(y_lvl_point, x_lvl_point, '>');
        
        //передвинуть меня
        bool out_cycle1 = false;
        move_me(map, &my_player, &c, &my_balls);
        for (int i = 0; i < enemy_number; i++) {
            if ((out_cycle1 = fight_if_collision(map, my_player.x, my_player.y, enemy_list[i].x, enemy_list[i].y,
            rows, cols, enemy_list[i].role)) == true)
                break;
        }
        if (out_cycle1 == true)
            break;
            
        
        //передвинуть противников
        bool out_cycle2 = false;
        for (int i = 0; i < enemy_number; i++) {
            move_enemy(map, &enemy_list[i], my_player.x, my_player.y);
            if ((out_cycle2 = fight_if_collision(map, my_player.x, my_player.y, enemy_list[i].x, enemy_list[i].y,
            rows, cols, enemy_list[i].role)) == true)
                break;
        }
        if (out_cycle2 == true)
            break;
            
        //вывод статистики
        print_output_panel(my_lvl, max_lvl, my_balls, balls_to_next_lvl, rows, cols);
        
    } while ((c = getch()) && c != 'q' && c != 27); //27 - ESC
    
        
        
        
        
    for (int i = 0; i < rows; i++) 
        free(map[i]);
    free(map);
    endwin();
    return 0;
}