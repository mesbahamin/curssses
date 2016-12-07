#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SECOND 1000

#define FPS          60
#define MS_PER_FRAME (SECOND / FPS)

#define UPDATES_PER_SECOND 100
#define MS_PER_UPDATE      (SECOND / UPDATES_PER_SECOND)

#define MOVEMENTS_PER_SECOND 10
#define UPDATES_PER_MOVEMENT (UPDATES_PER_SECOND / MOVEMENTS_PER_SECOND)

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
    // move body
    segment *current = s->tail;
    while (current != s->head)
    {
        current->x = current->prev->x;
        current->y = current->prev->y;
        current = current->prev;
    }

    // move head
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

uint64_t get_current_time_ms()
{
    struct timespec current;
    // TODO(amin): Fallback to other time sources when CLOCK_MONOTONIC is unavailable.
    clock_gettime(CLOCK_MONOTONIC, &current);
    uint64_t milliseconds = ((current.tv_sec * 1000000000) + current.tv_nsec) / 1000000;
    return milliseconds;
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
    int updates = 0;
    int frames = 0;
    int lag = 0;
    uint64_t previous_ms = get_current_time_ms();

    while (1)
    {
        frames++;
        uint64_t current_ms = get_current_time_ms();
        uint64_t elapsed_ms = current_ms - previous_ms;
        previous_ms = current_ms;
        lag += elapsed_ms;

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
                previous_ms = get_current_time_ms();
            } break;
            case 'i':
            {
                debug = !debug;
            } break;
        }


        // TODO(amin): Test on slow computers.
        // Have I sufficiently decoupled simulation speed from processor speed?
        while (lag >= MS_PER_UPDATE)
        {
            if (updates % UPDATES_PER_MOVEMENT == 0)
            {
                snake_move(s);
            }
            updates++;
            lag -= MS_PER_UPDATE;
        }


        erase();

        if (debug)
        {
            mvprintw(
                0, 0,
                "Screen: [%d, %d]\nSnake Length: %d\nHead: (%d, %d)\nTail: (%d, %d)\nFrame Time: %dms\nUpdates: %d\nFrames: %d",
                COLS, LINES, s->length, s->head->x, s->head->y, s->tail->x, s->tail->y, elapsed_ms, updates, frames);
        }

        segment *current = s->head;
        int i = 0;
        while (current != 0)
        {
            mvaddch(current->y, current->x, s->symbol);
            if (debug)
            {
                mvprintw((i%20)+8, (i/20)*25, "Segment %d (%d, %d)", i+1, current->x, current->y);
            }
            current = current->next;
            i++;
        }

        refresh();
        if (elapsed_ms <= MS_PER_FRAME)
        {
            usleep((MS_PER_FRAME - elapsed_ms) * SECOND);
        }
        // TODO(amin): Is this ever needed?
        else
        {
            usleep(MS_PER_FRAME * SECOND);
        }
    }

    clear();
    endwin();
    exit(0);

    return 0;
}
