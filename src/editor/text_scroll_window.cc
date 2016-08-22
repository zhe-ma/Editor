#include "editor/text_scroll_window.h"
#include "wx/sizer.h"
#include "wx/caret.h"
#include "wx/dcclient.h"
#include "wx/msgdlg.h"
#include "editor/line_number_panel.h"
#include "editor/text_file.h"
#include "editor/text_panel.h"
#include "editor/main_frame.h"
#include "editor/edit_command.h"
#include "editor/config.h"

namespace editor {

wxBEGIN_EVENT_TABLE(TextScrollWindow, wxScrolledWindow)
EVT_MENU(wxID_REDO, TextScrollWindow::OnRedo)
EVT_MENU(wxID_UNDO, TextScrollWindow::OnUndo)
EVT_MENU(wxID_PASTE, TextScrollWindow::OnPaste)
EVT_MENU(wxID_COPY, TextScrollWindow::OnCopy)
EVT_MENU(wxID_CUT, TextScrollWindow::OnCut)
wxEND_EVENT_TABLE()

const int kDefaultCaretWidth = 1;

TextScrollWindow::TextScrollWindow(Config* config)
    : config_(config),
      caret_pos_(0, 0),
      selection_start_(0, 0),
      selection_text_(wxT("")),
      line_number_panel_(NULL),
      text_panel_(NULL),
      text_file_(NULL),
      char_width_(0),
      char_height_(0),
      line_padding_(0),
      client_width_(0),
      client_height_(0),
      vscroll_pos_(0),
      hscroll_pos_(0),
      is_file_changed_(false),
      is_new_file_(true) {
}

TextScrollWindow::~TextScrollWindow() {
}

bool TextScrollWindow::Create(wxWindow* parent,
                              wxWindowID winid,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style) {
  wxScrolledWindow::Create(parent, winid, pos, size, style);
  SetBackgroundColour(*wxWHITE);
  InitMenuShortcut();
  SetCharSize();

  LineNumberPanel* line_number_panel= new LineNumberPanel(config_);
  line_number_panel_ = line_number_panel;
  line_number_panel_->Create(this);
  const int kLineNumberPanelWidth = 4 * char_width_;
  line_number_panel_->SetInitialSize(wxSize(kLineNumberPanelWidth, 0));

  TextPanel* text_panel = new TextPanel(config_);
  text_panel_ = text_panel;
  text_panel_->Create(this);
  text_panel_->SetCaret(new wxCaret(text_panel_, kDefaultCaretWidth, char_height_));
  text_panel_->GetCaret()->Show();

  wxBoxSizer* hsizer = new wxBoxSizer(wxHORIZONTAL);
  hsizer->Add(line_number_panel, 0, wxEXPAND);
  hsizer->Add(text_panel, 1, wxEXPAND | wxLEFT, 2);
  SetSizer(hsizer);

  SetScrollRate(char_width_, char_height_);
  SetTargetWindow(text_panel);

  return true;
}

void TextScrollWindow::Init() {
  line_number_panel_ = NULL;
  text_panel_ = NULL;
  text_file_ = NULL;
  char_width_ = 0;
  char_height_ = 0;
  line_padding_ = 0;
  client_width_ = 0;
  client_height_ = 0;
  vscroll_pos_ = 0;
  hscroll_pos_ = 0;
  is_file_changed_ = false;
  is_new_file_ = true;
}

void TextScrollWindow::InitMenuShortcut() {
  const int kEntryCount = 5;
  wxAcceleratorEntry entries[kEntryCount];
  entries[0].Set(wxACCEL_CTRL, (int)'Z', wxID_UNDO);
  entries[1].Set(wxACCEL_CTRL, (int)'Y', wxID_REDO);
  entries[2].Set(wxACCEL_CTRL, (int)'C', wxID_COPY);
  entries[3].Set(wxACCEL_CTRL, (int)'X', wxID_CUT);
  entries[4].Set(wxACCEL_CTRL, (int)'V', wxID_PASTE);
  wxAcceleratorTable accel(kEntryCount, entries);
  SetAcceleratorTable(accel);
}

void TextScrollWindow::HandleLineNumberPaint(wxDC& dc) {
  //size_t line_count = (text_file_ == NULL) ? 1 : text_file_->GetLineCount();
  //for (size_t i = 0; i != line_count; i++) {
  //  wxPoint point = GetViewStart();
  //  wxCoord x = char_width_ / 2 + point.x * char_width_;
  //  wxCoord y = i * char_height_ + line_padding_;
  //  dc.DrawText(wxString::Format(wxT("%u"), i + 1), x, y);
  //}

  size_t line_count = (text_file_ == NULL) ? 1 : text_file_->GetLineCount();
  for (size_t i = 0; i != line_count; ++i) {
    wxPoint point = GetViewStart();
    int panel_width = line_number_panel_->GetSize().GetWidth();
    const int KLeftPadding = 0.5 * char_width_;
    const int kRightPadding = 1.5 * char_width_;
    wxCoord x = panel_width - kRightPadding - char_width_ * CountDigitNumber(i + 1);
    wxCoord y = i * char_height_ + line_padding_;
    dc.DrawText(wxString::Format(wxT("%u"), i + 1), x, y);
  }
}

// TODO : Need to be optimized after syntax highlighting.
void TextScrollWindow::HandleTextPaint(wxDC& dc) {
  if (text_file_ == NULL) {
    return;
  }

  wxColor norm_bg_color(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));;
  wxColor selected_bg_color(0xFFD6AD);

