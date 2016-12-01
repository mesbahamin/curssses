#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SECOND 1000000
#define FPS 20

typedef enum
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
} direction;

typedef struct
{
    char symbol;
    int x;
    int y;
    direction d;
} snake;

int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nonl();
    nodelay(stdscr, TRUE);
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);

    snake s;
    s.symbol = 'o';
    s.x = COLS / 2;
    s.y = LINES / 2;

    int input = 0;
    while (1)
    {

        input = getch();

        erase();

        printw("Position: (%d, %d) Total: [%d, %d]", s.x, s.y, COLS, LINES);
        switch (input)
        {
            case KEY_LEFT:
            case 'h':
            {
                s.d = LEFT;
            } break;
            case KEY_RIGHT:
            case 'l':
            {
                s.d = RIGHT;
            } break;
            case KEY_UP:
            case 'k':
            {
                s.d = UP;
            } break;
            case KEY_DOWN:
            case 'j':
            {
                s.d = DOWN;
            } break;
        }

        switch (s.d)
        {
            case LEFT:
            {
                --s.x;
            } break;
            case RIGHT:
            {
                ++s.x;
            } break;
            case UP:
            {
                --s.y;
            } break;
            case DOWN:
            {
                ++s.y;
            } break;
        }

        if (s.y == LINES)
        {
            s.y = 0;
        }
        else if (s.y < 0)
        {
            s.y = LINES - 1;
        }
        if (s.x == COLS)
        {
            s.x = 0;
        }
        else if (s.x < 0)
        {
            s.x = COLS - 1;
        }

        mvaddch(s.y, s.x, s.symbol);

        refresh();
        usleep(SECOND / FPS);
    }

    clear();
    endwin();
    exit(0);

    return 0;
}
