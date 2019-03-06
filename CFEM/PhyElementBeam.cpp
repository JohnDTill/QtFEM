#include "PhyElementBeam.h"
#include "PhyNode.h"

void PhyElementBeam::setInternalMaterialProperties(VectorXd& pMat){
    E = pMat(0);
    I = pMat(1);
}

void PhyElementBeam::setGeometry(){
    L = fabs(eNodePtrs[1]->coordinate(0) - eNodePtrs[0]->coordinate(0));
}

void PhyElementBeam::Calculate_ElementStiffness(){
    a = E * I / pow(L, 3);
    ke.resize(4,4);
    ke <<  12,   6*L,  -12,   6*L,
          6*L, 4*L*L, -6*L, 2*L*L,
          -12,  -6*L,   12,  -6*L,
          6*L, 2*L*L, -6*L, 4*L*L;
    ke *= a;
}

void PhyElementBeam::SpecificOutput(ostream& out) const{  
    Vector4d ae;
    ae << eNodePtrs[0]->dofs[0].value,
          eNodePtrs[0]->dofs[1].value,
          eNodePtrs[1]->dofs[0].value,
          eNodePtrs[1]->dofs[1].value;

    #define Be(xi)  Vector4d(6*xi/(L*L), (-1+3*xi)/L, -6*xi/(L*L), (1+3*xi)/L)
    Vector4d dBe = Vector4d(6/(L*L), 3/L, -6/(L*L), 3*L);

    double shear = 2*E*I/L * dBe.dot(ae);
    out << shear << '\t';

    double moment_left = E*I*Be(-1).dot(ae);
    out << moment_left << '\t';
    double moment_right = E*I*Be(1).dot(ae);
    out << moment_right;

    #undef Be
}