  // Draw white background.
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(norm_bg_color);
  for (size_t i = 0; i != text_file_->GetLineCount(); i++) {
    wxCoord x = 0;
    wxCoord y = i * char_height_ + line_padding_;
    wxCoord width = char_width_ * (text_file_->GetLine(i).Len() + 1);
    wxCoord height = char_height_;
    dc.DrawRectangle(x, y, width, height);
  }

  // Draw selected background.
  dc.SetBrush(selected_bg_color);
  wxPoint start = selection_region_.start_pos();
  wxPoint end = selection_region_.end_pos();
  for (int i = start.y; i <= end.y; i++) {
    wxCoord x = 0;
    wxCoord y = i * char_height_ + line_padding_;
    wxCoord width = char_width_ * (text_file_->GetLine(i).Len() + 1);
    wxCoord height = char_height_;

    if (i == start.y) {
      x = start.x * char_width_;
      if (end.y > start.y) {
        width = width - start.x * char_width_;
      } else {
        width = (end.x - start.x) * char_width_;
      }
      dc.DrawRectangle(x, y, width, height);
    } else if (i == end.y) {
      width = char_width_ * end.x;
      dc.DrawRectangle(x, y, width, height);
    } else {
      dc.DrawRectangle(x, y, width, height);
    }
  }

  // Draw text.
  for (size_t i = 0; i != text_file_->GetLineCount(); i++) {
    wxCoord x = 0;
    wxCoord y = i * char_height_ + line_padding_;
    wxString current_line = (*text_file_)[i].GetData();
    current_line.Replace(wxT("\t"), wxT(" "), true);
    dc.DrawText(current_line, x, y);
  }

  //for (wxRegionIterator upd(text_panel_->GetUpdateRegion()); upd; ++upd) {
  //  wxRect rect = upd.GetRect();
  //  rect.GetBottom();
  //}
    //wxRect selected_rect(wxPoint(10, 1), wxPoint(20, 0));
  //wxRect update_rect(wxPoint(0, 0), wxPoint(0, 3));
  //// TODO : text——coords ————> draw——coords

  //// draw white background
  //dc.SetBrush(normBrush);
  //dc.DrawRectangle(update_rect);

