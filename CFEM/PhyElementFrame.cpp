#include "PhyElementFrame.h"
#include "PhyNode.h"

void PhyElementFrame::setInternalMaterialProperties(VectorXd& pMat)
{
    E = pMat(mpb_E);
    A = pMat(mpb_A);
    I = pMat(mpb_I);
//	eType = 4 Frame matfeng = [E A I] (454d)
}

void PhyElementFrame::setGeometry()
{
    VectorXd *crd0, *crd1;
    crd1 = &eNodePtrs[1]->coordinate;
    crd0 = &eNodePtrs[0]->coordinate;

    int sz = crd1->size();
    if (sz != 2)
        THROW("implementation only for 2D frame");
    double delX, delY;
    delX = (*crd1)(0) - (*crd0)(0);
    delY = (*crd1)(1) - (*crd0)(1);
    L = sqrt(delX * delX + delY * delY);
    c = delX / L;
    s = delY / L;
}


void PhyElementFrame::Calculate_ElementStiffness(){
	a1 = E * A / L;
	a2 = E * I / pow(L, 3);

	//! 1. stiffness matrix in local coordinate system
    kLocalCoordinate.resize(6,6);

    kLocalCoordinate <<  a1,      0,        0, -a1,       0,        0,
                          0,  12*a2,   6*L*a2,   0,  -12*a2,   6*L*a2,
                          0, 6*L*a2, 4*L*L*a2,   0, -6*L*a2, 2*L*L*a2,
                        -a1,      0,        0,  a1,       0,        0,
                          0, -12*a2,  -6*L*a2,   0,   12*a2,  -6*L*a2,
                          0, 6*L*a2, 2*L*L*a2,   0, -6*L*a2, 4*L*L*a2;


	//! 2. Transformation matrix T
    T = MatrixXd::Zero(6, 6);
	T(0, 0) = c;
	T(0, 1) = s;
	T(1, 0) = -s;
	T(1, 1) = c;
	T(2, 2) = 1;
    T.block<3,3>(3,3) = T.block<3,3>(0,0);

    ke = T.transpose()*kLocalCoordinate*T;
}

void PhyElementFrame::SpecificOutput(ostream& out) const{
    VectorXd ae(6);
    ae << eNodePtrs[0]->dofs[0].value,
          eNodePtrs[0]->dofs[1].value,
          eNodePtrs[0]->dofs[2].value,
          eNodePtrs[1]->dofs[0].value,
          eNodePtrs[1]->dofs[1].value,
          eNodePtrs[1]->dofs[2].value;

    VectorXd ae_local = T*ae;

    double tension = A*E/L * (ae_local(3) - ae_local(0));
    out << tension << '\t';

    Vector4d ae_beam = Vector4d(ae_local(1), ae_local(2), ae_local(4), ae_local(5));

    #define Be(xi)  Vector4d(6*xi/(L*L), (-1+3*xi)/L, -6*xi/(L*L), (1+3*xi)/L)
    Vector4d dBe = Vector4d(6/(L*L), 3/L, -6/(L*L), 3*L);

    double shear = 2*E*I/L * dBe.dot(ae_beam);
    out << shear << '\t';

    double moment_left = E*I*Be(-1).dot(ae_beam);
    out << moment_left << '\t';
    double moment_right = E*I*Be(1).dot(ae_beam);
    out << moment_right;

    #undef Be
}
