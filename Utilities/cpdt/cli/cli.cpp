//
// Created by gdsie on 10/13/2025.
//

#include "cli.h"

#include <iostream>
#include <library.h>
#include <fstream>
#include <ostream>
#include <algorithm>
#include <filesystem>

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        // Advance start_pos to avoid infinite loops if 'to' contains 'from'
        // e.g., replacing "x" with "yx" in "xxx"
        start_pos += to.length();
    }
    return str;
}
std::string genFSAddress(std::string rootReference, std::string filepath) {
    std::filesystem::path relative = std::filesystem::relative(filepath,rootReference);
    // example RootReference
    // C:\Users\testlib\
    // example filepath
    // C:\Users\testlib\assets\img201.png
    // Desired  output
    // ./assets/img201.png
    std::string s;
    s.push_back('\\');
    return std::string("/") + replace_all(relative.string(),s,"/");
}
int throwUsageAndQuit() {
    std::string msg_welcome = std::string("Compact Data File Tools (v") + VERSION_CPDT + "/" + VERSION_CPDL + ")";
    std::string usagemsg = "Usage: \ncpdt.exe genmap <folder src> <output map file>\ncpdt.exe compile <map file> <database file>\ncpdt.exe extract <database file> <internal file>";
    std::cerr << msg_welcome << std::endl << usagemsg << std::endl;
    return -1;
}
int main(int argc, char *argv[]) {
    std::string msg_welcome = std::string("Compact Data File Tools (v") + VERSION_CPDT + "/" + VERSION_CPDL + ")";
    std::string usagemsg = "Usage: \ncpdt.exe genmap <folder src> <output map file>\ncpdt.exe compile <map file> <database file>\ncpdt.exe extract <database file> <internal file>";
    if (argc < 3) {
        std::cerr << msg_welcome << std::endl << usagemsg << std::endl;
    } else if (argc == 4) {
        std::string command = argv[1];
        std::string dbfile = argv[2];
        std::string file2 = argv[3];
        std::transform(command.begin(), command.end(), command.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (command == "compile") {
            std::cout << msg_welcome << std::endl;
            try {
                cpd::dfCompile(dbfile, file2);
                std::cout << "All Done." << std::endl;
            } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                return EXIT_FAILURE;
            }
        } else if (command == "genmap") {
            std::vector<std::string> files;
            if (!std::filesystem::exists(dbfile) || !std::filesystem::is_directory(dbfile)) {
                std::cerr << "Error: Invalid directory path." << std::endl;
                return 1;
            }
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dbfile)) {
                if (std::filesystem::is_regular_file(entry.status())) {
                    files.push_back(entry.path().string());
                }
            }

            std::filesystem::path root = argv[2];
            std::filesystem::path rootpath = std::filesystem::canonical(root);
            std::vector<std::string> entries;
            for (const auto& file : files) {
                std::string filentry = "\"" + file + "\",\"" + genFSAddress(rootpath.generic_string(), file) + "\"";
                std::cout << filentry << std::endl;
                entries.push_back(filentry);
            }
            std::ofstream mapfile;
            mapfile.open(file2);
            for (std::string entry : entries) {
                mapfile.write(entry.c_str(), entry.length());
                mapfile.write("\n", 1);
            }
            mapfile.close();
            std::cout << "Completed!";
        } else if (command == "extract") {
            try {
                std::string file = argv[2];
                cpd::uDFLib library = cpd::dfLoad(dbfile);
                std::shared_ptr<cpd::uDFStream> vfile = library.fetchEntry(file2);
                std::string line;
                while (getline(vfile->get(), line)) {
                    std::cout << line << std::endl;
                }
            } catch (std::exception &e) {
                std::cerr << e.what() << std::endl;
                return EXIT_FAILURE;
            }

        } else {
            return throwUsageAndQuit();
        }

    }
    return 0;
}