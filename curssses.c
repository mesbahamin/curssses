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
} Direction;

typedef struct Segment Segment;
struct Segment
{
    int x;
    int y;
    Segment *prev;
    Segment *next;
};

typedef struct Snake Snake;
struct Snake
{
    char symbol;
    int length;
    Direction d;
    Segment *head;
    Segment *tail;
};

Snake* snake_init(void)
{
    Segment *first = malloc(sizeof(Segment));
    first->x = COLS / 2;
    first->y = LINES / 2;
    first->prev = 0;
    first->next = 0;

    Snake *s = malloc(sizeof(Snake));
    s->symbol = 'o';
    s->d = RIGHT;
    s->length = 1;
    s->head = first;
    s->tail = first;

    return s;
}

void snake_add_segment(Snake *s)
{
    Segment *new = malloc(sizeof(Segment));
    new->x = s->tail->x;
    new->y = s->tail->y;
    new->prev = s->tail;
    new->next = 0;

    s->tail->next = new;
    s->tail = new;
    s->length++;
}

void snake_move(Snake *s)
{
    // move body
    Segment *current = s->tail;
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

uint64_t get_current_time_ms(void)
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

    Snake *s = snake_init();
    for (int i = 0; i < 50; ++i)
    {
        snake_add_segment(s);
    }

    bool debug = false;
    int input = 0;
    int updates = 0;
    int frames = 0;
    int movements = 0;
    uint64_t lag = 0;
    uint64_t previous_ms = get_current_time_ms();

    while (1)
    {
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

        while (lag >= MS_PER_UPDATE)
        {
            if (updates % UPDATES_PER_MOVEMENT == 0)
            {
                snake_move(s);
                movements++;
            }
            updates++;
            lag -= MS_PER_UPDATE;
        }

        erase();

        if (debug)
        {
            mvprintw(
                0, 0,
                "Frame Time: %dms\nLag: %dms\nUpdates: %d\nFrames: %d\nMovements: %d\n"
                "Screen: [%d, %d]\nSnake Length: %d\nHead: (%d, %d)\nTail: (%d, %d)\n",
                elapsed_ms, lag,
                updates, frames, movements,
                COLS, LINES,
                s->length,
                s->head->x, s->head->y,
                s->tail->x, s->tail->y);
        }

        Segment *current = s->head;
        int i = 0;
        while (current != 0)
        {
            mvaddch(current->y, current->x, s->symbol);
            if (debug)
            {
                mvprintw((i%20)+9, (i/20)*25, "Segment %d (%d, %d)", i+1, current->x, current->y);
            }
            current = current->next;
            i++;
        }

        refresh();
        frames++;
        if (elapsed_ms <= MS_PER_FRAME)
        {
            usleep((MS_PER_FRAME - elapsed_ms) * 1000);
        }
    }

    clear();
    endwin();
    exit(0);

    return 0;
}