  //// draw selected background
  //for (int i = selected_rect.GetY() * char_height; i <= selected_rect.GetBottomLeft().y * char_height; i += char_height) {
  //  dc.SetBrush(selBrush);

  //}

  //// draw text

  //for (int i = 0; i < 5; i++) {
  //  dc.SetBrush(normBrush);
  //  dc.DrawRectangle(0, 0, char_width * i, char_height);
  //}
  //for (int i = 5; i < 10; i++) {
  //  dc.SetBrush(selBrush);
  //  dc.DrawRectangle(0, 0, char_width * i, char_height);
  //}
  //for (int i = 0; i < 3; i++) {
  //  dc.DrawText(str[i], wxPoint(i, i * char_height));
  //}

  //dc.SetBrush(normBrush);
  //dc.SetTextForeground( wxSystemSettings::GetColour
  //        (wxSYS_COLOUR_WINDOWTEXT));
}

void TextScrollWindow::HandleTextKeyDown(wxKeyEvent& event) {
  if (text_file_ == NULL) {
    SetTextFile(wxT("temp"));
  }
  wxChar ch = event.GetKeyCode();

  if (ch == WXK_UP || ch == WXK_DOWN || ch == WXK_LEFT || ch == WXK_RIGHT ||
      ch == WXK_HOME || ch == WXK_END || ch == WXK_PAGEDOWN || ch == WXK_PAGEUP) {
    MoveCaret(ch);
  } else if (ch == WXK_RETURN || ch == WXK_TAB) {
    InsertChar(ch);
  } else if (ch == WXK_DELETE || ch == WXK_BACK) {
    DeleteChar(ch);
  } else{
    event.Skip();
  }

  UpdateCaret();
  RefreshScrollbars();
}

void TextScrollWindow::HandleTextKeyChar(wxKeyEvent& event) {
  wxChar ch = event.GetUnicodeKey();
  InsertChar(ch);
}

// Handle the TextPanel mouse event

void TextScrollWindow::HandleTextMouseLeftDown(wxMouseEvent& event) {
  if (text_file_ == NULL) {
    return;
  }
  text_panel_->SetFocus();
  caret_pos_ = DeviceCoordsToTextCoords(event.GetPosition());
  selection_start_ = caret_pos_;
  UpdateCaret();
  UpdateSelectionRegion();
}

void TextScrollWindow::HandleTextMouseLeftUp(wxMouseEvent& event) {
  if (text_file_ == NULL) {
    return;
  }

  if (text_panel_->HasCapture()) {
    text_panel_->ReleaseMouse();
  }
}

void TextScrollWindow::HandleTextMouseMotion(wxMouseEvent& event) {
  if (text_file_ == NULL) {
    return;
  }

  if (event.Dragging() && event.LeftIsDown()) {
    caret_pos_ = DeviceCoordsToTextCoords(event.GetPosition());
    UpdateCaret();
    UpdateSelectionRegion();
    if (!text_panel_->HasCapture()) {
      text_panel_->CaptureMouse();
    }
  }
}

bool TextScrollWindow::HasSelection() const {
  return !selection_region_.IsEmpty();
}

void TextScrollWindow::UpdateSelectionRegion() {
  if (selection_start_.y > caret_pos_.y) {
    selection_region_.set_start_pos(caret_pos_);
    selection_region_.set_end_pos(selection_start_);
  } else if (selection_start_.y < caret_pos_.y){
    selection_region_.set_start_pos(selection_start_);
    selection_region_.set_end_pos(caret_pos_);
  } else if (selection_start_.x < caret_pos_.x) {
    selection_region_.set_start_pos(selection_start_);
    selection_region_.set_end_pos(caret_pos_);
  } else {
    selection_region_.set_start_pos(caret_pos_);
    selection_region_.set_end_pos(selection_start_);
  }

  text_panel_->Refresh();
}

