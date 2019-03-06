#ifndef PHY_ELEMENT__H
#define PHY_ELEMENT__H

#include "CFEMTypes_Global.h"

class PhyNode;

class PhyElement{
	friend ostream& operator<<(ostream& out, const PhyElement& dat);
public:
	virtual void setGeometry() = 0;
    virtual void setInternalMaterialProperties(VectorXd& pMat) = 0;
    void setNodeConnectivity_Sizes(vector<PhyNode*>& eNodePtrsIn);
	void print(ostream& out);
    virtual void Calculate_ElementStiffness() = 0;
    void calculateElementForces();
    void AssembleStiffnessForce(SparseMatrix<double>& globalK, VectorXd& globalF);
    virtual void SpecificOutput(ostream&) const {THROW("does not have implementation");}
    void insertOrIncrementDiagonal(SparseMatrix<double>& mat, double val, int row);
    void insertOrIncrementSymmetric(SparseMatrix<double>& mat, double val, int row, int col);
    void UpdateFpNodalPrescribedForces();

	int id;
    vector <PhyNode*> eNodePtrs;
	vector<int> dofMap;
	ElementType eType;
	int matID;
    MatrixXd ke; // element stiffness matrix (dense)
    VectorXd fde;	// element essential BC force
    VectorXd fee; // all element forces
};

// Defining types (e.g. here bar, beam, truss, frame elements) and returning the right choice of the parent class by "factory" functions is often used in C++ programming.
// For example consider a function that based on shape name creates the right shape and return them all as shape (pointer) but when compute area is called on these pointers the right area calculation function is called.
// Refer to the implementation of the function for reference.
PhyElement* PhyElementFactory(ElementType eTypeIn, int elem_id, int matID, vector <PhyNode*>& eNodePtrsIn);

#endif
