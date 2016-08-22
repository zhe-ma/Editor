#include "editor/main_frame.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/filedlg.h"
#include "wx/log.h"
#include "wx/caret.h"
#include "wx/accel.h"
#include "editor/text_scroll_window.h"

namespace editor {

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
EVT_MENU(wxID_SAVE, MainFrame::OnSave)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(Config* config) 
    :  config_(config),
       text_scroll_window_(NULL) {
}

bool MainFrame::Create(wxWindow* parent, wxWindowID id, const wxString& title) {
  wxFrame::Create(parent, id, title);
  text_scroll_window_ = new TextScrollWindow(config_);
  text_scroll_window_->Create(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
  InitMenuBar();
  return true;
}

void MainFrame::InitMenuBar() {
  wxMenuBar* editor_menu_bar = new wxMenuBar();

  wxMenu* file_menu = new wxMenu();
  file_menu->Append(wxID_OPEN, wxT("Open...\tCtrl+O"));
  file_menu->Append(wxID_SAVE, wxT("Save\tCtrl+S"));
  file_menu->Append(wxID_SAVEAS, wxT("Save As\tCtrl+Alt+S"));
  file_menu->Append(wxID_EXIT, wxT("Exit\tAlt+F4"));
  editor_menu_bar->Append(file_menu, wxT("File"));

  wxMenu* edit_menu = new wxMenu();
  edit_menu->Append(wxID_UNDO, wxT("Undo\tCtrl+Z"));
  edit_menu->Append(wxID_REDO, wxT("Redo\tCtrl+Y"));
  edit_menu->Append(wxID_COPY, wxT("Copy\tCtrl+C"));
  edit_menu->Append(wxID_CUT, wxT("Cut\tCtrl+X"));
  edit_menu->Append(wxID_PASTE, wxT("Paste\tCtrl+V"));
  editor_menu_bar->Append(edit_menu, wxT("Edit"));

  SetMenuBar(editor_menu_bar);
}

void MainFrame::OnOpen(wxCommandEvent& event) {
  wxString caption = wxT("Open");
  wxString wildcard = wxT("All types(*.*) | *.*| \
                          Normal text file(*.txt) | *.txt| \
                          C source file(*.c;*.h) | *.c;*.h| \
                          C++ source file(*.h;*.hpp,;*.cpp;*.cc) | *.h;*.hpp,;*.cpp;*.cc");
  wxString default_dir = wxT("./");
  wxString default_file_name = wxEmptyString;
  wxFileDialog file_dialog(this, caption, default_dir, default_file_name, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);

  if (file_dialog.ShowModal() == wxID_OK) {
    text_scroll_window_->SetTextFile(file_dialog.GetDirectory() + "\\" + file_dialog.GetFilename());
    text_scroll_window_->SetFileNew(false);
  }
}

void MainFrame::OnSave(wxCommandEvent& event) {
  if (text_scroll_window_->IsNewFile()) {
    wxString caption = wxT("Save");
    wxString wildcard = wxT("All types(*.*) | *.*| \
                            Normal text file(*.txt) | *.txt| \
                            C source file(*.c;*.h) | *.c;*.h| \
                            C++ source file(*.h;*.hpp,;*.cpp;*.cc) | *.h;*.hpp,;*.cpp;*.cc");
    wxString default_dir = wxT("./");
    wxString default_file_name = wxEmptyString;
    wxFileDialog file_dialog(this, caption, default_dir, default_file_name, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (file_dialog.ShowModal() == wxID_OK) {
      text_scroll_window_->CreateNewFile(file_dialog.GetDirectory() + "\\" + file_dialog.GetFilename());
    }
  } else {
    text_scroll_window_->SaveFile();
  }
}

void MainFrame::OnSaveAs(wxCommandEvent& event) {
  wxString caption = wxT("Save As");
  wxString wildcard = wxT("All types(*.*) | *.*| \
                          Normal text file(*.txt) | *.txt| \
                          C source file(*.c;*.h) | *.c;*.h| \
                          C++ source file(*.h;*.hpp,;*.cpp;*.cc) | *.h;*.hpp,;*.cpp;*.cc");
  wxString default_dir = wxT("./");
  wxString default_file_name = wxEmptyString;
  wxFileDialog file_dialog(this, caption, default_dir, default_file_name, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

  if (wxID_CANCEL == file_dialog.ShowModal()) {
    return;
  }
}

void MainFrame::OnExit(wxCommandEvent& event) {
  if (!text_scroll_window_->IsFileChanged()) {
    Close();
    return;
  }
  int ret = wxMessageBox(wxT("Save file?"), wxT("Save"), wxYES_NO | wxCANCEL, this);
  if (ret == wxOK) {
    text_scroll_window_->SaveFile();
  }
  if (ret == wxNO) {
    text_scroll_window_->CloseFile();
  }
  if (ret == wxCANCEL) {
    return;
  }
  Close();
}

}  // namespace editor
