//
// "$Id$"
//
// Sudoku game using the Fast Light Tool Kit (FLTK).
//
// Copyright 2005 by Michael Sweet.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//
// Default sizes...
//

#define GROUP_SIZE	160
#define CELL_SIZE	50
#define CELL_OFFSET	5
#ifdef __APPLE__
#  define MENU_OFFSET	0
#else
#  define MENU_OFFSET	25
#endif // __APPLE__


// Sudoku cell class...
class SudokuCell : public Fl_Widget {
  bool		readonly_;
  int		value_;
  int		test_value_[4];

  public:

		SudokuCell(int X, int Y, int W, int H);
  void		draw();
  int		handle(int event);
  void		readonly(bool r) { readonly_ = r; redraw(); }
  bool		readonly() const { return readonly_; }
  void		test_value(int v, int n) { test_value_[n] = v; redraw(); }
  int		test_value(int n) const { return test_value_[n]; }
  void		value(int v) {
		  value_ = v;
		  for (int i = 0; i < 4; i ++) test_value_[i] = 0;
		  redraw();
		}
  int		value() const { return value_; }
};

// Create a cell widget
SudokuCell::SudokuCell(int X, int Y, int W, int H)
  : Fl_Widget(X, Y, W, H, 0) {
  value(0);
}


// Draw cell
void
SudokuCell::draw() {
  // Draw the cell box...
  if (readonly()) fl_draw_box(FL_UP_BOX, x(), y(), w(), h(), color());
  else fl_draw_box(FL_DOWN_BOX, x(), y(), w(), h(), color());

  // Draw the cell background...
  if (Fl::focus() == this && !readonly()) {
    Fl_Color c = fl_color_average(FL_SELECTION_COLOR, color(), 0.5f);
    fl_color(c);
    fl_rectf(x() + 4, y() + 4, w() - 8, h() - 8);
    fl_color(fl_contrast(labelcolor(), c));
  } else fl_color(labelcolor());

  // Draw the cell value...
  char s[2];

  s[1] = '\0';

  if (value_) {
    s[0] = value_ + '0';

    fl_font(FL_HELVETICA_BOLD, h() - 10);
    fl_draw(s, x(), y(), w(), h(), FL_ALIGN_CENTER);
  }

  if (test_value_[0]) {
    fl_font(FL_HELVETICA_BOLD, h() / 5);

    s[0] = test_value_[0] + '0';
    fl_draw(s, x() + 5, y() + 5, w() - 10, h() - 10, FL_ALIGN_TOP_LEFT);

    if (test_value_[1]) {
      s[0] = test_value_[1] + '0';
      fl_draw(s, x() + 5, y() + 5, w() - 10, h() - 10, FL_ALIGN_TOP_RIGHT);
    }

    if (test_value_[2]) {
      s[0] = test_value_[2] + '0';
      fl_draw(s, x() + 5, y() + 5, w() - 10, h() - 10, FL_ALIGN_BOTTOM_LEFT);
    }

    if (test_value_[3]) {
      s[0] = test_value_[3] + '0';
      fl_draw(s, x() + 5, y() + 5, w() - 10, h() - 10, FL_ALIGN_BOTTOM_RIGHT);
    }
  }
}


// Handle events in cell
int
SudokuCell::handle(int event) {
  switch (event) {
    case FL_FOCUS :
      if (!readonly()) {
        Fl::focus(this);
	redraw();
	return 1;
      }
      break;

    case FL_UNFOCUS :
      redraw();
      return 1;
      break;

    case FL_PUSH :
      if (!readonly() && Fl::event_inside(this)) {
        Fl::focus(this);
	redraw();
	return 1;
      }
      break;

    case FL_KEYDOWN :
      int key = Fl::event_key() - '0';
      if (key > 0 && key <= 9) {
        if (Fl::event_state() & FL_SHIFT) {
	  int i;

	  for (i = 0; i < 4; i ++) {
	    if (!test_value_[i] || test_value_[i] == key) break;
	  }

	  if (i >= 4) {
	    for (i = 0; i < 3; i ++) test_value_[i] = test_value_[i + 1];
	  }

	  test_value_[i] = key;

	  redraw();
	} else {
	  value(key);
	  do_callback();
	}
	return 1;
      } else if (key == 0 || Fl::event_key() == FL_BackSpace ||
                 Fl::event_key() == FL_Delete) {
        value(0);
	do_callback();
	return 1;
      }
      break;
  }

  return Fl_Widget::handle(event);
}


