// generated by Fast Light User Interface Designer (fluid) version 1.00

#include "about_panel.h"

Fl_Window *about_panel=(Fl_Window *)0;

Fl_Group *display_group=(Fl_Group *)0;

static void cb_1998(Fl_Button*, void*) {
  display_group->hide();
copyright_box->show();
}

Fl_Box *copyright_box=(Fl_Box *)0;

static void cb_OK(Fl_Return_Button* o, void*) {
  ((Fl_Window*)(o->parent()))->hide();
}

Fl_Window* make_about_panel(const char *copyright) {
  Fl_Window* w;
  { Fl_Window* o = about_panel = new Fl_Window(309, 258, "about fluid");
    w = o;
    o->box(FL_UP_BOX);
    o->color(97);
    o->selection_color(47);
    w->hotspot(o);
    { Fl_Group* o = display_group = new Fl_Group(10, 10, 289, 240);
      { Fl_Box* o = new Fl_Box(50, 30, 70, 150);
        o->box(FL_ROUND_UP_BOX);
        o->color(14);
        o->selection_color(47);
        o->labelcolor(6);
      }
      { Fl_Box* o = new Fl_Box(50, 10, 90, 40);
        o->box(FL_FLAT_BOX);
        o->color(97);
        o->selection_color(47);
      }
      { Fl_Box* o = new Fl_Box(60, 50, 50, 70);
        o->box(FL_DOWN_BOX);
        o->color(6);
        o->selection_color(47);
      }
      { Fl_Box* o = new Fl_Box(40, 30, 90, 30);
        o->box(FL_ROUND_UP_BOX);
        o->color(6);
        o->selection_color(47);
        o->labelcolor(6);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      }
      { Fl_Box* o = new Fl_Box(10, 150, 150, 90, "fluid");
        o->box(FL_ROUND_UP_BOX);
        o->color(14);
        o->selection_color(47);
        o->labeltype(FL_SHADOW_LABEL);
        o->labelfont(1);
        o->labelsize(40);
        o->labelcolor(6);
      }
      { Fl_Box* o = new Fl_Box(50, 150, 67, 10);
        o->box(FL_FLAT_BOX);
        o->color(14);
        o->selection_color(47);
      }
      { Fl_Box* o = new Fl_Box(79, 160, 15, 15, "` ");
        o->box(FL_OVAL_BOX);
        o->color(6);
        o->selection_color(47);
        o->labelsize(20);
        o->labelcolor(7);
        o->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);
      }
      { Fl_Box* o = new Fl_Box(70, 145, 15, 15, "` ");
        o->box(FL_OVAL_BOX);
        o->color(6);
        o->selection_color(47);
        o->labelsize(20);
        o->labelcolor(7);
        o->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);
      }
      { Fl_Box* o = new Fl_Box(84, 125, 15, 15, "` ");
        o->box(FL_OVAL_BOX);
        o->color(6);
        o->selection_color(47);
        o->labelsize(20);
        o->labelcolor(7);
        o->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);
      }
      { Fl_Box* o = new Fl_Box(150, 27, 140, 113, "FLTK User\nInterface\nDesigner\nVersion 1.0");
        o->box(FL_OVAL_BOX);
        o->color(12);
        o->selection_color(47);
        o->labeltype(FL_SHADOW_LABEL);
        o->labelfont(1);
        o->labelsize(18);
        o->labelcolor(7);
      }
      { Fl_Button* o = new Fl_Button(181, 176, 115, 30, "\251""1998-1999 by\nBill Spitzak and others");
        o->box(FL_THIN_UP_BOX);
        o->labelsize(10);
        o->labelcolor(136);
        o->callback((Fl_Callback*)cb_1998);
      }
      o->end();
    }
    { Fl_Box* o = copyright_box = new Fl_Box(5, 5, 300, 210);
      o->labelsize(8);
      o->labelcolor(6);
      o->align(132|FL_ALIGN_INSIDE);
      o->hide();
      o->label(copyright);
    }
    { Fl_Return_Button* o = new Fl_Return_Button(180, 218, 116, 22, "OK");
      o->labelsize(10);
      o->callback((Fl_Callback*)cb_OK);
    }
    o->set_non_modal();
    o->end();
  }
  return w;
}
