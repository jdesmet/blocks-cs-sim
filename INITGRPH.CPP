//--------------------
// InitGrph.CPP
// Verzorgen grafische
// mode
//--------------------

#include <Graphics.h>
#include <StdIO.h>
#include <ConIO.h>
#include <PROCESS.H>
#include "initgrph.h"

Graphics::Graphics(int GaphDriver,int GraphMode)
{ initgraph(&GaphDriver, &GraphMode, "");
  GraphErr=graphresult();
  if (GraphErr != grOk) throw XGraphError();
  getpalette(&Palette);
}

Graphics::~Graphics()
{ if (GraphErr == grOk) closegraph();
}