// Sudoku window class...
class Sudoku : public Fl_Window {
  Fl_Sys_Menu_Bar *menubar_;
  Fl_Group	*grid_;
  char		grid_values_[9][9];
  SudokuCell	*grid_cells_[9][9];
  Fl_Group	*grid_groups_[3][3];
  int		difficulty_;

  static void	check_cb(Fl_Widget *widget, void *);
  static void	close_cb(Fl_Widget *widget, void *);
  static void	diff_cb(Fl_Widget *widget, void *d);
  static void	new_cb(Fl_Widget *widget, void *);
  static void	reset_cb(Fl_Widget *widget, void *);
  static void	solve_cb(Fl_Widget *widget, void *);

  static Fl_Preferences	prefs_;

  public:

	      	Sudoku();

  void		check_game();
  void		load_game();
  void		new_game();
  void		resize(int X, int Y, int W, int H);
  void		save_game();
  void		solve_game();
};


// Preferences/saved state for game...
Fl_Preferences	Sudoku::prefs_(Fl_Preferences::USER, "fltk.org", "sudoku");

// Create a Sudoku game window...
Sudoku::Sudoku()
  : Fl_Window(GROUP_SIZE * 3, GROUP_SIZE * 3 + MENU_OFFSET, "Sudoku")
{
  int i, j, k, m;
  Fl_Group *g;
  SudokuCell *cell;
  static Fl_Menu_Item	items[] = {
    { "&Game", 0, 0, 0, FL_SUBMENU },
    { "&New Game", FL_COMMAND | 'n', new_cb, 0, FL_MENU_DIVIDER },
    { "&Check Game", FL_COMMAND | 'c', check_cb, 0, 0 },
    { "&Solve Game", FL_COMMAND | 's', solve_cb, 0, FL_MENU_DIVIDER },
    { "&Quit", FL_COMMAND | 'q', close_cb, 0, 0 },
    { 0 },
    { "&Difficulty", 0, 0, 0, FL_SUBMENU },
    { "&Easy", FL_COMMAND | '1', diff_cb, (void *)"40", FL_MENU_RADIO },
    { "&Medium", FL_COMMAND | '2', diff_cb, (void *)"32", FL_MENU_RADIO },
    { "&Hard", FL_COMMAND | '3', diff_cb, (void *)"24", FL_MENU_RADIO },
    { "&Impossible", FL_COMMAND | '4', diff_cb, (void *)"16", FL_MENU_RADIO },
    { 0 },
    { 0 }
  };


  // Menubar...
  prefs_.get("difficulty", difficulty_, 40);

  if (difficulty_ == 16) items[10].flags |= FL_MENU_VALUE;
  else if (difficulty_ == 24) items[9].flags |= FL_MENU_VALUE;
  else if (difficulty_ == 32) items[8].flags |= FL_MENU_VALUE;
  else items[7].flags |= FL_MENU_VALUE;

  menubar_ = new Fl_Sys_Menu_Bar(0, 0, 3 * GROUP_SIZE, 25);
  menubar_->menu(items);

  // Create the grids...
  grid_ = new Fl_Group(0, MENU_OFFSET, 3 * GROUP_SIZE, 3 * GROUP_SIZE);

  for (i = 0; i < 3; i ++)
    for (j = 0; j < 3; j ++) {
      g = new Fl_Group(j * GROUP_SIZE, i * GROUP_SIZE + MENU_OFFSET,
		       GROUP_SIZE, GROUP_SIZE);
      g->box(FL_BORDER_BOX);
      g->color(FL_DARK3);

      grid_groups_[i][j] = g;

      for (k = 0; k < 3; k ++)
        for (m = 0; m < 3; m ++) {
	  cell = new SudokuCell(j * GROUP_SIZE + CELL_OFFSET + m * CELL_SIZE,
	                        i * GROUP_SIZE + CELL_OFFSET + k * CELL_SIZE +
				    MENU_OFFSET,
			        CELL_SIZE, CELL_SIZE);
	  cell->callback(reset_cb);
	  grid_cells_[i * 3 + k][j * 3 + m] = cell;
	}

      g->end();
    }

  callback(close_cb);
  resizable(grid_);

  // Restore the previous window dimensions...
  int X, Y, W, H;

  if (prefs_.get("x", X, -1)) {
    prefs_.get("y", Y, -1);
    prefs_.get("width", W, 3 * GROUP_SIZE);
    prefs_.get("height", H, 3 * GROUP_SIZE + MENU_OFFSET);

    resize(X, X, W, H);
  }

  // Load the previous game...
  load_game();
}


