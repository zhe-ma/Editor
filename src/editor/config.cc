#include "editor/config.h"
#include "wx/ffile.h"
#include <cstdlib>

namespace editor {

const char* kTabSizeKey = "tab_size";
const char* kFontSizeKey = "font_point_size";
const char* kFontNameKey = "font_face_name";
const char* kDefaultFontName = "Consolas";
const int kDefaulTabtSize = 4;
const int kDefaultFontSize = 11;
const int kMaxTabSize = 40;
const int kMaxFontSize = 50;
const int kMinTabSize = 0;
const int kMinFontSize = 0;

Config::Config() : tab_size_(kDefaulTabtSize) {
  font_.SetPointSize(kDefaultFontSize);
  font_.SetFaceName(kDefaultFontName);
}

Config::~Config() {
}

// TODO
bool Config::SaveFile(const wxString& file_path) {
  return true;
}

bool Config::LoadFile(const wxString& file_path) {
  wxFFile file(file_path, wxT("rb"));
  if (!file.IsOpened()) {
    return false;
  }

  TiXmlDocument doc; 
  if (!doc.LoadFile(file.fp())) {
    return false;
  }

  TiXmlNode* root = doc.FirstChildElement("config");
  if (root == NULL) {
    return false;
  }
   
  for (TiXmlNode* child_node = root->FirstChildElement("option");
       child_node != NULL;
       child_node = child_node->NextSiblingElement("option")) {
    TiXmlElement* element = child_node->ToElement();

    const char* key = element->Attribute("key");
    if (key == NULL) {
      continue;
    }

    const char* value = element->Attribute("value");
    if (value == NULL) {
      continue;
    }

    if (!strcmp(kTabSizeKey, key)) {
      int size = atoi(value);
      tab_size_ = (size > kMinTabSize && size < kMaxTabSize) ? size : kDefaulTabtSize;
    } else if (!strcmp(kFontNameKey, key)) {
      font_.SetFaceName(kDefaultFontName);
    } else if (!strcmp(kFontSizeKey, key)) {
      int size = atoi(value);
      int font_point_size = kDefaultFontSize;
      font_point_size = (size > kMinFontSize && size < kMaxFontSize) ? size : kDefaultFontSize;
      font_.SetPointSize(font_point_size);
    }
  }

  return true;
}

}  // namespace editor
