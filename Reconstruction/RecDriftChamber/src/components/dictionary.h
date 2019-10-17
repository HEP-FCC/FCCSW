

#include "TVector3.h"
struct hitINFO {
  double DCA;
  double MC_x;
  double MC_y;
  double MC_z;
  double EdepSum;
  double TOF;
  TVector3 hit_start;
  TVector3 hit_end;
  int layerId;
  int wireId;
  int trackNum;
  unsigned long cellId;
  double hitLength;
  double radius;
  double debug_zpos;
}; 
