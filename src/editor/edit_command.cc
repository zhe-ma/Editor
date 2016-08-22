#include "editor/edit_command.h"
#include "editor/text_file.h"

namespace editor {

// EditCommand

EditCommand::EditCommand(TextFile* text_file, const wxPoint& position, const wxString& text)
    : text_file_(text_file),
      position_(position),
      text_(text) {
}

// DeleteTextCommand

DeleteTextCommand::DeleteTextCommand(TextFile* text_file, const wxPoint& position, const wxString& text)
    : EditCommand(text_file, position, text) {
}

void DeleteTextCommand::Execute() {
  text_file_->DeleteText(position_, text_);
}

void DeleteTextCommand::Undo() {
  text_file_->InsertText(position_, text_);
}

// InsertTextCommand

InsertTextCommand::InsertTextCommand(TextFile* text_file, const wxPoint& position, const wxString& text)
    : EditCommand(text_file, position, text) {
}

void InsertTextCommand::Execute() {
  text_file_->InsertText(position_, text_);
}

void InsertTextCommand::Undo() {
  text_file_->DeleteText(position_, text_);
}

}  // namespace editor
