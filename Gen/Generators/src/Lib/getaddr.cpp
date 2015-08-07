// $Id: getaddr.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
////////////////////////////////////////////////////////////////////////
// Routine meant to be called from FORTRAN which simply returns the
// address of the (FORTRAN) argument. This should be 64bit-safe.
//
// 1999/01/08 Chris Green (Purdue University)
// 1999/04/29 CG * altered to use void* from long*
////////////////////////////////////////////////////////////////////////

#ifdef WIN32
extern "C" void* __stdcall GETADDR(void* arg) {
  return(arg);
}
#else
extern "C" {
  void* getaddr_(void* arg);
}

void* getaddr_(void* arg) {
  return(arg);
}
#endif
