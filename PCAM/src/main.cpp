
#include "PCAM.h"

using namespace std;

int main(int argc,char ** argv) {

    // Instatiate Image Stream and open camera
    if (argc < 2) {
        cout << "Error, please speficy a config file as the first argument to PCAM" << endl;
        return -1;
    }

    PCAM pcam(argv[1]);

    // Run camera if one if found and config is valid,
    // otherwise abort with error message
    if (pcam.haveValidConfig()) {
        if (pcam.haveValidCamera()) {
			pcam.PrintBuildInfo();
            pcam.RunCameraGui();
        }
        else {
            cout << "No valid camera found, aborting." << endl;
        }

    }
    else {
        cout << "No valid configuration found, aborting." << endl;
    }

    return 0;
}
