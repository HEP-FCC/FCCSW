C $Id: f77units.F,v 1.2 2007-06-29 13:21:06 ibelyaev Exp $
C ----------------------------------------------------------------------
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
C ----------------------------------------------------------------------
C $Log: not supported by cvs2svn $
C Revision 1.1  2006/10/04 13:36:46  ibelyaev
C  add utilities to open/close Fortran file
C 
C ----------------------------------------------------------------------
      INTEGER FUNCTION f77clfile ( LUN )
C-----------------------------------------------------------------------
C  Purpose   : close the opened F77 file with logical unit LUN
C  Arguments : LUN units to be close 
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER       lun
C-----------------------------------------------------------------------
      CLOSE ( LUN , ERR=100 ) 
      F77CLFILE = LUN 
      RETURN 
 100  CONTINUE 
      WRITE(*,*) 'ERROR in closing file LUN=',LUN
      F77CLFILE = 0
      RETURN
      END
C-----------------------------------------------------------------------
      INTEGER FUNCTION f77onfile ( LUN , CHFILE ) 
C-----------------------------------------------------------------------
C  Purpose   : open "NEW" file with logical unit LUN and name CHFILE 
C  Arguments : LUN units to be close 
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER       LUN
      CHARACTER*(*) CHFILE 
C-----------------------------------------------------------------------
      OPEN( LUN , FILE = CHFILE, STATUS='new' , ERR=100 ) 
      F77ONFILE = LUN 
      RETURN 
 100  CONTINUE
      WRITE(*,*)'ERROR in open "new" file LUN=',LUN,',NAME="',CHFILE,'"'
      F77ONFILE = 0
      RETURN
      END
C-----------------------------------------------------------------------
      INTEGER FUNCTION f77oofile ( LUN , CHFILE ) 
C-----------------------------------------------------------------------
C  Purpose   : open "OLD" file with logical unit LUN and name CHFILE 
C  Arguments : LUN units to be close 
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER       LUN
      CHARACTER*(*) CHFILE
C-----------------------------------------------------------------------
      OPEN( LUN , FILE = CHFILE, STATUS='old' , ERR=100 ) 
      F77OOFILE = LUN
      RETURN 
 100  CONTINUE
      WRITE(*,*)'ERROR in open "old" file LUN=',LUN,',NAME="',CHFILE,'"'
      F77OOFILE = 0
      RETURN
      END
C-----------------------------------------------------------------------
      INTEGER FUNCTION f77oufile ( LUN , CHFILE ) 
C-----------------------------------------------------------------------
C  Purpose   : open "UNKNOWN" file with logical unit LUN and name CHFILE
C 
C  Arguments : LUN units to be close 
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER       LUN
      CHARACTER*(*) CHFILE
C-----------------------------------------------------------------------
      OPEN( LUN , FILE = CHFILE, STATUS='unknown' , ERR=100 ) 
      F77OUFILE= LUN 
      RETURN 
 100  CONTINUE
      WRITE(*,*)'ERROR in open "unknown" file LUN=',LUN,',NAME="',CHFILE
     &  ,'"'
      F77OUFILE = 1 
      RETURN
      END      
C-----------------------------------------------------------------------
      INTEGER FUNCTION f77onffile ( LUN , CHFILE ) 
C-----------------------------------------------------------------------
C  Purpose   : open "NEW" FORMATTED file with logical unit LUN and name
C              CHFILE 
C  Arguments : LUN units to be close 
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER       LUN
      CHARACTER*(*) CHFILE 
C-----------------------------------------------------------------------
      OPEN( LUN , 
     &  FILE = CHFILE, STATUS='new' , FORM='formatted' , ERR =100 ) 
      F77ONFFILE = LUN 
      RETURN 
 100  CONTINUE
      WRITE(*,*)'ERROR in open "new"/"FORMATTED" file LUN=' , 
     &  LUN,',NAME="',CHFILE,'"'
      F77ONFFILE = 0
      RETURN
      END
C-----------------------------------------------------------------------
      INTEGER FUNCTION f77ooffile ( LUN , CHFILE ) 
C-----------------------------------------------------------------------
C  Purpose   : open "OLD" FORMATTED file with logical unit LUN and name
C              CHFILE 
C  Arguments : LUN units to be close 
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER       LUN
      CHARACTER*(*) CHFILE 
C-----------------------------------------------------------------------
      OPEN( LUN , 
     &  FILE = CHFILE, STATUS='old' , FORM='formatted' , ERR =100 ) 
      F77OOFFILE = LUN 
      RETURN 
 100  CONTINUE
      WRITE(*,*)'ERROR in open "old"/"FORMATTED" file LUN=' , 
     &  LUN,',NAME="',CHFILE,'"'
      F77OOFFILE = 0
      RETURN
      END
C-----------------------------------------------------------------------
      INTEGER FUNCTION f77ouffile ( LUN , CHFILE ) 
C-----------------------------------------------------------------------
C  Purpose   : open "UNKNOWN" "FORMATTED" file 
C              with logical unit LUN and name CHFILE 
C  Arguments : LUN units to be close 
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER       LUN
      CHARACTER*(*) CHFILE 
C-----------------------------------------------------------------------
      OPEN( LUN , 
     &  FILE = CHFILE, STATUS='unknown' , FORM='formatted' , ERR =100 ) 
      F77OUFFILE = LUN 
      RETURN 
 100  CONTINUE
      WRITE(*,*)'ERROR in open "unknown"/"FORMATTED" file LUN=' , 
     &  LUN,',NAME="',CHFILE,'"'
      F77OUFFILE = 0
      RETURN
      END
C-----------------------------------------------------------------------
      INTEGER FUNCTION f77GETUNIT ( N ) 
C-----------------------------------------------------------------------
C  Purpose   : search free fortran unit 
C  Arguments : 
C
C  Author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created   : 2007-06-28
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
      INTEGER N 
C-----------------------------------------------------------------------
      INTEGER i
      LOGICAL yes 
C-----------------------------------------------------------------------
      N = 0 
      DO i = 99,10,-1           ! scal all units 
        INQUIRE(unit=i,opened=yes)
        IF ( .not. yes ) THEN
          N = i 
          GO TO 100
        ENDIF
      ENDDO
C-----------------------------------------------------------------------
 100  CONTINUE 
      f77getunit = N 
 999  RETURN
      END
      


C-----------------------------------------------------------------------
C The END 
C-----------------------------------------------------------------------
