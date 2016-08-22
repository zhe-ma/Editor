#include "editor/text_line.h"

namespace editor {

TextLine::TextLine() : line_end_type_(kDefaultLineEndType) {
}

TextLine::TextLine(const TextLine& text_line)
    : line_data_(text_line.line_data_),
      line_end_type_(text_line.line_end_type_) {
}

TextLine::TextLine(const wxString& data, LineEndType line_end_type)
    : line_data_(data),
      line_end_type_(line_end_type) {
}

TextLine& TextLine::operator=(const TextLine& text_line) {
  if (this == &text_line) {
    return *this;
  }

  line_data_ = text_line.line_data_;
  line_end_type_ = text_line.line_end_type_;
  return *this;
}

}  // namespace editor