void TextScrollWindow::ClearSelectionRegion() {
  wxPoint pos = selection_region_.start_pos();
  selection_region_.set_start_pos(pos);
  selection_region_.set_end_pos(pos);
  text_panel_->Refresh();
}

void TextScrollWindow::UpdateSelectionText() {
  wxPoint start = selection_region_.start_pos();
  wxPoint end = selection_region_.end_pos();

  if (end.y - start.y == 0) {
    wxString& line = text_file_->GetLine(start.y).GetData();
    selection_text_ = line.SubString(start.x, end.x - 1);
    return;
  } 

  selection_text_.Clear();

  for (int i = start.y; i <= end.y; ++i) {
    wxString& line = text_file_->GetLine(i).GetData();
    if (i == start.y) {
      selection_text_.Append(line.SubString(start.x, line.Len()));
      selection_text_.Append('\r', 1);
    } else if (i == end.y) {
      selection_text_.Append(line.SubString(0, end.x - 1));
    } else {
      selection_text_.Append(line);
      selection_text_.Append('\r', 1);
    }
  }
}

void TextScrollWindow::ClearSelectionText() {
  selection_text_.Clear();
}

void TextScrollWindow::DeleteSelectionText() {
  UpdateSelectionText();
  ClearSelectionRegion();
  EditCommand* command = new DeleteTextCommand(text_file_, selection_region_.start_pos(), selection_text_);
  text_file_->Execute(command);
}

// Handle the TextPanel resize event.

void TextScrollWindow::HandleTextSize(wxSizeEvent& event) {
  text_panel_->GetClientSize(&client_width_, &client_height_);
  RefreshScrollbars();
 // RefreshLines(caret_pos_.y, false);
}

// Transform text coords into logical coords.

wxPoint TextScrollWindow::TextCoordsToLogicalCoords(const wxPoint& point) const {
  wxPoint logical_point;
  logical_point.x = point.x * char_width_;
  logical_point.y = point.y * char_height_;
  return logical_point;
}

// Transform device coords into text coords.

wxPoint TextScrollWindow::DeviceCoordsToTextCoords(const wxPoint& point) const {
  wxPoint text_point = point;

  text_point.x = point.x / char_width_ + GetViewStart().x;
  text_point.y = point.y / char_height_ + GetViewStart().y;

  if (text_point.y >= text_file_->GetLineCount()) {
    text_point.y = text_file_->GetLineCount() - 1;
    text_point.x = text_file_->GetLine(text_point.y).Len();
  }
  if (text_point.x > text_file_->GetLine(text_point.y).Len()) {
    text_point.x = text_file_->GetLine(text_point.y).Len();
  }

  CheckTabBounds(text_point);

  return text_point;
}

void TextScrollWindow::OnLineUpdate(const wxPoint& position, bool is_multi_lines) {
  caret_pos_ = position;
  RefreshLines(position.y, is_multi_lines);
  RefreshScrollbars();
  if (is_multi_lines) {
    RefreshLineNumber();
  }
  UpdateCaret();
}

// Refresh display

void TextScrollWindow::UpdateCaret() {
  wxPoint point = GetViewStart();
  int xpos = caret_pos_.x * char_width_ - point.x * char_width_;
  int ypos = (char_height_) * (caret_pos_.y - point.y);

  if (ypos > client_height_ - char_height_) {
    vscroll_pos_ = caret_pos_.y - client_height_ / char_height_ + 1;
    Scroll(hscroll_pos_, vscroll_pos_);
  }
  if (ypos < 0) {
    vscroll_pos_ = caret_pos_.y;
    Scroll(hscroll_pos_, vscroll_pos_);
  }
  if (xpos > client_width_ - char_width_) {
    hscroll_pos_ = caret_pos_.x - client_width_ / char_width_ + 6;
    Scroll(hscroll_pos_, vscroll_pos_);
  }
  if (xpos < 0) {
    hscroll_pos_ = caret_pos_.x - 6;
    if (hscroll_pos_ < 0) {
      hscroll_pos_ = 0;
    }
    Scroll(hscroll_pos_, vscroll_pos_);
  }
  point = GetViewStart();
  xpos = caret_pos_.x * char_width_ - point.x * char_width_;
  ypos = char_height_ * (caret_pos_.y - point.y);
  text_panel_->GetCaret()->Move(xpos, ypos);
}

