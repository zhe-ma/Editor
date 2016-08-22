#ifndef EDITOR_EDIT_COMMAND_MANAGER_H_
#define EDITOR_EDIT_COMMAND_MANAGER_H_
#pragma once

#include <list>

namespace editor {

class EditCommand;

class EditCommandManager {
public:
  EditCommandManager() {}
  ~EditCommandManager();

  void Execute(EditCommand* command);
  void Redo();
  void Undo();

  bool CanRedo() const;
  bool CanUndo() const;

private:
  void ClearRedoCommands();
  void ClearUndoCommands();

private:
  std::list<EditCommand*> redo_commands_;
  std::list<EditCommand*> undo_commands_;
};

}  // namespace editor

#endif  // EDITOR_EDIT_COMMAND_MANAGER_H_
