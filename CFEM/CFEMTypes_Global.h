#ifndef CFEM_TYPES__H
#define CFEM_TYPES__H

#include <iostream> /* for input and output using >> and << operator */
#include <fstream>  /* file streams for input and output */
#include <assert.h>     /* assert */
#include <vector>  
#include <map>	 
#include <string>  // for std::string class (words)
#include <cmath>   // for certain math operators (fabs, sqrt, ...)

using namespace std;	

#include "eigen/Eigen/Dense"
#include "eigen/Eigen/SparseCore"
#include "eigen/Eigen/SparseLU"
using namespace Eigen;

#ifndef EXIT
#define EXIT exit(1);getchar();getchar();
#endif

// to exit the code with useful information when something is wrong. It provides file and line number with a message if a check is incorrect.
// example
//					if (denominator == 0) THROW("cannot divide be zero!\n");
#ifndef THROW
#define THROW(msg){{char tmpstr[255];sprintf(tmpstr, "In %s, line %d : %s \n", __FILE__, __LINE__, msg);	cerr<<tmpstr;getchar(); getchar(); throw(tmpstr); }}
#endif

//You can put debug statements in DB for example
// DB( cout << "the value of the matrix is a\n" << a << '\n';)
// if DEBUG_MODE is 1 the value of a is printed through the above statement, otherwise it won't.
// see double& MATRIX::operator()(unsigned int i, unsigned int j  for example
#define DEBUG_MODE	1
#if DEBUG_MODE
#define DB(x) x
#else
#define DB(x) 
#endif

typedef enum {etDefault, etBar, etBeam, etTruss, etFrame} ElementType;
typedef long ID;
typedef enum {mpb_E, mpb_A, mpb_I} mP_barTruss;

#endif
