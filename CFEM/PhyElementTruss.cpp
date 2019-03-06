#include "PhyElementTruss.h"
#include "PhyNode.h"

void PhyElementTruss::setInternalMaterialProperties(VectorXd& pMat){
    E = pMat(mpb_E);
    A = pMat(mpb_A);
}

void PhyElementTruss::setGeometry(){
    VectorXd *crd0, *crd1;
	crd1 = &eNodePtrs[1]->coordinate;
	crd0 = &eNodePtrs[0]->coordinate;

	int sz = crd1->size();
	if (sz != 2) 
		THROW("implementation only for 2D truss");
	double delX, delY;
	delX = (*crd1)(0) - (*crd0)(0); 
	delY = (*crd1)(1) - (*crd0)(1); 
	L = sqrt(delX * delX + delY * delY);
	c = delX / L;
	s = delY / L;
}

void PhyElementTruss::Calculate_ElementStiffness(){
    double a = E*A/L;

    Matrix2d kLocalCoordinate;
    kLocalCoordinate << a, -a,
                       -a,  a;

    MatrixXd T(2,4);
    T << c, s, 0, 0,
         0, 0, c, s;

    ke = T.transpose()*kLocalCoordinate*T;
}

void PhyElementTruss::SpecificOutput(ostream& out) const{
    double tension = A*E/L * (c * (eNodePtrs[1]->dofs[0].value - eNodePtrs[0]->dofs[0].value) + s * (eNodePtrs[1]->dofs[1].value - eNodePtrs[0]->dofs[1].value));
    out << tension;
}
