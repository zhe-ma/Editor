#include "editor/edit_command_manager.h"
#include "editor/edit_command.h"

namespace editor {

EditCommandManager:: ~EditCommandManager() {
  ClearRedoCommands();
  ClearUndoCommands();
}

void EditCommandManager::Execute(EditCommand* command) {
  command->Execute();
  undo_commands_.push_back(command);
  ClearRedoCommands();
}

void EditCommandManager::Redo() {
  if (redo_commands_.empty()) {
    return;
  }

  EditCommand* command = redo_commands_.back();
  command->Execute();
  undo_commands_.push_back(command);
  redo_commands_.pop_back();
}

void EditCommandManager::Undo() {
  if (undo_commands_.empty()) {
    return;
  }

  EditCommand* command = undo_commands_.back();
  command->Undo();
  redo_commands_.push_back(command);
  undo_commands_.pop_back();
}

bool EditCommandManager::CanUndo() const {
  return !undo_commands_.empty();
}

bool EditCommandManager::CanRedo() const {
  return !redo_commands_.empty();
}

void EditCommandManager::ClearRedoCommands() {
  //for (std::list<EditCommand*>::iterator it = redo_commands_.begin();
  //     it != redo_commands_.end();
  //     ++it) {
  //  delete *it;
  //}

  for (EditCommand*& edit_command : redo_commands_) {
    delete edit_command;
  }
  redo_commands_.clear();
}

void EditCommandManager::ClearUndoCommands() {
  //for (std::list<EditCommand*>::iterator it = undo_commands_.begin();
  //     it != undo_commands_.end();
  //     ++it) {
  //  delete *it;
  //}

  for (EditCommand*& edit_command : undo_commands_) {
    delete edit_command;
  }
  undo_commands_.clear();
}

}
