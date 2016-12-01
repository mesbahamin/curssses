#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#define DISP_W 60
#define DISP_H 24

#define HERO_CHAR '@'

int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);

    int hero_x = DISP_W / 2;
    int hero_y = DISP_H / 2;
    for (int y = 0; y < DISP_H; ++y)
    {
        for (int x = 0; x < DISP_W; ++x)
        {
            mvaddch(y, x, '.');
        }
    }
    mvaddch(hero_y, hero_x, HERO_CHAR);
    printw(" (%d, %d)", hero_x, hero_y);
    refresh();

    int input = 0;
    while (1)
    {
        input = getch();
        erase();
        for (int y = 0; y < DISP_H; ++y)
        {
            for (int x = 0; x < DISP_W; ++x)
            {
                mvaddch(y, x, '.');
            }
        }
        switch (input)
        {
            case KEY_LEFT:
            {
                mvaddch(hero_y, --hero_x, HERO_CHAR);
            } break;
            case KEY_RIGHT:
            {
                mvaddch(hero_y, ++hero_x, HERO_CHAR);
            } break;
            case KEY_UP:
            {
                mvaddch(--hero_y, hero_x, HERO_CHAR);
            } break;
            case KEY_DOWN:
            {
                mvaddch(++hero_y, hero_x, HERO_CHAR);
            } break;
        }
        printw(" (%d, %d)", hero_x, hero_y);
        refresh();
    }

    clear();
    endwin();
    exit(0);

    return 0;
}
