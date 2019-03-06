#ifndef PHY_ELEMENT_TRUSS__H
#define PHY_ELEMENT_TRUSS__H

#include "PhyElement.h"

class PhyElementTruss : public PhyElement{
public:
    virtual void setInternalMaterialProperties(VectorXd& pMat);
    virtual void Calculate_ElementStiffness();
	virtual void SpecificOutput(ostream& out) const;

	double L;
	double A;
	double E;
	double c;
	double s;
//private:
	virtual void setGeometry();
};

#endif
