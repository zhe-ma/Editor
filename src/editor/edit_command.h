#ifndef EDITOR_EDIT_COMMAND_H_
#define EDITOR_EDIT_COMMAND_H_
#pragma once

#include "wx/chartype.h"
#include "wx/gdicmn.h"

namespace editor {

class TextFile;

// EditCommand

class EditCommand {
public:
  EditCommand(TextFile* text_file, const wxPoint& position, const wxString& text);
  virtual ~EditCommand() {}

  virtual void Execute() = 0;
  virtual void Undo() = 0;

protected:
  TextFile* text_file_;
  wxPoint position_;
  wxString text_;
};

// DeleteTextCommand
class DeleteTextCommand : public EditCommand {
public:
  DeleteTextCommand(TextFile* text_file, const wxPoint& position, const wxString& text);

  virtual void Execute() override;
  virtual void Undo() override;
};

// InsertTextCommand
class InsertTextCommand : public EditCommand {
public:
  InsertTextCommand(TextFile* text_file, const wxPoint& position, const wxString& text);

  virtual void Execute() override;
  virtual void Undo() override;
};

}  // namespace editor

#endif  // EDITOR_EDIT_COMMAND_H_
