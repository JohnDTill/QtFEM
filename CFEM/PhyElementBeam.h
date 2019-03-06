#ifndef PHY_ELEMENT_Beam__H
#define PHY_ELEMENT_Beam__H

#include "PhyElement.h"
#include "CFEMTypes_Global.h"

// class PhyElementFrame : public PhyElement means PhyElementFrame is a (public) subclass of PhyElement.
// So it has all the functions and data of PhyEelement beside its own functions and data (for example L, A, E for solid Frame below)

class PhyElementBeam : public PhyElement{
public:
    virtual void setGeometry();
    virtual void setInternalMaterialProperties(VectorXd& pMat);
    virtual void Calculate_ElementStiffness();
    virtual void SpecificOutput(ostream& out) const;
    // the following are set in setGeometry
    double L;	// length of the element
    double E, I;	// material properties
    double a; // element mixed material geometry properties used in stiffness matrix kLocalCoordinate
};

#endif