int TextScrollWindow::CountDigitNumber(int n) const {
  int count = 0;
  while (n / 10 > 0) {
    ++count;
    n /= 10;
  }
  ++count;
  return count;
}

void TextScrollWindow::RefreshLineNumber() {
  static int digit_number= 1;
  int n = text_file_->GetLineCount();
  int count = CountDigitNumber(n);

  if (digit_number != count) {
    const int padding = 2 * char_width_;
    int width = 0;

    if (count > 2) {
      width = padding + count * char_width_;
    } else {
      width = padding * 2;
    }

    line_number_panel_->SetInitialSize(wxSize(width, 0));
    Layout();
    digit_number = count;
  }

  line_number_panel_->Refresh();
}

void TextScrollWindow::RefreshScrollbars() {
  if (text_file_ == NULL) {
    return;
  }
  int width = 0;
  int height = char_height_ * text_file_->GetLineCount() + client_height_;
  int max_column_width = text_file_->GetMaxLineSize() * char_width_;
  if (max_column_width > client_width_ * 2 / 3) {
    width = client_width_ + max_column_width - client_width_ * 2 / 3;
  }
  text_panel_->SetVirtualSize(width, height);
  AdjustScrollbars();
}

void TextScrollWindow::RefreshLines(int line_number, bool is_multi_lines) {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  GetViewStart(&x, &y);
  text_panel_->GetVirtualSize(&width, &height);
  x = 0;
  height = is_multi_lines ? (height - y * char_height_) : char_height_;
  y = (line_number - y) * char_height_;
  y = is_multi_lines ? (y - char_height_) : y;
  text_panel_->RefreshRect(wxRect(x, y, width, height));
}

// Undo/Redo

void TextScrollWindow::OnUndo(wxCommandEvent& event) {
  text_file_->Undo();
}

void TextScrollWindow::OnRedo(wxCommandEvent& event) {
  text_file_->Redo();
}

// Copy/Cut/Paste

void TextScrollWindow::OnCopy(wxCommandEvent& event) {
  //wxMessageBox("1");
  UpdateSelectionText();
}

void TextScrollWindow::OnCut(wxCommandEvent& event) {
  DeleteSelectionText();
}

void TextScrollWindow::OnPaste(wxCommandEvent& event) {
  EditCommand* command = new InsertTextCommand(text_file_, caret_pos_, selection_text_);
  text_file_->Execute(command);
}

// Move caret according to keyboard event.

void TextScrollWindow::MoveCaret(wxChar ch){
   if (ch == WXK_UP) {
    MoveToPrevLine();
  } else if (ch == WXK_DOWN) {
    MoveToNextLine();
  } else if (ch == WXK_LEFT) {
    MoveToPrevChar();
  } else if (ch == WXK_RIGHT) {
    MoveToNextChar();
  } else if (ch == WXK_HOME) {
    MoveToLineHome();
  } else if (ch == WXK_END){
    MoveToLineEnd();
  } else if (ch == WXK_PAGEUP) {
    MoveToPrevPage();
  } else if (ch == WXK_PAGEDOWN) {
    MoveToNextPage();
  }
  ClearSelectionRegion();
}

void TextScrollWindow::MoveToPrevLine() {
  if (caret_pos_.y != 0) {
    size_t cols = text_file_->GetLine(--caret_pos_.y).Len();
    caret_pos_.x = caret_pos_.x > cols ? cols : caret_pos_.x;
    CheckTabBounds(caret_pos_);
  }
}

