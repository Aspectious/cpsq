#include "library.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace cpd {
    std::vector<std::string> splitString(const std::string &s, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(s); // Create an input string stream from the string
        std::string token;

        while (std::getline(iss, token, delimiter)) {
            // Read tokens until the delimiter
            tokens.push_back(token);
        }
        return tokens;
    }

    std::vector<unsigned char> readBytesFromTargetFile(std::string filename) {
        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return {};
        }

        // Get file size
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // Allocate buffer and read
        std::vector<unsigned char> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            std::cerr << "Failed to read file: " << filename << std::endl;
            return {};
        }

        return buffer;

    }

    void replaceAll(std::string &str, const std::string &from, const std::string &to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Move past the replaced text
        }
    }

    std::vector<unsigned char> intToBytes(int value) {
        std::vector<unsigned char> bytes;
        // Iterate through the bytes of the integer
        // Assuming a 32-bit int, this will extract 4 bytes.
        // Adjust loop for different int sizes or desired byte count.
        for (int i = 0; i < sizeof(int); ++i) {
            // Extract each byte using bitwise AND and right shift
            bytes.push_back(static_cast<unsigned char>((value >> (i * 8)) & 0xFF));
        }
        // Reverse the order of bytes if you want big-endian representation
        // (most significant byte first). By default, this creates little-endian.
        std::reverse(bytes.begin(), bytes.end());
        return bytes;
    }
    class uDFLib {
    private:
        std::map<std::string, unsigned char[]> entries;

    public:
        uDFLib() = default;

        void addElement(std::string internalLoc, unsigned char *data, int data_len) {
            unsigned char *buffer = new unsigned char[data_len];
            for (int i = 0; i < data_len; i++) {
                buffer[i] = data[i];
            }


            //entries.insert(internalLoc, *buffer);
        }

        void printElements() {
        }
    };

    class cDFLib {
    public:
        std::byte *data;
        int dataLength;
    };

    int dfCompile(std::string mapfile, std::string outfile) {
        // Files
        std::ifstream f_map;
        std::ofstream o_df;

        std::unordered_map<std::string, std::vector<unsigned char> > DFfileContents;
        std::vector<std::pair<std::string, std::string>> mapcontents;

        // Read Map file and get which files to be compiled
        // Open and iterate through the map file
        f_map.open(mapfile);
        if (f_map.is_open()) {
            std::cout << "Reading Map File... ";
            std::string line;
            while (getline(f_map, line)) {
                std::vector<std::string> vec = splitString(line, ',');
                if (vec.size() != 2) {
                    std::cout << "Error Processing Map File!" << std::endl;
                } else {
                    replaceAll(vec[0], "\"", "");
                    replaceAll(vec[1], "\"", "");
                    std::string externalLoc = vec[0];
                    std::string internalLoc = vec[1];
                    mapcontents.push_back({externalLoc, internalLoc});
                }
            }
            f_map.close();
            std::cout << "Done." << std::endl;
        } else {
            std::cerr << "Error opening map file for reading!" << std::endl;
        }

        if (mapcontents.size() > 65535) {
            std::cout << "Number of Files too Large! Consider breaking assets into sepeterate DVs." << std::endl;
            return -16;
        }

        // Open Output file for writing
        o_df.open(outfile, std::ios::binary);

        if (o_df.is_open()) {
            std::cout << "Preparing new cpdf file at [" + outfile + "]... ";
            uint8_t bytes[] = {0x05, 0x63, 0x70, 0x64, 0x66, 0x00, 0x00, 0x00};
            // This header at the top of the file is the signature of a .cpdf file. It lets the reading side
            // Instantly know if it's opening the right file.
            o_df.write(reinterpret_cast<const char*>(bytes), sizeof(bytes));


            std::vector<uint8_t> buffer;

            // Header Information
            int filecount = mapcontents.size();

            buffer.push_back(0x01);                      // Header
            buffer.push_back((filecount >> 8) & 0xFF);      // High byte
            buffer.push_back(filecount & 0xFF);             // Low byte

            for (int i = 0; i < 5; ++i) {
                buffer.push_back(0x00);                  // Padding
            }
            uint8_t* bytebuffer = new uint8_t[buffer.size()];
            std::copy(buffer.begin(), buffer.end(), bytebuffer);
            o_df.write(reinterpret_cast<const char*>(bytebuffer), buffer.size());
            std::cout << "Done." << std::endl;
            delete[] bytebuffer;

            std::cout << "Writing Files..." << std::endl;
            // Now from the list of files provided from the map file,
            // We read each subsequent file and store it's bytes in a large vector.
            // This large vector handles all the data and gets transformed into the final file.
            for (int i=0; i < filecount; ++i) {
                try {
                    std::string externalpath = mapcontents[i].first;
                    std::string internalpath = mapcontents[i].second;

                    std::cout << "[" << outfile << "] <-- [" + externalpath + "]... ";
                    std::vector<uint8_t> bytebuffer;
                    bytebuffer.push_back(0x11);
                    bytebuffer.push_back(0x12);
                    for (char c : internalpath) {
                        bytebuffer.push_back(static_cast<uint8_t>(c));
                    }
                    bytebuffer.push_back(0x12);
                    bytebuffer.push_back(0x14);
                    std::vector<unsigned char> filebytes = readBytesFromTargetFile(externalpath);

                    std::vector<unsigned char> lengthbytes = intToBytes(filebytes.size());
                    for (unsigned char byte : lengthbytes) {
                        bytebuffer.push_back(byte);
                    }
                    lengthbytes.clear();
                    lengthbytes.shrink_to_fit();

                    bytebuffer.push_back(0x14);
                    for (uint8_t val : filebytes) {
                        bytebuffer.push_back(static_cast<uint8_t>(val));
                    }
                    bytebuffer.push_back(0x10);

                    uint8_t* ptr_bytebuffer = new uint8_t[bytebuffer.size()];
                    std::copy(bytebuffer.begin(), bytebuffer.end(), ptr_bytebuffer);
                    o_df.write(reinterpret_cast<const char*>(ptr_bytebuffer), bytebuffer.size());
                    delete[] ptr_bytebuffer;
                    bytebuffer.clear();
                    bytebuffer.shrink_to_fit();
                    std::cout << "Done!" << std::endl;
                } catch (std::exception &e) {
                    std::cerr << "Error Processing Map File!" << std::endl;
                }
            }


            o_df.close();
        }






            return 0;
        }
    }
