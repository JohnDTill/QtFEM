#include "CFEMTypes_Global.h"
#include "FEMSolver.h"
#include "PhyElement.h"
#include "PhyGlobal.h"
#include "CFEMTypes_Global.h"
#include <string>
#include <locale>

bool FEMSolver::compareCaseInsensitive(std::string a, std::string b){
    std::locale loc;
    for(std::string::size_type i = 0; i < a.length(); i++){
        a[i] = std::tolower(a[i], loc);
        if(a[i]=='\t') a[i]=' '; //treat tabs as a space
    }
    for(std::string::size_type i = 0; i < b.length(); i++){
        b[i] = std::tolower(b[i], loc);
        if(b[i]=='\t') b[i]=' ';
    }
    return a == b;
}

void FEMSolver::consumeLine(istream& in){
    std::string line;
    std::getline(in,line);
}

void FEMSolver::parseSectionHeader(istream& in, std::string name, int line_num){
    std::string line;
    std::getline(in,line);
    if(!compareCaseInsensitive(line,name)) THROW(("Input file error: Expected section header \""
                          + name + "\"on line " + std::to_string(line_num)).c_str());
}

void FEMSolver::parseColumnHeaders(istream& in, std::string name, int line_num){
    std::string line;
    std::getline(in,line);
    if(!compareCaseInsensitive(line,name)) std::cout << ("Input file warning: Expected column headers \""
                          + name + "\"on line " + std::to_string(line_num)) << std::endl;
}

int FEMSolver::parseNamedInt(istream& in, std::string name, int line_num){
    std::string line;
    std::getline(in,line);
    if(line.length() < name.length() || !compareCaseInsensitive(name, line.substr(0,name.length())))
        THROW(("Input file error: Expected \"" + name + "INTEGER\" on line "
               + std::to_string(line_num)).c_str());

    try{
        return std::atoi(line.substr(name.length()).c_str());
    }catch(...){
        THROW(("Input file error: Expected \"" + name + "INTEGER\" on line "
               + std::to_string(line_num)).c_str());
    }
}

template<typename T>
T FEMSolver::parseUnnamed(istream& in){
    T tmp;
    in >> tmp;
    return tmp;
}

void FEMSolver::Input(istream& in){
    int line_num = 1;
    int dim = parseNamedInt(in, "dim ", line_num++);
    int num_dof_per_node = parseNamedInt(in, "ndofpn ", line_num++);
    parseSectionHeader(in, "Nodes", line_num++);
    int nNodes = parseNamedInt(in, "nNodes ", line_num++);
    nodes.reserve(nNodes);
    int totaldof = nNodes*num_dof_per_node;
    parseColumnHeaders(in, "id crd", line_num++);

    //Construct nodes with coordinates
    VectorXd coordinate(dim);
    for(int i = 0; i < nNodes; i++){
        int node_id = parseUnnamed<int>(in);
        if(node_id != (i + 1)) THROW(("Input file error: Incorrect node id on line "
                                      + std::to_string(line_num)).c_str());
        for(int j = 0; j < dim; j++) in >> coordinate(j);
        nodes.push_back( PhyNode(node_id, num_dof_per_node, coordinate) );
        line_num++;
    }

    consumeLine(in);
    parseSectionHeader(in, "Elements", line_num++);
    int ne = parseNamedInt(in, "ne ", line_num++);
    elements.reserve(ne);
    parseColumnHeaders(in, "id elementType matID neNodes eNodes", line_num++);

    //Construct elements with references to nodes, int for material id
    for (int i = 0; i < ne; i++){
        int elem_id = parseUnnamed<int>(in);
        if(elem_id != (i + 1)) THROW(("Input file error: Incorrect element id on line "
                                   + std::to_string(line_num)).c_str());

        ElementType eType = static_cast<ElementType>(parseUnnamed<int>(in));
        int matID = parseUnnamed<int>(in);
        int number_of_nodes_in_element = parseUnnamed<int>(in);

        vector<PhyNode*> eNodePtrsTmp(number_of_nodes_in_element);
        for (int j = 0; j < number_of_nodes_in_element; ++j){
            int node_id = parseUnnamed<int>(in);
            if(node_id < 1 || node_id > nodes.size())
                THROW(("Input file error: Invalid node id on line " + std::to_string(line_num)).c_str());

            eNodePtrsTmp[j] = &nodes[node_id-1];
        }

        PhyElement* pe = PhyElementFactory(eType, i+1, matID, eNodePtrsTmp);
        elements.push_back(pe);
        line_num++;
	}

    consumeLine(in);
    parseSectionHeader(in, "PrescribedDOF", line_num++);
    int np = parseNamedInt(in, "np ", line_num++);
    int nf = totaldof - np;
    F.resize(nf);
    Fp.resize(np);
    K.resize(nf,nf);
    parseColumnHeaders(in, "node node_dof_index value", line_num++);

    //Set dofs with essential BCs
    for(int i = 0; i < np; i++){
        int node_id = parseUnnamed<int>(in);
        int node_dof_index = parseUnnamed<int>(in);
        if(node_dof_index < 1 || node_dof_index > num_dof_per_node)
            THROW(("Input file error: Invalid node_dof_index on line " + std::to_string(line_num)).c_str());

        nodes.at(node_id-1).dofs[node_dof_index-1].is_prescribed = true;
        nodes.at(node_id-1).dofs[node_dof_index-1].value = parseUnnamed<double>(in);
        line_num++;
    }

    consumeLine(in);
    parseSectionHeader(in, "FreeDofs", line_num++);
    int non_zero_free = parseNamedInt(in, "nNonZeroForceFDOFs ", line_num++);
    parseColumnHeaders(in, "node node_dof_index value", line_num++);

    //Set dofs with non-zero forces
    for(int i = 0; i < non_zero_free; i++){
        int node_id = parseUnnamed<int>(in);
        int node_dof_index = parseUnnamed<int>(in);
        if(node_dof_index < 1 || node_dof_index > num_dof_per_node)
            THROW(("Input file error: Invalid node_dof_index on line " + std::to_string(line_num)).c_str());

        nodes.at(node_id-1).dofs.at(node_dof_index-1).force = parseUnnamed<double>(in);
        line_num++;
    }

    consumeLine(in);
    parseSectionHeader(in, "Materials", line_num++);
    int nmats = parseNamedInt(in, "nMat ", line_num++);
    parseColumnHeaders(in, "id numPara Paras", line_num++);

    //Build material parameter vectors
    for(int i = 0; i < nmats; i++){
        int mat_id = parseUnnamed<int>(in);
        if(mats.find(mat_id) != mats.end())
            THROW(("Input file has two materials with id " + std::to_string(mat_id)).c_str());

        int numPara = parseUnnamed<int>(in);
        mats[mat_id-1] = VectorXd(numPara);

        for(int j = 0; j < numPara; j++) mats[mat_id-1](j) = parseUnnamed<double>(in);
    }

    //Set the element materials
    for(PhyElement* pe : elements){
        int matID = pe->matID;
        pe->setInternalMaterialProperties(mats[matID-1]);
	}
}

