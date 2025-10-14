//
// Created by gdsie on 10/13/2025.
//

#include "cli.h"

#include <iostream>
#include <library.h>
#include <ostream>


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: cli.exe <map file> <output file>" << std::endl;
    } else {
        cpd::dfCompile(argv[1], argv[2]);
    }

    return 0;
}
