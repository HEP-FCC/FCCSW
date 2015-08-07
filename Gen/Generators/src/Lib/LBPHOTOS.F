C $Id: $
      SUBROUTINE SetPhotosOutputUnit( OUTPUTUNIT ) 
C-----------------------------------------------------------------------
C  Purpose   : Change the default output unit of photos
C  Arguments : 
C
C  Author    : Patrick Robbe
C  Created   : 2013-05-31
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
C- Local Variables
      INTEGER PHLUN
      COMMON/PHOLUN/PHLUN
      INTEGER OUTPUTUNIT
C-----------------------------------------------------------------------
      PHLUN = OUTPUTUNIT 
  999 RETURN
      END
