#ifndef SIMG4COMMON_G4ConstantField_H
#define SIMG4COMMON_G4ConstantField_H

class G4ConstantField: public G4MagneticField
{
friend class G4ConstantMagneticFieldTool;

private:	
	double bX=0;
	double bY=0;
	double bZ=6.*tesla;
	
	double rMax=9*m;
	double zMax=8*m;
	
public:
	void GetFieldValue(const G4double point[4],double* bField ) const
	{
		if (std::sqrt(point[0]*point[0]+point[1]*point[1])<rMax &&
		    std::abs(point[2])<zMax)
		{
			bField[0]=bX;
			bField[1]=bY;
			bField[2]=bZ;
		}
		else bField[0]=bField[1]=bField[2]=0;
	}
};

#endif
