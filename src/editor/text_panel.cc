#include "editor/text_panel.h"
#include <assert.h>
#include "wx/dc.h"
#include "wx/msgdlg.h"
#include "wx/font.h"
#include "wx/dcbuffer.h"
#include "editor/text_file.h"
#include "editor/config.h"
#include "editor/line_number_panel.h"
#include "editor/text_scroll_window.h"

namespace editor {

wxBEGIN_EVENT_TABLE(TextPanel, wxPanel)
EVT_PAINT(TextPanel::OnPaint)
EVT_SIZE(TextPanel::OnSize)
EVT_KEY_DOWN(TextPanel::OnKeyDown)
EVT_CHAR(TextPanel::OnChar)
EVT_LEFT_DOWN(TextPanel::OnMouseLeftDown)
EVT_MOTION(TextPanel::OnMouseMotion)
EVT_LEFT_UP(TextPanel::OnMouseLeftUp)
wxEND_EVENT_TABLE()

TextPanel::TextPanel(Config* config)
    : config_(config),
      owner_(NULL) {
}

bool TextPanel::Create(TextScrollWindow* parent) {
  owner_ = parent;
  wxPanel::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);
  SetFont(config_->font_);
  SetFocus();
  SetBackgroundColour(*wxWHITE);
  SetBackgroundStyle(wxBG_STYLE_PAINT);
  return true;
}

void TextPanel::ScrollWindow(int dx, int dy, const wxRect* rect) {
  wxPanel::ScrollWindow(dx, dy, rect);
  owner_->GetLineNumberPanel()->ScrollWindow(0, dy, rect);
}

void TextPanel::OnSize(wxSizeEvent& event) {
  owner_->HandleTextSize(event);
  event.Skip();
}

void TextPanel::OnMouseLeftDown(wxMouseEvent& event) {
  owner_->HandleTextMouseLeftDown(event);
  event.Skip();
}

void TextPanel::OnMouseMotion(wxMouseEvent& event) {
  owner_->HandleTextMouseMotion(event);
  event.Skip();
}

void TextPanel::OnMouseLeftUp(wxMouseEvent& event) {
  owner_->HandleTextMouseLeftUp(event);
  event.Skip();
}

void TextPanel::OnPaint(wxPaintEvent& event) {
  wxAutoBufferedPaintDC dc(this);
  dc.Clear();
  owner_->PrepareDC(dc);
  owner_->HandleTextPaint(dc);
}

void TextPanel::OnKeyDown(wxKeyEvent& event) {
  owner_->HandleTextKeyDown(event);
}

void TextPanel::OnChar(wxKeyEvent& event) {
  owner_->HandleTextKeyChar(event);
}

}  // namespace editor
