#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SECOND 1000000
#define FPS 60

typedef enum
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
} direction;

typedef struct segment segment;
struct segment
{
    int x;
    int y;
    segment *prev;
    segment *next;
};

typedef struct snake snake;
struct snake
{
    char symbol;
    direction d;
    int length;
    segment *head;
    segment *tail;
};

snake* snake_init()
{
    segment *first = malloc(sizeof(segment));
    first->x = COLS / 2;
    first->y = LINES / 2;
    first->prev = 0;
    first->next = 0;

    snake *s = malloc(sizeof(snake));
    s->symbol = 'o';
    s->d = RIGHT;
    s->length = 1;
    s->head = first;
    s->tail = first;

    return s;
}

void snake_add_segment(snake *s)
{
    segment *new = malloc(sizeof(segment));
    new->x = s->tail->x;
    new->y = s->tail->y;
    new->prev = s->tail;
    new->next = 0;

    s->tail->next = new;
    s->tail = new;
    s->length++;
}

void snake_move(snake *s)
{
    segment *current = s->tail;
    while (current != s->head)
    {
        current->x = current->prev->x;
        current->y = current->prev->y;
        current = current->prev;
    }

    switch (s->d)
    {
        case LEFT:
        {
            --s->head->x;
        } break;
        case RIGHT:
        {
            ++s->head->x;
        } break;
        case UP:
        {
            --s->head->y;
        } break;
        case DOWN:
        {
            ++s->head->y;
        } break;
    }

    if (s->head->y >= LINES)
    {
        s->head->y = 0;
    }
    else if (s->head->y < 0)
    {
        s->head->y = LINES - 1;
    }
    if (s->head->x >= COLS)
    {
        s->head->x = 0;
    }
    else if (s->head->x < 0)
    {
        s->head->x = COLS - 1;
    }
}

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

    bool debug = false;

    snake *s = snake_init();
    for (int i = 0; i < 50; ++i)
    {
        snake_add_segment(s);
    }

    int input = 0;
    while (1)
    {
        input = getch();

        switch (input)
        {
            case KEY_LEFT:
            case 'h':
            {
                s->d = LEFT;
            } break;
            case KEY_RIGHT:
            case 'l':
            {
                s->d = RIGHT;
            } break;
            case KEY_UP:
            case 'k':
            {
                s->d = UP;
            } break;
            case KEY_DOWN:
            case 'j':
            {
                s->d = DOWN;
            } break;
            case 'p':
            {
                // pause until keypress
                nodelay(stdscr, FALSE);
                getch();
                nodelay(stdscr, TRUE);
            } break;
            case 'i':
            {
                debug = !debug;
            } break;
        }

        erase();

        snake_move(s);

        if (debug)
        {
            mvprintw(
                0, 0,
                "Screen: [%d, %d]\nSnake Length: %d\nHead: (%d, %d)\nTail: (%d, %d)",
                COLS, LINES, s->length, s->head->x, s->head->y, s->tail->x, s->tail->y);
        }

        segment *current = s->head;
        int i = 0;
        while (current != 0)
        {
            mvaddch(current->y, current->x, s->symbol);
            if (debug)
            {
                mvprintw((i%20)+4, (i/20)*25, "Segment %d (%d, %d)", i+1, current->x, current->y);
            }
            current = current->next;
            i++;
        }

        refresh();
        usleep(SECOND / FPS);
    }
    clear();
    endwin();
    exit(0);

    return 0;
}
