#ifndef EDITOR_TEXT_LISTENER_H_
#define EDITOR_TEXT_LISTENER_H_
#pragma once

class wxPoint;

namespace editor {

class TextListener {
public:
  virtual ~TextListener() {}

  virtual void OnLineUpdate(const wxPoint& position, bool is_multi_lines) = 0;
};

}  // namespace editor

#endif  // EDITOR_TEXT_LISTENER_H_
