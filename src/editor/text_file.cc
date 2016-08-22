#include "editor/text_file.h"
#include <cassert>
#include <vector>
#include <fstream>
#include <algorithm>
#include "editor/text_line.h"
#include "editor/text_listener.h"
#include "editor/edit_command.h"

namespace editor {

static void SplitString(const wxString& str, wxChar separator, std::vector<wxString>& result) {
  size_t len = str.Len();
  size_t start = 0;
  size_t end = 0;
  wxString sep = wxString(separator);

  for (size_t i = 0; i < len; ++i) {
    if (str[i] == separator) {
      start = end;
      end = i;
      wxString tem = str.SubString(start, end - 1);
      if (!tem.IsEmpty()) {
        result.push_back(tem);
      }
      if (tem != sep) {
        result.push_back(sep);
      }
      ++end;
    }
  }

  if (start == end && !str.IsEmpty()) {
    result.push_back(str);
    end = len;
  }

  if (end != len) {
    result.push_back(str.SubString(end, len - 1));
  }
}

TextFile::TextFile() {
}

TextFile::TextFile(const wxString& path)
    : path_(path) {
}

TextFile::~TextFile() {
  ClearRedoCommands();
  ClearUndoCommands();
}

void TextFile::AttachListener(TextListener* text_listener){
  text_listeners_.push_back(text_listener);
}

void TextFile::DetachListener(TextListener* text_listener){
  std::list<TextListener*>::iterator iter;
  iter = std::find(text_listeners_.begin(), text_listeners_.end(), text_listener);
  if (iter != text_listeners_.end()) {
    text_listeners_.erase(iter);
  }
}

void TextFile::NotifyLineUpdate(const wxPoint& position, bool is_multi_lines){
  //for (std::list<TextListener*>::iterator iter = text_listeners_.begin();
  //     iter != text_listeners_.end();
  //     ++iter) {
  //  (*iter)->OnLineUpdate(position, is_multi_lines);
  //}
  for (TextListener*& text_listener : text_listeners_) {
    text_listener->OnLineUpdate(position, is_multi_lines);
  }
}

void TextFile::DeleteText(const wxPoint& position, const wxString& text) {
  std::vector<wxString> str_vec;
  SplitString(text, '\r', str_vec);

  if (str_vec.empty()) {
    return;
  }

  for (size_t i = 0; i < str_vec.size(); ++i) {
    if (str_vec[i][0] != '\r') {
      DeleteString(position, str_vec[i]);
    } else {
      DeleteLine(position);
    }
  }

  bool is_multi_lines = str_vec.size() > 1 || str_vec[0][0] == '\r';
  NotifyLineUpdate(position, is_multi_lines);
}

void TextFile::InsertText(const wxPoint& position, const wxString& text) {
  wxPoint pos(position);
  std::vector<wxString> str_vec;
  SplitString(text, '\r', str_vec);

  if (str_vec.empty()) {
    return;
  }

  for (size_t i = 0; i < str_vec.size(); ++i) {
    if (str_vec[i][0] != '\r') {
      InsertString(pos, str_vec[i]);
      NotifyLineUpdate(pos, false);  // TODO
    } else {
      InsertLine(pos);
    }
  }
  // TODO
  bool is_multi_lines = str_vec.size() > 1 || str_vec[0][0] == '\r';
  NotifyLineUpdate(pos, is_multi_lines);
}

bool TextFile::Read() {
  std::ifstream ifs(path_.fn_str(), std::ios::in);
  if (!ifs.is_open() || !ifs.good()) {
    return false;
  }
  const size_t BUFFER_SIZE = 4096;
  wxString text;
  char temp[BUFFER_SIZE] = "";
  while (!ifs.eof()) {
    ifs.read(temp, BUFFER_SIZE);
    text.Append(temp, static_cast<size_t>(ifs.gcount()));
  }
  ifs.close();
  ParseTextData(text);

  return true;
}

void TextFile::ParseTextData(const wxString& text) {
  wxString::const_iterator start = text.begin();
  wxString::const_iterator end = text.end();
  for (wxString::const_iterator it = start; it != end; it++) {
    const wxChar ch = *it;
    LineEndType line_type = LINE_END_TYPE_NONE;
    if (ch == '\r') {
      wxString::const_iterator next = it + 1;
      if (next != end && *next == '\n') {
        line_type = LINE_END_TYPE_DOS;
      } else {
        line_type = LINE_END_TYPE_MAC;
      }
      AddLine(wxString(start, it), line_type);
      if (line_type == LINE_END_TYPE_DOS) {
        it++;
      }
      start = it + 1;
    } 
    if (ch == '\n'){
      line_type = LINE_END_TYPE_UNIX;
      AddLine(wxString(start, it), line_type);
      start = it + 1;
    }
  }

  wxString last_line(start, end);
  AddLine(last_line, LINE_END_TYPE_NONE);
}

bool TextFile::Write() {
  return Write(path_);
}

//bool TextFile::Write(const wxString& path) {
//  std::ofstream ofs(path_.fn_str(), std::ios::out);
//  if (!ofs.is_open() || !ofs.good()) {
//    return false;
//  }
//  wxString temp;
//  for (size_t i = 0; i < text_lines_.size(); ++i) {
//    temp.Append(text_lines_[i]);
//    temp.Append(GetEOL(line_end_types_[i]));
//  }
//  ofs << temp.ToStdString();
//  ofs.close();
//  return true;
//}

bool TextFile::Write(const wxString& path) {
  std::ofstream ofs(path_.fn_str(), std::ios::out);
  if (!ofs.is_open() || !ofs.good()) {
    return false;
  }
  wxString temp;
  //for (size_t i = 0; i < text_lines_.size(); ++i) {
  //  temp.Append(text_lines_[i].GetData());
  //  temp.Append(GetEOL(text_lines_[i].GetLineEndType()));
  //}
  for (TextLine& text_line : text_lines_) {
    temp.Append(text_line.GetData());
    temp.Append(GetEOL(text_line.GetLineEndType()));
  }
  ofs << temp.ToStdString();
  ofs.close();
  return true;
}

wxString TextFile::GetEOL(LineEndType type) const {
  switch (type) {
    case LINE_END_TYPE_NONE:
      return wxEmptyString;
    case LINE_END_TYPE_UNIX:
      return wxT("\n");
    case LINE_END_TYPE_DOS:
      //return wxT("\r\n");
      return wxT("\n");
    case LINE_END_TYPE_MAC:
      return wxT("\r");
    default:
      return wxEmptyString;
  }
}

//size_t TextFile::GetMaxLineSize() const {
//  size_t line_size = 0;
//  for (size_t i = 0; i != text_lines_.size(); ++i) {
//    size_t length = text_lines_[i].Length();
//    line_size = line_size < length ? length : line_size;
//  }
//  return line_size;
//}

size_t TextFile::GetMaxLineSize() const {
  size_t line_size = 0;
  for (size_t i = 0; i != text_lines_.size(); ++i) {
    size_t length = text_lines_[i].Len();
    line_size = line_size < length ? length : line_size;
  }
  return line_size;
}

void TextFile::DeleteString(const wxPoint& position, const wxString& str) {
  wxString& current_line = GetLine(position.y).GetData();
  current_line.Remove(position.x, str.Len());
}

void TextFile::DeleteLine(const wxPoint& position) {
  wxString& current_line = GetLine(position.y).GetData();
  wxString& next_line = GetLine(position.y + 1).GetData();
  current_line.Append(next_line);
  DeleteLine(position.y + 1);
}

void TextFile::InsertString(wxPoint& position, const wxString& str) {
  wxString& current_line = GetLine(position.y).GetData();
  current_line.insert(position.x, str);
  position.x += str.Len();
}

void TextFile::InsertLine(wxPoint& position) {
  wxString& current_line = GetLine(position.y).GetData();
  wxString left_str = current_line.SubString(0, position.x - 1);
  wxString right_str = current_line.SubString(position.x, current_line.Len());
  current_line = left_str;
  InsertLine(right_str, ++position.y);
  position.x = 0;
}

//void TextFile::InsertLine(const wxString& str, size_t n, LineEndType type) {
//  text_lines_.insert(text_lines_.begin() + n, str);
//  line_end_types_.insert(line_end_types_.begin() + n, type);
//}
//
//void TextFile::AddLine(const wxString& str, LineEndType type) {
//  text_lines_.push_back(str);
//  line_end_types_.push_back(type);
//}
//
//void TextFile::DeleteLine(size_t n) {
//  text_lines_.erase(text_lines_.begin() + n);
//  line_end_types_.erase(line_end_types_.begin() + n);
//}

void TextFile::InsertLine(const wxString& str, size_t n, LineEndType type) {
  text_lines_.insert(text_lines_.begin() + n, TextLine(str, type));
}

void TextFile::AddLine(const wxString& str, LineEndType type) {
  text_lines_.push_back(TextLine(str, type));
}

void TextFile::DeleteLine(size_t n) {
  text_lines_.erase(text_lines_.begin() + n);
}

size_t TextFile::GetLineCount() const {
  return text_lines_.size();
}

void TextFile::Execute(EditCommand* command) {
  command->Execute();
  undo_commands_.push_back(command);
  ClearRedoCommands();
}

void TextFile::Redo() {
  if (redo_commands_.empty()) {
    return;
  }

  EditCommand* command = redo_commands_.back();
  command->Execute();
  undo_commands_.push_back(command);
  redo_commands_.pop_back();
}

void TextFile::Undo() {
  if (undo_commands_.empty()) {
    return;
  }

  EditCommand* command = undo_commands_.back();
  command->Undo();
  redo_commands_.push_back(command);
  undo_commands_.pop_back();
}

void TextFile::ClearRedoCommands() {
  for (EditCommand* edit_command : redo_commands_) {
    delete edit_command;
  }
  redo_commands_.clear();
}

void TextFile::ClearUndoCommands() {
  for (EditCommand* edit_command : undo_commands_) {
    delete edit_command;
  }
  undo_commands_.clear();
}

bool TextFile::CanUndo() const {
  return !undo_commands_.empty();
}

bool TextFile::CanRedo() const {
  return !redo_commands_.empty();
}

}  // namespace editor
