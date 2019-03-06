#ifndef PHY_NODE__H
#define PHY_NODE__H

#include "CFEMTypes_Global.h"
#include "PhyDof.h"

class PhyNode{

public:
    PhyNode(ID id, int nndofIn, VectorXd coordinate);
	ID id;
    VectorXd coordinate;
    vector<PhyDof> dofs;
};

ostream& operator<<(ostream& out, const PhyNode& node);

#endif
