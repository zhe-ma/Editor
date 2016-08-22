#ifndef EDITOR_APP_H_
#define EDITOR_APP_H_
#pragma once

#include "wx/app.h"

namespace editor {

class Config;

class App : public wxApp {
public:
  virtual bool OnInit() override;
  virtual int OnExit() override;

private:
  Config* config_;
};

}  // namespace editor

#endif  // EDITOR_APP_H_
