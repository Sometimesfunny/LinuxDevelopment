#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

void display_page(char *filename, char **lines, size_t num_lines, unsigned current_line, int win_height, int win_width, int h_offset, WINDOW *win) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " %s ", filename);
    int line_no;
    for (line_no = 1; line_no < win_height - 1; line_no++) {
        unsigned file_line_index = current_line + line_no - 1;
        if (file_line_index >= num_lines)
            break;
        char *line = lines[file_line_index];
        int line_len = strlen(line);
        if (h_offset >= line_len) {
            mvwprintw(win, line_no, 1, " ");
        } else {
            int display_len = line_len - h_offset;
            if (display_len > win_width - 2)
                display_len = win_width - 2;
            mvwprintw(win, line_no, 1, "%.*s", display_len, line + h_offset);
        }
    }
    wrefresh(win);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: Show filename\n");
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    size_t lines_allocated = 100;
    size_t num_lines = 0;
    char **lines = malloc(sizeof(char*) * lines_allocated);
    if (lines == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            len--;
        }
        char *line = malloc(len + 1);
        if (line == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        strcpy(line, buffer);
        if (num_lines >= lines_allocated) {
            lines_allocated *= 2;
            char **new_lines = realloc(lines, sizeof(char*) * lines_allocated);
            if (new_lines == NULL) {
                perror("Memory reallocation failed");
                exit(EXIT_FAILURE);
            }
            lines = new_lines;
        }
        lines[num_lines++] = line;
    }
    fclose(fp);

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    unsigned max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    unsigned win_height = max_y - 2;
    unsigned win_width = max_x - 2;
    unsigned win_starty = 1;
    unsigned win_startx = 1;

    WINDOW *win = newwin(win_height, win_width, win_starty, win_startx);
    keypad(win, TRUE);

    unsigned current_line = 0;
    int h_offset = 0;
    display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
    int ch;
    while (1) {
        ch = wgetch(win);
        if (ch == 27) { // ESC key
            break;
        } else if (ch == ' ') {
            current_line += win_height - 2;
            if (current_line >= num_lines) {
                if (current_line <= win_height - 2)
                    current_line = 0;
                else
                    current_line = num_lines - (win_height - 2);
            }
            display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
        } else if (ch == KEY_RIGHT) {
            h_offset++;
            display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
        } else if (ch == KEY_LEFT) {
            if (h_offset > 0)
                h_offset--;
            display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
        } else if (ch == KEY_DOWN) {
            if (current_line + 1 < num_lines) {
                current_line++;
                display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
            }
        } else if (ch == KEY_UP) {
            if (current_line > 0) {
                current_line--;
                display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
            }
        } else if (ch == KEY_NPAGE) { // Page Down
            current_line += win_height - 2;
            if (current_line >= num_lines) {
                if (current_line <= win_height - 2)
                    current_line = 0;
                else
                    current_line = num_lines - (win_height - 2);
            }
            display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
        } else if (ch == KEY_PPAGE) { // Page Up
            if (current_line <= win_height - 2)
                current_line = 0;
            else
                current_line -= win_height - 2;
            display_page(filename, lines, num_lines, current_line, win_height, win_width, h_offset, win);
        }
    }

    delwin(win);
    endwin();

    for (size_t i = 0; i < num_lines; i++) {
        free(lines[i]);
    }
    free(lines);

    return 0;
}
