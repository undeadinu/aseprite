// Aseprite
// Copyright (C) 2019  Igara Studio S.A.
// Copyright (C) 2001-2017  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifndef APP_COMMANDS_CMD_EXPORT_SPRITE_SHEET_H_INCLUDED
#define APP_COMMANDS_CMD_EXPORT_SPRITE_SHEET_H_INCLUDED
#pragma once

#include "app/commands/auto.h"
#include "app/commands/move_thing.h"

#include "enum.h"

namespace app {

  BETTER_ENUM(MoveMaskCommand_Target, int, Boundaries, Content);

  class MoveMaskCommand : public AutoLoadParamsCommand {
  public:
    typedef MoveMaskCommand_Target Target;

    MoveMaskCommand();
    Command* clone() const override { return new MoveMaskCommand(*this); }

    Target getTarget() const { return m_target(); }
    gfx::Point getDelta(Context* context) const;

  protected:
    bool onNeedsParams() const override { return true; }
    void onLoadParams(const Params& params) override;
    bool onEnabled(Context* context) override;
    void onExecute(Context* context) override;
    std::string onGetFriendlyName() const override;

  private:
    AutoParam<Target> m_target { this, +Target::Boundaries, "target" };
    AutoParam<bool> m_wrap { this, false, "wrap" };
    MoveThing m_moveThing;
  };

} // namespace app

#endif
