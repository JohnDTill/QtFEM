#include "CFEMTypes_Global.h"
#include "FEMSolver.h"

int main(int argc, char **argv)
{
    std::string runName;
    bool verboseIn;

    if(argc==3){
        runName = std::string(argv[1]);

        if(!(std::stringstream(argv[2]) >> verboseIn)) {
            std::cout << "The second command line argument must be a boolean ('0' or '1')\n"
                "CFEM usage: 'CFEM -INPUT_FILENAME- -VERBOSE_MODE_BOOLEAN-" << std::endl;
            return 0;
        }
    }else if(argc==1){
        std::cout << "runName\t";
        std::cin >> runName;

        int tmpi;
        std::cout << "verbose ('0' or '1')\t";
        std::cin >> tmpi;

        verboseIn = static_cast<bool>(tmpi);
    }else{
        std::cout << "CFEM usage: 'CFEM -INPUT_FILENAME- -VERBOSE_MODE_BOOLEAN-" << std::endl;
        return 0;
    }

	FEMSolver femSolver;
    femSolver.FEMSolve(runName, verboseIn);

    if(verboseIn) std::cout << "CFEM completed successfully" << std::endl;

	return 0;
}
