#pragma once

#include <ncurses.h>

class CardView {
    WINDOW* board_win;

    void addBorder() const { box(board_win, 0, 0);}

public:
    CardView(int height, int width){
        int xMax, yMax;
        getmaxyx(stdscr, yMax, xMax);
        board_win = newwin(height, width, yMax/2 - height / 2, xMax / 2 - width / 2);
    }

};
