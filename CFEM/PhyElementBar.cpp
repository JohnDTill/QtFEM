#include "PhyElementBar.h"
#include "PhyNode.h"


void PhyElementBar::setGeometry(){
	L = fabs(eNodePtrs[1]->coordinate(0) - eNodePtrs[0]->coordinate(0));
}

void PhyElementBar::setInternalMaterialProperties(VectorXd& pMat){
    A = pMat(mpb_A);
    E = pMat(mpb_E);
}

void PhyElementBar::Calculate_ElementStiffness(){
	// compute stiffness matrix:
	ke.resize(2, 2);
	double factor = A * E / L;
	ke(0, 0) = ke(1, 1) = factor;
	ke(1, 0) = ke(0, 1) = -factor;
}

void PhyElementBar::SpecificOutput(ostream& out) const{
    double tension = A*E/L * (eNodePtrs[1]->dofs[0].value - eNodePtrs[0]->dofs[0].value);
    out << tension;
}