// Check for a solution to the game...
void
Sudoku::check_cb(Fl_Widget *widget, void *) {
  ((Sudoku *)(widget->window()))->check_game();
}


// Check if the user has correctly solved the game...
void
Sudoku::check_game() {
  bool empty = false;
  bool correct = true;


  // Check the game for right/wrong answers...
  for (int i = 0; i < 9; i ++)
    for (int j = 0; j < 9; j ++) {
      SudokuCell *cell = grid_cells_[i][j];
      int val = cell->value();

      if (!val) empty = true;

      if (val && grid_values_[i][j] != val) {
        cell->color(FL_YELLOW);
        cell->redraw();
	correct = false;
      }
    }

  if (!empty && correct) {
    // Success!
    solve_game();
    fl_message("Congratulations, you solved the game!");
  }
}


// Close the window, saving the game first...
void
Sudoku::close_cb(Fl_Widget *widget, void *) {
  Sudoku *s = (Sudoku *)(widget->window() ? widget->window() : widget);

  s->save_game();
  s->hide();
}


// Set the level of difficulty...
void
Sudoku::diff_cb(Fl_Widget *widget, void *d) {
  Sudoku *s = (Sudoku *)(widget->window() ? widget->window() : widget);

  s->difficulty_ = atoi((char *)d);
  s->new_game();
}


// Load the game from saved preferences...
void
Sudoku::load_game() {
  // Load the current values and state of each grid...
  memset(grid_values_, 0, sizeof(grid_values_));

  for (int i = 0; i < 9; i ++)
    for (int j = 0; j < 9; j ++) {
      char name[255];
      int val;

      SudokuCell *cell = grid_cells_[i][j];

      sprintf(name, "value%d.%d", i, j);
      if (!prefs_.get(name, val, 0)) {
        i = 9;
	grid_values_[0][0] = 0;
	break;
      }

      grid_values_[i][j] = val;

      sprintf(name, "state%d.%d", i, j);
      prefs_.get(name, val, 0);
      cell->value(val);
 
      sprintf(name, "readonly%d.%d", i, j);
      prefs_.get(name, val, 0);
      cell->readonly(val);

      if (val) cell->color(FL_GRAY);
      else cell->color(FL_LIGHT3);

      sprintf(name, "test0%d.%d", i, j);
      prefs_.get(name, val, 0);
      cell->test_value(val, 0);

      sprintf(name, "test1%d.%d", i, j);
      prefs_.get(name, val, 0);
      cell->test_value(val, 1);

      sprintf(name, "test2%d.%d", i, j);
      prefs_.get(name, val, 0);
      cell->test_value(val, 2);

      sprintf(name, "test3%d.%d", i, j);
      prefs_.get(name, val, 0);
      cell->test_value(val, 3);
    }

  // If we didn't load any values, then create a new game...
  if (!grid_values_[0][0]) new_game();
}


// Create a new game...
void
Sudoku::new_cb(Fl_Widget *widget, void *) {
  ((Sudoku *)(widget->window()))->new_game();
}


