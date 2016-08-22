#ifndef EDITOR_MAIN_FRAME_H_
#define EDITOR_MAIN_FRAME_H_
#pragma once

#include "wx/frame.h"

namespace editor {

class TextScrollWindow;
class Config;

class MainFrame : public wxFrame {
  wxDECLARE_EVENT_TABLE();

public:
  explicit MainFrame(Config* config);
  bool Create(wxWindow* parent, wxWindowID id, const wxString& title);

private:
  void OnOpen(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnSave(wxCommandEvent& event);
  void OnSaveAs(wxCommandEvent& event);

  void InitMenuBar();

private:
  TextScrollWindow* text_scroll_window_;
  Config* config_;
};

}  // namespace editor

#endif  // EDITOR_MAIN_FRAME_H_
