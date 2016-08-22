#ifndef EDITOR_CONFIG_H_
#define EDITOR_CONFIG_H_
#pragma once

#include <string>
#include <map>
#include "wx/font.h"
#include "wx/string.h"
#include "tinyxml/tinyxml.h"

namespace editor {

class Config {
public:
  Config();
  ~Config();

  bool LoadFile(const wxString& file_path);
  bool SaveFile(const wxString& file_path);

public:
  wxFont font_;
  int tab_size_;
};

}  // namespace editor

#endif  // EDITOR_CONFIG_H_
