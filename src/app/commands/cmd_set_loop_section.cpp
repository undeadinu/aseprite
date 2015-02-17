// Aseprite
// Copyright (C) 2001-2015  David Capello
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/app.h"
#include "app/commands/command.h"
#include "app/commands/params.h"
#include "app/context_access.h"
#include "app/pref/preferences.h"
#include "app/ui/main_window.h"
#include "app/ui/timeline.h"

namespace app {

class SetLoopSectionCommand : public Command {
public:
  enum class Action { Auto, On, Off };

  SetLoopSectionCommand();
  Command* clone() const override { return new SetLoopSectionCommand(*this); }

protected:
  void onLoadParams(Params* params) override;
  bool onEnabled(Context* context) override;
  void onExecute(Context* context) override;

  Action m_action;
  frame_t m_begin, m_end;
};

SetLoopSectionCommand::SetLoopSectionCommand()
  : Command("SetLoopSection",
            "Set Loop Section",
            CmdRecordableFlag)
  , m_action(Action::Auto)
  , m_begin(0)
  , m_end(0)
{
}

void SetLoopSectionCommand::onLoadParams(Params* params)
{
  std::string action = params->get("action");
  if (action == "on") m_action = Action::On;
  else if (action == "off") m_action = Action::Off;
  else m_action = Action::Auto;

  std::string begin = params->get("begin");
  std::string end = params->get("end");

  m_begin = frame_t(strtol(begin.c_str(), NULL, 10));
  m_end = frame_t(strtol(end.c_str(), NULL, 10));
}

bool SetLoopSectionCommand::onEnabled(Context* ctx)
{
  return ctx->checkFlags(ContextFlags::HasActiveDocument);
}

void SetLoopSectionCommand::onExecute(Context* ctx)
{
  Document* doc = ctx->activeDocument();
  if (!doc)
    return;

  DocumentPreferences& docPref = App::instance()->preferences().document(doc);
  frame_t begin = m_begin;
  frame_t end = m_end;
  bool on = false;

  switch (m_action) {

    case Action::Auto: {
      Timeline::Range range = App::instance()->getMainWindow()->getTimeline()->range();
      if (range.enabled() && (range.frames() > 1)) {
        begin = range.frameBegin();
        end = range.frameEnd();
        on = true;
      }
      else {
        on = false;
      }
      break;
    }

    case Action::On:
      on = true;
      break;

    case Action::Off:
      on = false;
      break;

  }

  if (on) {
    docPref.loop.visible(true);
    docPref.loop.from(begin);
    docPref.loop.to(end);
  }
  else
    docPref.loop.visible(false);
}

Command* CommandFactory::createSetLoopSectionCommand()
{
  return new SetLoopSectionCommand;
}

} // namespace app