void TextScrollWindow::MoveToNextLine() {
  if (caret_pos_.y != (text_file_->GetLineCount() - 1)) {
    size_t cols = text_file_->GetLine(++(caret_pos_.y)).Len();
    caret_pos_.x = caret_pos_.x > cols ? cols : caret_pos_.x;
    CheckTabBounds(caret_pos_);
  }
}

void TextScrollWindow::MoveToPrevChar() {
  const wxString& current_line = text_file_->GetLine(caret_pos_.y).GetData();
  if (caret_pos_.x != 0) {
    if (current_line[caret_pos_.x - 1] != wxT('\t')) {
      --(caret_pos_.x);
    } else {
      caret_pos_.x -= config_->tab_size_;
    }
  } else {
    if (caret_pos_.y != 0) {
      caret_pos_.x = (text_file_->GetLine(--caret_pos_.y)).Len();
    }
  }
}

void TextScrollWindow::MoveToNextChar() {
  const wxString& current_line = text_file_->GetLine(caret_pos_.y).GetData();
  if (caret_pos_.x != current_line.Len()) {
    if (current_line[caret_pos_.x] != wxT('\t')) {
      ++(caret_pos_.x);
    } else {
      caret_pos_.x += config_->tab_size_;
    }
  } else {
    if (caret_pos_.y != text_file_->GetLineCount() - 1) {
      caret_pos_.y++;
      caret_pos_.x = 0;
    }
  }
}

void TextScrollWindow::MoveToLineEnd() {
  caret_pos_.x = text_file_->GetLine(caret_pos_.y).Len();
}

void TextScrollWindow::MoveToLineHome() {
  caret_pos_.x = 0;
}

void TextScrollWindow::MoveToPrevPage() {
  wxPoint point = GetViewStart();
  vscroll_pos_ = point.y - client_height_ / char_height_;
  caret_pos_.y -= client_height_ / char_height_;
  CheckRowBounds();
  CheckColumnBounds();
  CheckTabBounds(caret_pos_);
  Scroll(hscroll_pos_, vscroll_pos_);
}

void TextScrollWindow::MoveToNextPage() {
  wxPoint point = GetViewStart();
  vscroll_pos_ = point.y + client_height_ / char_height_;
  caret_pos_.y += client_height_ / char_height_;
  CheckRowBounds();
  CheckColumnBounds();
  CheckTabBounds(caret_pos_);
  Scroll(hscroll_pos_, vscroll_pos_);
}

// check whether the caret is in right position and correct it.

void TextScrollWindow::CheckColumnBounds() {
  const wxString& current_line = text_file_->GetLine(caret_pos_.y).GetData();
  if (caret_pos_.x > current_line.Len()) {
    caret_pos_.x = current_line.Len();
  }
}

void TextScrollWindow::CheckRowBounds() {
  size_t line_count = text_file_->GetLineCount();
  if (vscroll_pos_ < 0 || caret_pos_.y < 0) {
    caret_pos_.y = 0;
    vscroll_pos_ = 0;
  }
  if ((size_t)vscroll_pos_ >= line_count || caret_pos_.y >= line_count) {
    caret_pos_.y = line_count - 1;
    vscroll_pos_ = line_count - 1;
  }
}

void TextScrollWindow::CheckTabBounds(wxPoint& point) const {
  const wxString& current_line = text_file_->GetLine(point.y).GetData();
  size_t cols = current_line.Len();
  if (point.x != cols && current_line[point.x] == wxT('\t')) {
    size_t pos = 0;
    for (pos = point.x; pos != 0 && current_line[pos] == wxT('\t'); --pos) {}
    pos = (pos == 0 && current_line[pos] == wxT('\t')) ? 0 : ++pos;
    size_t tem = (point.x - pos) % config_->tab_size_;
    if (tem != 0) {
      point.x = point.x - tem + config_->tab_size_;
    }
  }
}

