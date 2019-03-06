#ifndef PHY_ELEMENT_Frame__H
#define PHY_ELEMENT_Frame__H

#include "PhyElement.h"
#include "CFEMTypes_Global.h"

// class PhyElementFrame : public PhyElement means PhyElementFrame is a (public) subclass of PhyElement. 
// So it has all the functions and data of PhyEelement beside its own functions and data (for example L, A, E for solid Frame below)

class PhyElementFrame : public PhyElement{
public:
	virtual void setGeometry();
    virtual void setInternalMaterialProperties(VectorXd& pMat);
    virtual void Calculate_ElementStiffness();
	virtual void SpecificOutput(ostream& out) const;
    MatrixXd kLocalCoordinate;	// kbar page 384
    MatrixXd T;	//page 382
	// the following are set in setGeometry
	double c, s;	// cosine and sine of phi used in T matrix
	double L;	// length of the element

	// A, E, I are set in setInternalMaterialProperties
	double A, E, I;	// material properties

	double a1, a2; // element mixed material geometry properties used in stiffness matrix kLocalCoordinate
};

#endif
