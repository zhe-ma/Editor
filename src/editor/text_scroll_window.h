#ifndef EDITOR_TEXT_SCROLL_WINDOW_H_
#define EDITOR_TEXT_SCROLL_WINDOW_H_
#pragma once

#include <list>
#include "wx/scrolwin.h"
#include "wx/gdicmn.h"
#include "editor/selection_region.h"
#include "editor/text_listener.h"

namespace editor {

class Config;
class TextFile;
class TextPanel;
class LineNumberPanel;
class EditCommand;

class TextScrollWindow : public wxScrolledWindow, public TextListener {
  wxDECLARE_EVENT_TABLE();
public:
  explicit TextScrollWindow(Config* config);

  ~TextScrollWindow();

  bool Create(wxWindow *parent,
              wxWindowID winid = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxScrolledWindowStyle);

  // TODO : need reconstruct
  void SetTextFile(const wxString& file_name);
  bool IsFileChanged();
  bool IsNewFile();
  void SetFileNew(bool state);
  void SetFileChanged(bool state);
  void CreateNewFile(const wxString& file_name);
  void SaveFile();
  void CloseFile();

  LineNumberPanel* GetLineNumberPanel() { return line_number_panel_; }

  virtual void OnLineUpdate(const wxPoint& position, bool is_multi_lines) override;

private:
  void Init();
  void InitMenuShortcut();

  void OnUndo(wxCommandEvent& event);
  void OnRedo(wxCommandEvent& event);
  void OnCopy(wxCommandEvent& event);
  void OnCut(wxCommandEvent& event);
  void OnPaste(wxCommandEvent& event);

  void SetCharSize();
  void MoveCaret(wxChar ch);
  void UpdateCaret();

  void RefreshScrollbars();
  void RefreshLineNumber();
  void RefreshLines(int line_number, bool is_multi_lines);

  void MoveToPrevLine();
  void MoveToNextLine();
  void MoveToPrevChar();
  void MoveToNextChar();
  void MoveToLineHome();
  void MoveToLineEnd();
  void MoveToPrevPage();
  void MoveToNextPage();

  void InsertChar(wxChar ch);
  void DeleteChar(wxChar ch);

  void CheckColumnBounds();
  void CheckRowBounds();
  void CheckTabBounds(wxPoint& point) const;

  void HandleTextPaint(wxDC& dc);
  void HandleLineNumberPaint(wxDC& dc);
  void HandleTextKeyDown(wxKeyEvent& event);
  void HandleTextKeyChar(wxKeyEvent& event);
  void HandleTextMouseLeftDown(wxMouseEvent& event);
  void HandleTextMouseLeftUp(wxMouseEvent& event);
  void HandleTextMouseMotion(wxMouseEvent& event);
  void HandleTextSize(wxSizeEvent& event);

  wxPoint DeviceCoordsToTextCoords(const wxPoint& point) const;
  wxPoint TextCoordsToLogicalCoords(const wxPoint& point) const;

  bool HasSelection() const;

  void UpdateSelectionRegion();
  void ClearSelectionRegion();

  void UpdateSelectionText();
  void DeleteSelectionText();
  void ClearSelectionText();

  int CountDigitNumber(int n) const;

private:
  friend class LineNumberPanel;
  friend class TextPanel;

  LineNumberPanel* line_number_panel_;
  TextPanel* text_panel_;
  TextFile* text_file_;
  Config* config_;

  bool is_file_changed_;
  bool is_new_file_;

  int char_width_;
  int char_height_;
  int client_width_;
  int client_height_;
  size_t line_padding_;

  int vscroll_pos_;
  int hscroll_pos_;

  wxPoint caret_pos_;
  wxPoint selection_start_;
  SelectionRegion selection_region_;
  wxString selection_text_;
};

}  // namespace editor

#endif  // EDITOR_TEXT_SCROLL_WINDOW_H_
