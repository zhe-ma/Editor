#ifndef EDITOR_LINE_NUNBER_PANEL_H_
#define EDITOR_LINE_NUNBER_PANEL_H_
#pragma once

#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/font.h"

namespace editor {

class Config;

class TextScrollWindow;

class LineNumberPanel : public wxPanel {
  wxDECLARE_EVENT_TABLE();

public:
  explicit LineNumberPanel(Config* config);
  bool Create(TextScrollWindow* parent);

private:
  void OnPaint(wxPaintEvent& event);

private:
  TextScrollWindow* owner_;
  Config* config_;
};

}  // namespace editor

#endif  // EDITOR_LINE_NUNBER_PANEL_H_
