#ifndef FEM_SOLVER__H
#define FEM_SOLVER__H

#include "CFEMTypes_Global.h"
#include "PhyNode.h"
#include <cstring>

class PhyElement;

class FEMSolver
{
public:
	~FEMSolver();
    void FEMSolve(std::string& runName, bool verboseIn);
    void Input(istream& in); //We only partially cover bad input error messages, left for future work

    //For the UI
    std::vector<PhyNode> getNodes();
    std::tuple<vector<PhyNode>, vector<PhyElement*>, map<int, VectorXd> > getData();

private:
    void setDofPositionsAndForce();
    void Calculate_ElementStiffness_Force();
    void Assemble();
    bool Solve_Dofs();
    void Assign_dof();
    void UpdateFpNodalPrescribedForces();

    std::string getCurrentDirectory();
    friend istream& operator>>(istream& input, FEMSolver& dat);
    friend ostream& operator<<(ostream& out, const FEMSolver& dat);
    bool compareCaseInsensitive(std::string a, std::string b);
    void consumeLine(istream& in);
    void parseSectionHeader(istream& in, std::string name, int line_num);
    void parseColumnHeaders(istream& in, std::string name, int line_num);
    int parseNamedInt(istream& in, std::string name, int line_num);
    template<typename T> T parseUnnamed(istream& in);

	vector<PhyNode> nodes;
    vector<PhyElement*>	elements;
    SparseMatrix<double> K; //stiffness matrix: K*a = F
    VectorXd a;
    VectorXd F;
    VectorXd Fp;
    map<int, VectorXd> mats;
};

#endif
