#include "PhyElement.h"
#include "PhyElementBar.h"
#include "PhyElementTruss.h"
#include "PhyElementBeam.h"
#include "PhyElementFrame.h"
#include "PhyNode.h"
#include "PhyDof.h"
#include "CFEMTypes_Global.h"
#include "PhyGlobal.h"

PhyElement* PhyElementFactory(ElementType eTypeIn, int elem_id, int matID, vector <PhyNode*>& eNodePtrsIn)
{
	PhyElement* pePtr = NULL;
	switch (eTypeIn)
	{
	case etBar:
		pePtr = new PhyElementBar();
		break;

    case etBeam:
        pePtr = new PhyElementBeam();
        break;

	case etTruss:
		pePtr = new PhyElementTruss();
		break;

    case etFrame:
        pePtr = new PhyElementFrame();
        break;

	default:
		THROW("the type is not defined");
	}
	if (pePtr != NULL)
		pePtr->eType = eTypeIn;

    pePtr->id = elem_id;
    pePtr->matID = matID;
    pePtr->setNodeConnectivity_Sizes(eNodePtrsIn);
    pePtr->setGeometry();

	return pePtr;
}

ostream& operator<<(ostream& out, const PhyElement& dat){
	// id ElementType
	out << dat.id << '\t' << dat.eType << '\n';
    if (verbose == true)
        out << dat.fee.transpose() << '\n';

    dat.SpecificOutput(out);

	return out;
}

void PhyElement::setNodeConnectivity_Sizes(vector <PhyNode*>& eNodePtrsIn){
    eNodePtrs = eNodePtrsIn;
}

void PhyElement::calculateElementForces(){
    VectorXd ae(dofMap.size());

    int i = 0;
    for(PhyNode* n : eNodePtrs){
        for(PhyDof d : n->dofs){
            if(d.is_prescribed){
                ae(i++) = d.value;
            }else{
                ae(i++) = 0;
            }
        }
    }

    fde = ke*ae;
    fee = -fde;
}

void PhyElement::insertOrIncrementDiagonal(SparseMatrix<double>& mat, double val, int row){
    //Insertion takes longer compared to dense matrices, but storage and solving are better (if the problem is really sparse)
    for(SparseMatrix<double>::InnerIterator it(mat, row); it; ++it){
        if(it.row()==row){
            mat.coeffRef(row,row) += val;
            return;
        }
    }
    mat.insert(row,row) = val;
}

void PhyElement::insertOrIncrementSymmetric(SparseMatrix<double>& mat, double val, int row, int col){
    //Sets both elements of the symmetric matrix.
    //Initial attempts to exploit triangular storage did not quickly find traction, but it would be a good optimization.

    for(SparseMatrix<double>::InnerIterator it(mat, col); it; ++it){
        if(it.row()==row){
            mat.coeffRef(row,col) += val;
            mat.coeffRef(col,row) += val;
            return;
        }
    }
    mat.insert(row,col) = val;
    mat.insert(col,row) = val;
}

void PhyElement::UpdateFpNodalPrescribedForces(){
    //Update element force vector
    VectorXd ae(dofMap.size());

    int i = 0;
    for(PhyNode* n : eNodePtrs){
        for(PhyDof& d : n->dofs){
            ae(i++) = d.value;
        }
    }

    fde = ke*ae;
    fee = -fde;

    //Update reaction forces on prescribed nodes
    i = 0;
    for(PhyNode* n : eNodePtrs){
        for(PhyDof& d : n->dofs){
            if(d.is_prescribed)
                d.force -= fee(i);
            i++;
        }
    }
}

void PhyElement::AssembleStiffnessForce(SparseMatrix<double>& globalK, VectorXd& globalF){
    //Assemble global force vector
    int i = 0;
    for(PhyNode* n : eNodePtrs){
        for(PhyDof d : n->dofs){
            if(!d.is_prescribed){
                globalF(d.pos-1) += fee(i);
            }
            i++;
        }
    }

    //Assemble global stiffness matrix
    for(int row_local = 0; row_local < dofMap.size(); row_local++){
        int row_global = dofMap[row_local] - 1;

        //Only dofs with positive ids are free
        if(row_global >= 0){
            //Build the diagonal and don't put zereos in our sparse matrix
            if(ke(row_local,row_local)!=0)
                insertOrIncrementDiagonal(globalK, ke(row_local, row_local), row_global);

            //Build the upper right and use symmetry for the lower left
            for(int col_local = row_local + 1; col_local < dofMap.size(); col_local++){
                int col_global = dofMap[col_local] - 1;
                if(col_global >= 0 && ke(row_local,col_local)!=0)
                    insertOrIncrementSymmetric(globalK, ke(row_local, col_local), row_global, col_global);
            }
        }
    }
}
