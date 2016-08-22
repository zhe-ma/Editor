#ifndef EDITOR_SELECTION_REGION_H_
#define EDITOR_SELECTION_REGION_H_
#pragma once

#include "wx/gdicmn.h"

namespace editor {

class SelectionRegion {
public:
  SelectionRegion();
  SelectionRegion(const wxPoint& start_pos, const wxPoint& end_pos);
  SelectionRegion(const SelectionRegion& region);

  SelectionRegion& operator=(const SelectionRegion& region);

  wxPoint start_pos() const { return start_pos_; }
  void set_start_pos(const wxPoint& start_pos) { start_pos_ = start_pos; }

  wxPoint end_pos() const { return end_pos_; }
  void set_end_pos(const wxPoint& end_pos) { end_pos_ = end_pos; }

  bool IsEmpty() const;

private:
  wxPoint start_pos_;
  wxPoint end_pos_;
};

}  // namespace editor

#endif  // EDITOR_SELECTION_REGION_H_