// Create a new game...
void
Sudoku::new_game() {
  int i, j, k, m, t, count;


  // Generate a new (valid) Sudoku grid...
  srand(time(NULL));

  memset(grid_values_, 0, sizeof(grid_values_));

  for (i = 0; i < 9; i += 3) {
    for (j = 0; j < 9; j += 3) {
      for (t = 1; t <= 9; t ++) {
	for (count = 0; count < 20; count ++) {
	  k = i + (rand() % 3);
	  m = j + (rand() % 3);
	  if (!grid_values_[k][m]) {
	    int kk;

	    for (kk = 0; kk < k; kk ++)
	      if (grid_values_[kk][m] == t) break;

	    if (kk < k) continue;

	    int mm;

	    for (mm = 0; mm < m; mm ++)
	      if (grid_values_[k][mm] == t) break;

	    if (mm < m) continue;

	    grid_values_[k][m] = t;
	    break;
	  }
	}

	if (count == 20) {
	  // Unable to find a valid puzzle so far, so start over...
	  j = 9;
	  i = -3;
	  memset(grid_values_, 0, sizeof(grid_values_));
	}
      }
    }
  }

  // Start by making all cells editable
  SudokuCell *cell;

  for (i = 0; i < 9; i ++)
    for (j = 0; j < 9; j ++) {
      cell = grid_cells_[i][j];

      cell->value(0);
      cell->readonly(0);
      cell->color(FL_LIGHT3);
    }

  // Show N cells, starting with potential confusing ones...
  count = difficulty_;

  for (i = 0; i < 8; i ++)
    for (j = 0; j < 8; j ++) {
      cell = grid_cells_[i][j];

      for (k = i + 1; k < 9; k ++) {
        if (grid_values_[i][j] == grid_values_[k][j + 1] &&
	    grid_values_[i][j + 1] == grid_values_[k][j]) {
	  cell->value(grid_values_[i][j]);
	  cell->readonly(1);
	  cell->color(FL_GRAY);

	  count --;
	}
      }

      for (m = j + 1; m < 9; m ++) {
        if (!cell->readonly() &&
	    grid_values_[i][j] == grid_values_[i + 1][m] &&
	    grid_values_[i + 1][m] == grid_values_[i][m]) {
	  cell->value(grid_values_[i][j]);
	  cell->readonly(1);
	  cell->color(FL_GRAY);

	  count --;
	}
      }
    }

  // Now show random fields...
  while (count > 0) {
    i    = rand() % 9;
    j    = rand() % 9;
    cell = grid_cells_[i][j];

    if (!cell->readonly()) {
      cell->value(grid_values_[i][j]);
      cell->readonly(1);
      cell->color(FL_GRAY);

      count --;
    }
  }
}


// Reset widget color to gray...
void
Sudoku::reset_cb(Fl_Widget *widget, void *) {
  widget->color(FL_LIGHT3);
  widget->redraw();
}


// Resize the window...
void
Sudoku::resize(int X, int Y, int W, int H) {
  // Force resizes to keep the proper aspect ratio...
  int M = H - MENU_OFFSET;

  if (M > W) M = W;

  if (W != M || H != (M + MENU_OFFSET)) {
    W = M;
    H = M + MENU_OFFSET;
  }

  // Resize the window...
  Fl_Window::resize(X, Y, W, H);

  // Save the new window geometry...
  prefs_.set("x", X);
  prefs_.set("y", Y);
  prefs_.set("width", W);
  prefs_.set("height", H);
}


// Save the current game state...
void
Sudoku::save_game() {
  // Save the current values and state of each grid...
  for (int i = 0; i < 9; i ++)
    for (int j = 0; j < 9; j ++) {
      char name[255];
      SudokuCell *cell = grid_cells_[i][j];

      sprintf(name, "value%d.%d", i, j);
      prefs_.set(name, grid_values_[i][j]);

      sprintf(name, "state%d.%d", i, j);
      prefs_.set(name, cell->value());

      sprintf(name, "readonly%d.%d", i, j);
      prefs_.set(name, cell->readonly());

      for (int k = 0; k < 4; k ++) {
	sprintf(name, "test%d%d.%d", k, i, j);
	prefs_.set(name, cell->test_value(k));
      }
    }
}


// Solve the puzzle...
void
Sudoku::solve_cb(Fl_Widget *widget, void *) {
  ((Sudoku *)(widget->window()))->solve_game();
}


// Solve the puzzle...
void
Sudoku::solve_game() {
  int i, j;

  for (i = 0; i < 9; i ++)
    for (j = 0; j < 9; j ++) {
      SudokuCell *cell = grid_cells_[i][j];

      cell->value(grid_values_[i][j]);
      cell->readonly(1);
      cell->color(FL_GRAY);
    }
}


// Main entry for game...
int
main(int argc, char *argv[]) {
  Sudoku s;

  s.show(argc, argv);
  return (Fl::run());
}


//
// End of "$Id$".
//
