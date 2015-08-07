C $Id: $
      SUBROUTINE SetTaulaOutputUnit( OUTPUTUNIT ) 
C-----------------------------------------------------------------------
C  Purpose   : Change the default output unit of taula
C  Arguments : 
C
C  Author    : Patrick Robbe
C  Created   : 2013-05-31
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
C- Local Variables
      INTEGER OUTPUTUNIT
      COMMON / INOUT / INUT,IOUT
      INTEGER INUT, IOUT
C-----------------------------------------------------------------------
      IOUT = OUTPUTUNIT 
  999 RETURN
      END