void TextScrollWindow::SetTextFile(const wxString& file_name) {
  text_file_ = new TextFile(file_name);

  if (text_file_->Read()) {
    wxString tab(wxT('\t'), config_->tab_size_);
    for (size_t i = 0; i != text_file_->GetLineCount(); ++i) {
      wxString& line = text_file_->GetLine(i).GetData();
      line.Replace(wxT("\t"), tab, true);
    }
  }

  if (text_file_->GetLineCount() == 0) {
    text_file_->AddLine(wxT(""), LINE_END_TYPE_NONE);
  }

  text_file_->AttachListener(this);
  RefreshScrollbars();
  text_panel_->Refresh();
  RefreshLineNumber();
}

bool TextScrollWindow::IsFileChanged() {
  return is_file_changed_;
}

bool TextScrollWindow::IsNewFile() {
  return is_new_file_;
}

void TextScrollWindow::SetFileNew(bool state) {
  is_new_file_ = state;
}

void TextScrollWindow::SetFileChanged(bool state) {
  is_file_changed_ = state;
}

void TextScrollWindow::SaveFile() {
  wxString tab(wxT('\t'), config_->tab_size_);
  for (size_t i = 0; i != text_file_->GetLineCount(); ++i) {
    wxString& line = text_file_->GetLine(i).GetData();
    line.Replace(tab, wxT('\t'), true);
  }
  text_file_->Write();
}

void TextScrollWindow::CloseFile() {
}

void TextScrollWindow::CreateNewFile(const wxString& file_name) {
  TextFile file(file_name);
  size_t line_count = text_file_->GetLineCount();
  if (line_count > 1) {
    file.AddLine((*text_file_)[0].GetData());
  }
  for (size_t n = 1; n != line_count; n++) {
    file.AddLine((*text_file_)[n].GetData(), text_file_->GetLineEndType(n));
  }
  file.Write();

  is_file_changed_ = false;
  is_new_file_ = false;
  SetTextFile(file_name);
}

void TextScrollWindow::InsertChar(wxChar ch) {
  wxString text;

  if (ch > 31 && ch < 127 || ch == WXK_RETURN) {
    text.Append(ch, 1);
  } else if (ch == WXK_TAB) {
    text.Append(ch, config_->tab_size_);
  } else {
  }

  if (HasSelection()) {
    DeleteSelectionText();
    ClearSelectionText();
  }

  EditCommand* command = new InsertTextCommand(text_file_, caret_pos_, text);
  text_file_->Execute(command);
}

void TextScrollWindow::DeleteChar(wxChar ch) {
  if (HasSelection()) {
    DeleteSelectionText();
    ClearSelectionText();
    return;
  }

  if (ch == WXK_BACK) {
    if (caret_pos_ == wxPoint(0, 0)) {
      return;
    }
    MoveToPrevChar();
  }

  wxString text;
  wxString& current_line = text_file_->GetLine(caret_pos_.y).GetData();

  if (caret_pos_.x == current_line.Len()) {
    if (caret_pos_.y == text_file_->GetLineCount() - 1) {
      return;
    }
    text.Append(static_cast<wxChar>(WXK_RETURN), 1);
  } else {
    int size = (ch == WXK_TAB) ? config_->tab_size_ : 1;
    wxChar ch = current_line[caret_pos_.x];
    text.Append(ch, size);
  }

  EditCommand* command = new DeleteTextCommand(text_file_, caret_pos_, text);
  text_file_->Execute(command);
  ClearSelectionRegion();
}

void TextScrollWindow::SetCharSize() {
  SetFont(config_->font_);
  wxClientDC dc(this);
  dc.SetFont(GetFont());
  char_height_ = dc.GetCharHeight() + 2 * line_padding_;
  char_width_ = dc.GetCharWidth();
}

}  // namespace editor
