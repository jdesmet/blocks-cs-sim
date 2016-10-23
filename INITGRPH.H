//--------------
// InitGrph
//--------------

#include <Graphics.h>

class Graphics
{
  int GraphErr;
  struct palettetype Palette;
public:
  Graphics(int GaphDriver=DETECT,int GraphMode=0);
  ~Graphics();
};

class XGraphError
{
public:
  XGraphError(){};
  XGraphError(XGraphError& x){};
};

