// Aseprite
// Copyright (C) 2019  Igara Studio S.A.
// Copyright (C) 2001-2018  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/cmd/set_palette.h"
#include "app/commands/auto.h"
#include "app/context_access.h"
#include "app/modules/palettes.h"
#include "app/tx.h"
#include "doc/palette.h"
#include "doc/sprite.h"
#include "ui/manager.h"

#include "palette_size.xml.h"

#include <limits>

namespace app {

class PaletteSizeCommand : public AutoLoadParamsCommand {
public:
  PaletteSizeCommand();
  Command* clone() const override { return new PaletteSizeCommand(*this); }

protected:
  void onExecute(Context* context) override;

private:
  AutoParam<int> m_size { this, 0, "size" };
};

PaletteSizeCommand::PaletteSizeCommand()
  : AutoLoadParamsCommand(CommandId::PaletteSize(), CmdRecordableFlag)
{
}

void PaletteSizeCommand::onExecute(Context* context)
{
  ContextReader reader(context);
  frame_t frame = reader.frame();
  Palette palette(*reader.sprite()->palette(frame));
  int ncolors = (m_size() ? m_size(): palette.size());

#ifdef ENABLE_UI
  if (m_size() == 0 && context->isUIAvailable()) {
    app::gen::PaletteSize window;
    window.colors()->setTextf("%d", ncolors);
    window.openWindowInForeground();
    if (window.closer() != window.ok())
      return;

    ncolors = window.colors()->textInt();
  }
#endif

  if (ncolors == palette.size())
    return;

  palette.resize(MID(1, ncolors, std::numeric_limits<int>::max()));

  ContextWriter writer(reader);
  Tx tx(context, "Palette Size", ModifyDocument);
  tx(new cmd::SetPalette(writer.sprite(), frame, &palette));
  tx.commit();

  set_current_palette(&palette, false);
#ifdef ENABLE_UI
  if (context->isUIAvailable())
    ui::Manager::getDefault()->invalidate();
#endif
}

Command* CommandFactory::createPaletteSizeCommand()
{
  return new PaletteSizeCommand;
}

} // namespace app
