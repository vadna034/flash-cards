// src/tui.cpp
#include <algorithm>
#include <iostream>
#include <ncurses.h>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "db/cardRepo.hpp"
#include "db/collectionRepo.hpp"
#include "db/internal/db.hpp"
#include "db/transaction.hpp"

namespace ui {

struct CardView {
  Card card;
  bool showBack = false;
};

static void init_colors() {
  if (!has_colors())
    return;
  start_color();
  use_default_colors();
  init_pair(1, COLOR_CYAN, -1);   // titles
  init_pair(2, COLOR_YELLOW, -1); // status
  init_pair(3, COLOR_GREEN, -1);  // hints
  init_pair(4, COLOR_WHITE, -1);  // normals
  init_pair(5, COLOR_RED, -1);    // warnings
}

static void draw_boxed(int h, int w, int y, int x,
                       const std::string &title = "") {
  WINDOW *win = newwin(h, w, y, x);
  box(win, 0, 0);
  if (!title.empty()) {
    wattron(win, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(win, 0, 2, (" " + title + " ").c_str());
    wattroff(win, COLOR_PAIR(1) | A_BOLD);
  }
  wrefresh(win);
  delwin(win);
}

static void draw_footer(const std::string &text) {
  int h, w;
  getmaxyx(stdscr, h, w);
  attron(COLOR_PAIR(2));
  mvhline(h - 2, 0, ' ', w);
  mvprintw(h - 2, 1, "%s", text.c_str());
  attroff(COLOR_PAIR(2));
}

static void center_text_block(const std::vector<std::string> &lines) {
  int h, w;
  getmaxyx(stdscr, h, w);
  int bh = static_cast<int>(lines.size());
  int by = std::max(1, (h - bh) / 2);
  for (int i = 0; i < (int)lines.size(); ++i) {
    int x = std::max(2, (w - (int)lines[i].size()) / 2);
    mvprintw(by + i, x, "%s", lines[i].c_str());
  }
}

// -------------------- Deck Browser --------------------
std::optional<std::int64_t> deck_browser(CollectionRepo &collections,
                                         int64_t rootId = 0) {
  int h, w;
  getmaxyx(stdscr, h, w);
  int sel = 0;
  std::vector<Collection> rows = collections.childrenOf(rootId);

  while (true) {
    clear();
    draw_boxed(h, w, 0, 0, "Flashcards — Decks (↑↓ move, Enter open, q quit)");
    for (int i = 0; i < (int)rows.size(); ++i) {
      if (i == sel)
        attron(A_REVERSE);
      mvprintw(2 + i, 2, "%s", rows[i].name.c_str());
      if (i == sel)
        attroff(A_REVERSE);
    }
    draw_footer("q: quit  Enter: open  ↑/↓: move");
    refresh();

    int ch = getch();
    switch (ch) {
    case 'q':
      return std::nullopt;
    case KEY_UP:
      sel = (sel + (int)rows.size() - 1) % (int)rows.size();
      break;
    case KEY_DOWN:
      sel = (sel + 1) % (int)rows.size();
      break;
    case '\n':
    case KEY_ENTER:
      return rows.empty() ? std::nullopt
                          : std::optional<std::int64_t>(rows[sel].id);
    }
  }
}

// -------------------- Study View --------------------
// Simple word-wrap to fit text in the box
static std::vector<std::string> wrap(const std::string &s, int maxw) {
  std::vector<std::string> out;
  if (maxw <= 4) {
    out.push_back(s);
    return out;
  }
  int w = std::max(1, maxw - 4); // side padding margin when centered
  std::string line;
  line.reserve(w);
  std::istringstream iss(s);
  std::string word;
  while (iss >> word) {
    if (line.empty()) {
      if ((int)word.size() <= w)
        line = word;
      else { // hard-break very long word
        int i = 0;
        while (i < (int)word.size()) {
          out.push_back(word.substr(i, w));
          i += w;
        }
        line.clear();
      }
    } else {
      if ((int)line.size() + 1 + (int)word.size() <= w) {
        line += ' ';
        line += word;
      } else {
        out.push_back(line);
        line = word;
      }
    }
  }
  if (!line.empty())
    out.push_back(line);
  if (out.empty())
    out.push_back(std::string());
  return out;
}

static void draw_frame(const std::string &title) {
  int h, w;
  getmaxyx(stdscr, h, w);
  // top border/title
  attron(A_BOLD);
  mvhline(0, 0, ' ', w);
  mvprintw(0, 2, "%s", title.c_str());
  attroff(A_BOLD);
  // bottom help
  mvhline(h - 2, 0, ' ', w);
  mvprintw(h - 2, 2,
           "Space/Enter: flip  n/→: next  p/←: prev  q: back  ?: help");
}

static void draw_centered_block(const std::vector<std::string> &lines) {
  int h, w;
  getmaxyx(stdscr, h, w);
  int total = (int)lines.size();
  int starty = std::max(2, (h - total) / 2);
  for (int i = 0; i < total; ++i) {
    int x = std::max(2, (w - (int)lines[i].size()) / 2);
    mvprintw(starty + i, x, "%s", lines[i].c_str());
  }
}

void study_loop(CardRepo &cardsRepo, int64_t collectionId,
                const std::string &deckName) {
  std::vector<Card> cards = cardsRepo.listByCollection(collectionId);

  clear();
  if (cards.empty()) {
    draw_frame("Study — " + deckName + " [0/0]");
    mvprintw(4, 4, "No cards in this collection.");
    refresh();
    // wait for any key to return
    getch();
    return;
  }

  int idx = 0;
  bool showBack = false;
  bool showHelp = false;

  auto redraw = [&] {
    int h, w;
    getmaxyx(stdscr, h, w);
    clear();
    std::string title = "Study — " + deckName + " [" + std::to_string(idx + 1) +
                        "/" + std::to_string(cards.size()) + "]";
    draw_frame(title);

    if (showHelp) {
      mvprintw(3, 4, "Keys:");
      mvprintw(5, 6, "Space/Enter  Flip card");
      mvprintw(6, 6, "n / →        Next card");
      mvprintw(7, 6, "p / ←        Previous card");
      mvprintw(8, 6, "q            Back to browser");
    } else {
      const std::string &text = showBack ? cards[idx].back : cards[idx].front;
      auto lines = wrap(text, w);
      draw_centered_block(lines);
    }
    refresh();
  };

  redraw();
  while (true) {
    int ch = getch();
    switch (ch) {
    case 'q':
      return;
    case ' ':
    case '\n':
    case KEY_ENTER:
      showBack = !showBack;
      redraw();
      break;
    case 'n':
    case KEY_RIGHT:
      idx = (idx + 1) % (int)cards.size();
      showBack = false;
      redraw();
      break;
    case 'p':
    case KEY_LEFT:
      idx = (idx + (int)cards.size() - 1) % (int)cards.size();
      showBack = false;
      redraw();
      break;
    case '?':
      showHelp = !showHelp;
      redraw();
      break;
    }
  }
}

} // namespace ui

// -------------------- Entry point: integrate repos --------------------
int main(int argc, char **argv) {
  // Init DB + repos (adjust path / rootId as needed)
  Db db("flashcards.db"); // your RAII Db
  db.applyMigrations();
  std::cout << "here" << std::endl;
  CollectionRepo collections(db);
  CardRepo cards(db);
  int64_t rootId = 0;

  // Init ncurses
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  ui::init_colors();

  // Deck selection → study loop
  std::optional<std::int64_t> sel = ui::deck_browser(collections, rootId);
  if (sel) {
    // find name for header (you can add a repo call to fetch by id for display)
    auto c = collections.get(*sel);
    std::string name = c ? c->name : std::to_string(*sel);
    ui::study_loop(cards, *sel, name);
  }

  // Teardown
  endwin();
  return 0;
}
