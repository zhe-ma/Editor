#ifndef EDITOR_TEXT_FILE_H_
#define EDITOR_TEXT_FILE_H_
#pragma once

#include <vector>
#include <list>
#include "wx/string.h"
#include "wx/gdicmn.h"
#include "editor/text_line.h"

namespace editor {

class TextListener;
class EditCommand;
class TextLine;

class TextFile {
public:
  TextFile();
  explicit TextFile(const wxString& path);

  ~TextFile();

  bool Read();
  bool Write();
  bool Write(const wxString& path);

  void Execute(EditCommand* command);
  void Redo();
  void Undo();

  bool CanRedo() const;
  bool CanUndo() const;

  void AttachListener(TextListener* text_listener);
  void DetachListener(TextListener* text_listener);

  void AddLine(const wxString& str, LineEndType type = kDefaultLineEndType);

  void DeleteText(const wxPoint& position, const wxString& text);
  void InsertText(const wxPoint& position, const wxString& text);

  size_t GetMaxLineSize() const;
  size_t GetLineCount() const;
  const wxString& GetPath() const { return path_; }

  //LineEndType GetLineEndType(size_t n) const { return line_end_types_[n].; }

  //wxString& GetLine(size_t n) { return text_lines_[n]; }
  //const wxString& GetLine(size_t n) const { return text_lines_[n]; }

  //wxString& operator[](size_t n) { return text_lines_[n]; }
  //const wxString& operator[](size_t n) const { return text_lines_[n]; }

  LineEndType GetLineEndType(size_t n) const { return text_lines_[n].GetLineEndType(); }

  // return wxString or TextLine ?
  //wxString& GetLine(size_t n) { return text_lines_[n].GetString(); }
  //const wxString& GetLine(size_t n) const { return text_lines_[n].GetString(); }

  //wxString& operator[](size_t n) { return text_lines_[n].GetString(); }
  //const wxString& operator[](size_t n) const { return text_lines_[n].GetString(); }

  TextLine& GetLine(size_t n) { return text_lines_[n]; }
  const TextLine& GetLine(size_t n) const { return text_lines_[n]; }

  TextLine& operator[](size_t n) { return text_lines_[n]; }
  const TextLine& operator[](size_t n) const { return text_lines_[n]; }

private:
  wxString GetEOL(LineEndType type) const;

  void ParseTextData(const wxString& text);

  void NotifyLineUpdate(const wxPoint& position, bool is_multi_lines);

  void InsertLine(const wxString& str, size_t n, LineEndType type = kDefaultLineEndType);
  void InsertLine(wxPoint& position);

  void DeleteLine(const wxPoint& position);
  void DeleteLine(size_t n);

  void InsertString(wxPoint& position, const wxString& str);
  void DeleteString(const wxPoint& position, const wxString& str);

  void ClearRedoCommands();
  void ClearUndoCommands();

private:
  std::list<EditCommand*> redo_commands_;
  std::list<EditCommand*> undo_commands_;

  std::vector<TextLine> text_lines_;

  std::list<TextListener*> text_listeners_;

  wxString path_;
};

}  // namespace editor

#endif  // EDITOR_TEXT_FILE_H_
