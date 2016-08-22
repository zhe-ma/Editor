#include "editor/selection_region.h"

namespace editor {

SelectionRegion::SelectionRegion() {
}

SelectionRegion::SelectionRegion(const wxPoint& start_pos, const wxPoint& end_pos)
    : start_pos_(start_pos),
      end_pos_(end_pos) {
}

SelectionRegion::SelectionRegion(const SelectionRegion& region)
    : start_pos_(region.start_pos_),
      end_pos_(region.end_pos_) {
}

SelectionRegion& SelectionRegion::operator=(const SelectionRegion& region) {
  if (this == &region) {
    return *this;
  }

  start_pos_ = region.start_pos_;
  end_pos_ = region.end_pos_;
  return *this;
}

bool SelectionRegion::IsEmpty() const {
  return start_pos_ == end_pos_;
}

}  // namespace editor
