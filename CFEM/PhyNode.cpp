#include "PhyNode.h"
#include "PhyGlobal.h"

PhyNode::PhyNode(ID id, int nndofIn, VectorXd coordinate){
    this->id = id;
    dofs.resize(nndofIn);
    this->coordinate = coordinate;
}

ostream& operator<<(ostream& out, const PhyNode& node){
	out << node.id;
    out << '\t' << node.coordinate.transpose();

	// values
	out << '\n';
    for(const PhyDof& d : node.dofs)
        out << d.value << '\t';

	// force
	out << '\n';
    for(const PhyDof& d : node.dofs)
        out << d.force << '\t';
//	out << '\n';

	if (verbose == true){
		// position
        out << '\n';
        for(const PhyDof& d : node.dofs)
            out << d.pos << '\t';
		out << '\n';

		// prescribed_boolean
        for(const PhyDof& d : node.dofs)
            out << d.is_prescribed << '\t';
	}

	return out;
}



