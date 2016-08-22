#include "editor/line_number_panel.h"
#include "wx/dcbuffer.h"
#include "editor/text_scroll_window.h"
#include "editor/config.h"

namespace editor {

wxBEGIN_EVENT_TABLE(LineNumberPanel, wxPanel)
EVT_PAINT(LineNumberPanel::OnPaint)
wxEND_EVENT_TABLE()

LineNumberPanel::LineNumberPanel(Config* config) : config_(config) {
}

bool LineNumberPanel::Create(TextScrollWindow* parent) {
  owner_ = parent;
  wxPanel::Create(parent, wxID_ANY);
  SetFont(config_->font_);
  SetBackgroundColour(*wxLIGHT_GREY);
  SetBackgroundStyle(wxBG_STYLE_PAINT);
  return true;
}

void LineNumberPanel::OnPaint(wxPaintEvent& event) {
  wxAutoBufferedPaintDC dc(this);
  dc.Clear();
  owner_->PrepareDC(dc);
  owner_->HandleLineNumberPaint(dc);
}

}  // namespace editor
