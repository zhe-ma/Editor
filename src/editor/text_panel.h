#ifndef EDITOR_TEXT_PANEL_H_
#define EDITOR_TEXT_PANEL_H_
#pragma once

#include "wx/panel.h"
#include "wx/scrolwin.h"

namespace editor {

class TextScrollWindow;
class Config;

class TextPanel : public wxPanel {
  wxDECLARE_EVENT_TABLE();

public:
  explicit TextPanel(Config* config);
  bool Create(TextScrollWindow* parent);

private:
  virtual void ScrollWindow(int dx, int dy, const wxRect* rect) override;

  void OnPaint(wxPaintEvent& event);
  void OnChar(wxKeyEvent& event);
  void OnKeyDown(wxKeyEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMouseLeftDown(wxMouseEvent& event);
  void OnMouseLeftUp(wxMouseEvent& event);
  void OnMouseMotion(wxMouseEvent& event);

private:
  TextScrollWindow* owner_;
  Config* config_;
};

}  // namespace editor

#endif  // EDITOR_TEXT_PANEL_H_
