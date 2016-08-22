#include "editor/app.h"
#include "wx/stdpaths.h"
#include "wx/filefn.h"
#include "editor/main_frame.h"
#include "editor/config.h"

namespace editor {

DECLARE_APP(App);
IMPLEMENT_APP(App);

const wxString kFilePath = wxT("/config.xml");

bool App::OnInit() {
  wxApp::OnInit();

  wxString file_path;
  wxStandardPaths& paths = wxStandardPaths::Get();
  file_path = paths.GetUserDataDir();

  if (!wxDirExists(file_path)) {
    wxMkDir(file_path);
  }

  file_path = file_path + kFilePath;
  config_ = new Config();
  config_->LoadFile(file_path);

  MainFrame* frame = new MainFrame(config_);
  frame->Create(NULL, wxID_ANY, wxT("Text Editor"));
  frame->Show();

  return true;
}

int App::OnExit() {
  wxApp::OnExit();
  delete config_;
  return 0;
}

}  // namespace editor