istream& operator>>(istream& input, FEMSolver& dat){
	dat.Input(input);
	return input;
}

ostream& operator<<(ostream& out, const FEMSolver& dat){
    // For the Nodes
	out << "Nodes\n";
    out << "nNodes\t" << dat.nodes.size() << '\n';
	out << "id\tcrd\n";
	out << "values\nforces\n";
	if (verbose == true){
        out << "position (verbose)\n";
        out << "prescribed_boolean (verbose)\n";
	} 
    for(const PhyNode& n : dat.nodes)
        out << n << '\n';

    // For the Elements
    out << "Elements\n";
    out << "ne\t" << dat.elements.size() << "\n";
    out << "id\telementType\n";
    if(verbose == true){
        out << "forces (verbose)\n";
    }
    out << "specific output\n";

    for(const PhyElement* pe : dat.elements)
        out << *pe << '\n';

	return out;
}

FEMSolver::~FEMSolver(){
    for(PhyElement* pe : elements) delete pe;
}

void FEMSolver::FEMSolve(std::string& runName, bool verboseIn){
	verbose = verboseIn;
    std::string inputFileName;
	inputFileName = runName + ".txt";
	fstream in(inputFileName.c_str(), ios::in);
    if(in.is_open() == false){
        cout << "input file name\t" << inputFileName << " does not exist in active directory '"
             << getCurrentDirectory() << "'\n";
        THROW("Input file does not exist\n");
	}
    //Parse the input file
	Input(in);
    in.close();

    setDofPositionsAndForce();

    //Build a map from the local element's dof ids to the global dof ids
    for(PhyElement* e : elements){
        for(PhyNode* n : e->eNodePtrs){
            for(PhyDof d : n->dofs){
                e->dofMap.push_back(d.pos);
            }
        }
    }

    Calculate_ElementStiffness_Force();
    Assemble();
    if(Solve_Dofs() == false) THROW("Matrix solve failed\n");
    Assign_dof();
    UpdateFpNodalPrescribedForces();

	// output
    std::string outputFileName = runName + "Output.txt";
	fstream out(outputFileName.c_str(), ios::out);
    if(!out.is_open()) THROW(("Output error: could not open '" +
                              outputFileName + "' for writing.").c_str());
    out << (*this);
}

std::vector<PhyNode> FEMSolver::getNodes(){
    return nodes;
}

std::tuple<vector<PhyNode>, vector<PhyElement *>, map<int, VectorXd> > FEMSolver::getData(){
    return std::tuple<vector<PhyNode>, vector<PhyElement *>, map<int, VectorXd> >
            (nodes, elements, mats);
}

void FEMSolver::setDofPositionsAndForce(){
    int pos_free = 0;
    int pos_cons = 0;

    for(PhyNode& n : nodes){
        for(PhyDof& d : n.dofs){
            if(d.is_prescribed){
                d.pos = --pos_cons;
            }else{
                F(pos_free) = d.force;
                d.pos = ++pos_free;
            }
        }
    }
}

void FEMSolver::Calculate_ElementStiffness_Force(){
    for(PhyElement* pe : elements){
        pe->Calculate_ElementStiffness();
        pe->calculateElementForces();
    }
}

void FEMSolver::Assemble(){
    for (PhyElement* pe : elements)
        pe->AssembleStiffnessForce(K, F);
}

bool FEMSolver::Solve_Dofs(){
    if(verbose){
        std::cout << "K\n" << K.toDense() << std::endl;
        std::cout << "\nF: " << F.transpose() << std::endl;
    }

    SparseLU<SparseMatrix<double> > solver;
    solver.compute(K);
    if(solver.info()!=Success) return false;
    a = solver.solve(F);

    if(verbose) std::cout << "SOLN: " << a.transpose() << std::endl;

    return true;
}

void FEMSolver::Assign_dof(){
    for(PhyNode& n : nodes){
        for(PhyDof& d : n.dofs){
            if(!d.is_prescribed) d.value = a(d.pos-1);
        }
    }
}

void FEMSolver::UpdateFpNodalPrescribedForces(){ //Let's look at this
    for(PhyElement* e : elements)
        e->UpdateFpNodalPrescribedForces();
}

#include <stdio.h>  // defines FILENAME_MAX
#define WINDOWS
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
std::string FEMSolver::getCurrentDirectory(){
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  std::string current_working_dir(buff);

  return current_working_dir;
}
