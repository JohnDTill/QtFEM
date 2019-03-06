#ifndef PHY_ELEMENT_BAR__H
#define PHY_ELEMENT_BAR__H

#include "PhyElement.h"
#include "CFEMTypes_Global.h"

// class PhyElementBar : public PhyElement means PhyElementBar is a (public) subclass of PhyElement. 
// So it has all the functions and data of PhyEelement beside its own functions and data (for example L, A, E for solid bar below)

class PhyElementBar : public PhyElement{
public:
	virtual void setGeometry();
    virtual void setInternalMaterialProperties(VectorXd& pMat);
    virtual void Calculate_ElementStiffness();
	virtual void SpecificOutput(ostream& out) const;
	double L;
	double A;
	double E;
};

#endif
