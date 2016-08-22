#ifndef EDITOR_TEXT_LINE_H_
#define EDITOR_TEXT_LINE_H_
#pragma once

#include "wx/string.h"

namespace editor {

enum LineEndType {
  LINE_END_TYPE_NONE = 0,  // incomplete (the last line of the file only)
  LINE_END_TYPE_UNIX,  // line is terminated with 'LF' = 0xA = 10 = '\n'
  LINE_END_TYPE_DOS,  // line is terminated with 'CR' 'LF'
  LINE_END_TYPE_MAC,  // line is terminated with 'CR' = 0xD = 13 = '\r'
};

const LineEndType kDefaultLineEndType =
#if defined(__WINDOWS__)
  LINE_END_TYPE_DOS;
#elif defined(__UNIX__)
  TEXT_FILE_TYPE_UNIX;
#else
  TEXT_FILE_TYPE_NONE;
  #error  "wxTextBuffer: unsupported platform."
#endif

class TextLine {
public:
  TextLine();
  TextLine(const TextLine& text_line);
  TextLine(const wxString& line_string, LineEndType line_end_type = kDefaultLineEndType);

  TextLine& operator=(const TextLine& text_line);

  wxString& GetData() { return line_data_; }
  const wxString& GetData() const { return line_data_; }

  LineEndType GetLineEndType() const { return line_end_type_; }
  void SetLineEndType(LineEndType type) { line_end_type_ = type; }

  size_t Len() const { return line_data_.Len(); }

private:
  wxString line_data_;
  LineEndType line_end_type_;
};

}  // namespace editor

#endif  // EDITOR_TEXT_LINE_H_
