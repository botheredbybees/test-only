// "$Id$"
//
// Copyright 1998-2006 by Bill Spitzak and others.
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
// Please report all bugs and problems to "fltk-bugs@fltk.org".

#ifndef fltk_Image_h
#define fltk_Image_h

#include <config.h>
#include "Symbol.h"
# undef OPAQUE // fix for windows.h

namespace fltk {

class FL_API Widget;
class FL_API xbmImage;
class DrawImageHelper;

class FL_API Image : public Symbol {

  void* picture;	// system-specific data for the drawn image
  int flags;		// flags indicating the state of picture
  friend class xbmImage;
  friend class DrawImageHelper;
  enum {DRAWN=1, OPAQUE=2, MASK=4, USES_FG=8, USES_BG=16};
  void init(int w=0, int h=0, const char * const * d=0); // initialize object as too many data is here to be inlined

protected:
  // image properties, buffer and palette
  int w_, h_;
  PixelType p_;
  int ld_, count_; // no d_ (depth) on purpose as PixelType is more informative
  const char * const *data_; 
  int nb_data_;	    // number of (const char *) data_lines to be allocated
		    // meaningful only for data (non pixels) buffers
		    // that should be generated by all flk images
  U32  *colors_;    // color palette if any
  int  ncolors_;    // nb of colors
  int  transp_index_; // transparency index in the palette
  bool owned_;	    // is the image responsible for buffer destruction

  void dealloc_data(); // called by destructor: dealloc only owned data

public:
  // Construction / destruction
  Image(const char* name=0) : Symbol(name) {init();}
  Image(int w, int h, const char* name=0, const char * const * d=0) : Symbol(name) {init(w,h,d);}
  ~Image() {dealloc_data();} // unalloc owned img data
  
  // make Image responsible for allocation and destruction of owned data, 
  // and make it a black box implementation
  //   any image allocation / deallocation should be done only here
  //   because we decided not to use a virtual destructor for Symbol 
  //   so there must only one way to achieve that
  uchar * alloc_pixels(int w,int h, PixelType p); // alloc data, sets the pixel type,  and will destroy owned data
  const char ** alloc_data(int size ); 
  void copy_data(); // copy the data if not owned, useful with constant pixmap data array before image manip.
  //!  load the image like in read but no drawing is made thus permitting (generic) buffer parsing
  virtual bool fetch() {return false;}

  // Image Attributes definition
  int w() const { return w_;}
  int width() const {return w_;}
  void w(int W) {w_=W;}
  void width(int W) {w_=W;}

  int h() const {return h_;}
  int height() const {return h_;}
  void h(int H) {h_=H;}
  void height(int H) {h_=H;}

  int d() const {return fltk::depth(p_);}
  int depth() const {return fltk::depth(p_);}

  //////////////////////////////////
  // buffer and palette access API //
  //////////////////////////////////
  void pixel_type(PixelType p ) {p_=p;}
  PixelType pixel_type() const {return p_;}
  //! get  color palette if any
  U32  *colors() const {return colors_;}
  //! set the color palette vector
  void set_colors(U32  *colors, int ncolors) {colors_=colors; ncolors_=ncolors;}
  //! nb of colors in the palette
  int  ncolors() const {return ncolors_;}
  //! set the transparency index and color
  void transp_index(int index, U32 color) {
      if (colors_&& index>=0 && index<ncolors_) colors_[index]=color;transp_index_=index;}
  //! set the transparency index
  void transp_index(int index) {if (colors_&& index>=0 && index<ncolors_) transp_index_=index;}
  //! get the transparency index
  int  transp_index() {return transp_index_;}
  //!  set img line size
  void ld(int LD) {ld_ = LD;}
  //!  set img line size
  void line_size(int ls) {ld_ = ls;} 
  //!  get img line size
  int ld() const {return ld_ ? ld_ : w() * d() ;}
  //!  get img line size
  int line_size() const {return ld();}

  // Accessing, allocating a data/pixel buffer
  void data(const char * const *p, int c=0) {data_ = p; count_ = c; owned_= false;}
  void pixels(const uchar*p, int c=1) {data_ = (const char* const *)p; count_ = c; owned_= false;}
  void own_data(const char * const *p, int c=0) {data(p,c); owned_= true;}
  void own_pixels(const uchar *p, int c=1) {pixels(p,c); owned_= true;}
  const char * const * data() const {return (const char * const * ) data_;}
  const uchar * pixels() const { return ((const uchar*) data_); }
  int count() const {return count_;} // count = 0 means that a data buffer is used

  // Common image methods
  virtual void update() = 0;
  void destroy();
  void setsize(int w, int h);
  void make_current();
  void over(const Rectangle& from, const Rectangle& to) const;

  // Common image algorithms
  virtual void color_average(Color c, float i);
  void inactive() { color_average(fltk::GRAY75, .33f); }

  // Drawing 
  bool drawn() const;
  void redraw() {flags &= ~DRAWN;}
  void draw(int x, int y) const;
  void draw(const Rectangle& r) const {_draw(r);} // un-hide the base class
  //	implementation as Symbol subclass:
  void _draw(const Rectangle&) const;
  void _measure(int& W, int& H) const;
  bool fills_rectangle() const;

  // for back compatability with fltk1 only:
  void label(Widget* o);
};

}

#endif

//
// End of "$Id$".
//
