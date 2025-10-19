#include "library.h"
#include "library.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace std {
    class any;
}

namespace cpd {
#pragma region utilitymethods
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
        if (!file.read(reinterpret_cast<char *>(buffer.data()), size)) {
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

    int bytesToInt32B(std::vector<unsigned char> bytes) {
        if (bytes.size() != 4) throw std::exception("sus");
        else {
            int out = 0;
            out |= bytes[3];
            out |= (bytes[2] << 8);
            out |= (bytes[1] << 16);
            out |= (bytes[0] << 24);
            return out;
        }
    }

    std::vector<unsigned char> extractBetweenVals(std::vector<unsigned char> invec, unsigned char delim) {
        std::vector<unsigned char> outbytes;
        bool started = false;
        for (int i = 0; i < invec.size(); i++) {
            unsigned char c = invec[i];
            if (c == delim) {
                if (started) {
                    started = false;
                    break;
                }
            }

            if (started) {
                outbytes.push_back(c);
            }

            if (c == delim) {
                if (not started) {
                    started = true;
                }
            }
        }
        return outbytes;
    }

    std::vector<uDFHeaderEntry> parseHeadersFromVec(std::vector<std::vector<unsigned char> > bytearr2, int size) {
        try {
            std::vector<uDFHeaderEntry> headers;

            for (int i=0; i<bytearr2.size(); i++) {
                std::vector<unsigned char> bytearr = bytearr2[i];

                // Initialize Fields
                int pathendindex, fileSize, fileOffset = 0;
                std::string filePath = "";

                // Get filepath
                for (int j=1; j<bytearr.size(); j++) {
                    unsigned char c = bytearr[j];
                    if (c == 0x12) {
                        pathendindex = j;
                        break;
                    } else {
                        filePath += c;
                    }
                }
                std::vector v_filesize = {bytearr[pathendindex+1],bytearr[pathendindex+2],bytearr[pathendindex+3],bytearr[pathendindex+4]};
                std::vector v_offset = {bytearr[pathendindex+5],bytearr[pathendindex+6],bytearr[pathendindex+7],bytearr[pathendindex+8]};
                fileSize = bytesToInt32B(v_filesize);
                fileOffset = bytesToInt32B(v_offset);



                headers.push_back(uDFHeaderEntry(filePath, fileOffset, fileSize));
            }

            return headers;
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

#pragma endregion utilitymethods

    uDFLib::uDFLib(std::string filepath) {
        // First, check if file exists. If so, check if it is a valid CPDF file.
        // Then, read through the header, and ensure it is OK.

        std::ifstream file;
        file.open(filepath, std::ios::binary);

        if (!file.is_open()) {
            throw *new std::runtime_error("[CPDLib] Failed to open file: " + filepath);
        }

        std::vector<char> sigtest(8);
        std::vector<unsigned char> sigtest_against = {
            0x05, 0x63, 0x70, 0x64, 0x66, 0x00, 0x00, 0x00
        };
        file.read(sigtest.data(), sigtest.size());
        bool isEqual = true;
        for (int i = 0; i < sigtest.size(); i++) {
            if (sigtest[i] != sigtest_against[i]) {
                isEqual = false;
                throw std::runtime_error("[CPDLib](init) File is not valid CPDF File!");
            }
        }
        if (isEqual) {
            // Time to figure out the file header. From there we can determine the data start address,
            // the length of each file, and the location of each file.

            file.seekg(12, std::ios::beg);
            // Read last four bytes of signature to read header length
            std::vector<unsigned char> headsize(4);
            file.read(reinterpret_cast<char *>(headsize.data()), headsize.size());
            // Decode value with OR Logic
            int headerlength = 0;
            headerlength |= headsize[3];
            headerlength |= headsize[2] << 8;
            headerlength |= headsize[1] << 16;
            headerlength |= headsize[0] << 24;


            std::vector<unsigned char> headercontents;
            bool keepreading = true;


            int bytecount = 0;
            while (keepreading) {
                bytecount += 1;
                unsigned char byte;
                file.read(reinterpret_cast<char *>(&byte), 1);

                // There should be no bytes in the header texts except for the EOH that is 0x10.
                if (bytecount == 8 + headerlength) {
                    keepreading = false;
                    break;
                }
                headercontents.push_back(byte);
            }

            // Now, parse header information into different uDFHeaderEntry

            std::vector<std::vector<unsigned char> > headers;
            std::vector<unsigned char> headerentry;

            for (int i = 0; i < headercontents.size(); ++i) {
                unsigned char ch = headercontents[i];
                if (i >= headercontents.size() - 9) {
                    unsigned char val = headercontents[i];
                    break;
                }
                if (ch == 0x12) {
                    headerentry.push_back(ch);
                    std::vector<unsigned char> nexteightbytes = {
                        headercontents[i + 1], headercontents[i + 2], headercontents[i + 3], headercontents[i + 4],
                        headercontents[i + 5], headercontents[i + 6], headercontents[i + 7], headercontents[i + 8]
                    };
                    for (unsigned char byte: nexteightbytes) {
                        headerentry.push_back(byte);
                    }
                    i += 8;
                    headers.push_back(headerentry);
                    headerentry.clear();
                    headerentry.shrink_to_fit();
                } else {
                    headerentry.push_back(ch);
                }
            }

            std::vector<uDFHeaderEntry> parsedheaders = parseHeadersFromVec(headers, headers.size());

            this->dataoffset = headerlength;
            file.close();
            this->header = parsedheaders;
            this->filepath = filepath;
        }
    }

    std::shared_ptr<uDFStream> uDFLib::fetchEntry(std::string internalLoc) {
        // Check if file is in headers
        bool filefound = false;
        for (uDFHeaderEntry head: this->header) {
            if (head.path == internalLoc) {
                uDFHeaderEntry header = head;
                filefound = true;

                // Open file, read data, close file
                std::ifstream pFile;
                std::vector<unsigned char> bytes(header.length);

                pFile.open(this->filepath, std::ios::binary);
                if (!pFile.is_open()) {
                    throw new std::runtime_error("[CPDLib] Failed to open file: " + internalLoc);
                }
                int offset = header.offsetaddress + this->dataoffset;
                pFile.seekg(16 + offset, std::ios::beg);
                pFile.read(reinterpret_cast<char *>(bytes.data()), header.length);
                pFile.close();


                uDFStream stream(bytes);
                return std::make_unique<uDFStream>(bytes);
            }
        }
        throw std::runtime_error("[CPDLib] File not found: " + internalLoc);
    }

    uDFLib dfLoad(std::string filepath) {
        cpd::uDFLib library = cpd::uDFLib(filepath);
        return library;
    }

    int dfCompile(std::string mapfile, std::string outfile) {
        // Files
        std::ifstream f_map;
        std::ofstream o_df;

        std::unordered_map<std::string, std::vector<unsigned char> > DFfileContents;
        std::vector<std::pair<std::string, std::string> > mapcontents;

        // Read Map file and get which files to be compiled
        // Open and iterate through the map file
        f_map.open(mapfile);
        if (f_map.is_open()) {
            std::cout << "Reading Map File... ";
            std::string line;
            while (getline(f_map, line)) {
                std::vector<std::string> vec = cpd::splitString(line, ',');
                if (vec.size() != 2) {
                    throw std::runtime_error("Error Processing Map File!");
                } else {
                    cpd::replaceAll(vec[0], "\"", "");
                    cpd::replaceAll(vec[1], "\"", "");
                    std::string externalLoc = vec[0];
                    std::string internalLoc = vec[1];
                    mapcontents.push_back({externalLoc, internalLoc});
                }
            }
            f_map.close();
            std::cout << "Done." << std::endl;
        } else {
            throw std::runtime_error("Error opening map file for reading!");
        }

        if (mapcontents.size() > 65535) {
            throw std::runtime_error("Number of Files too Large! Consider breaking assets into sepeterate DVs.");
            return -16;
        }

        // Open Output file for writing
        o_df.open(outfile, std::ios::binary);

        if (o_df.is_open()) {
            std::cout << "Preparing new cpdf file at [" + outfile + "]... ";
            uint8_t bytes[] = {0x05, 0x63, 0x70, 0x64, 0x66, 0x00, 0x00, 0x00};
            // This header at the top of the file is the signature of a .cpdf file. It lets the reading side
            // Instantly know if it's opening the right file.
            o_df.write(reinterpret_cast<const char *>(bytes), sizeof(bytes));


            std::vector<uint8_t> buffer;

            // Header Information
            int filecount = mapcontents.size();

            buffer.push_back(0x01); // Header
            buffer.push_back((filecount >> 8) & 0xFF); // High byte
            buffer.push_back(filecount & 0xFF); // Low byte

            for (int i = 0; i < 5; ++i) {
                buffer.push_back(0x00); // Padding
            }
            uint8_t *bytebuffer = new uint8_t[buffer.size()];
            std::copy(buffer.begin(), buffer.end(), bytebuffer);
            o_df.write(reinterpret_cast<const char *>(bytebuffer), buffer.size());
            std::cout << "Done." << std::endl;

            std::cout << "Writing Headers..." << std::endl;
            // Now from the list of files provided from the map file,
            // We read each subsequent file and store it's bytes in a large vector.
            // This large vector handles all the data and gets transformed into the final file

            /* V2
             * Now, We will iterate through each entry twice - once to store critical information about the files
             * and a second time that simply prints the bytes of the file
             * This is done so that the header, or otherwise a simple "map" of the address in the file
             * in which a certain file can be loaded
             * This is done so that a program can keep just the header in memory, and fetch the segment of data
             * that is needed at runtime, rather than keep the entire file in memory
             * These aide in managing either very large CPDF files or resource-intensive actions
             */
            unsigned int headlength = 0;
            unsigned int offset = 0;

            // First Pass
            for (int i = 0; i < filecount; ++i) {
                try {
                    std::string externalpath = mapcontents[i].first;
                    std::string internalpath = mapcontents[i].second;

                    std::cout << "[" << outfile << "] <-- (HEAD)[" + externalpath + "]... ";
                    std::vector<uint8_t> bytebuffer;
                    bytebuffer.push_back(0x11);

                    for (char c: internalpath) {
                        bytebuffer.push_back(static_cast<uint8_t>(c));
                    }
                    bytebuffer.push_back(0x12);
                    std::vector<unsigned char> filebytes = cpd::readBytesFromTargetFile(externalpath);
                    std::vector<unsigned char> lengthbytes = cpd::intToBytes(filebytes.size());
                    for (unsigned char byte: lengthbytes) {
                        bytebuffer.push_back(byte);
                    }
                    lengthbytes.clear();
                    lengthbytes.shrink_to_fit();

                    for (unsigned char byte: cpd::intToBytes(offset)) {
                        bytebuffer.push_back(byte);
                    }


                    offset += filebytes.size();


                    headlength += bytebuffer.size();
                    uint8_t *ptr_bytebuffer = new uint8_t[bytebuffer.size()];
                    std::copy(bytebuffer.begin(), bytebuffer.end(), ptr_bytebuffer);
                    o_df.write(reinterpret_cast<const char *>(ptr_bytebuffer), bytebuffer.size());
                    delete[] ptr_bytebuffer;
                    bytebuffer.clear();
                    bytebuffer.shrink_to_fit();
                    std::cout << "OK." << std::endl;
                } catch (std::exception &e) {
                    throw std::runtime_error("Error Processing Map File!");
                }
            }

            std::vector<unsigned char> dataStartByte = {0x10};

            o_df.write(reinterpret_cast<const char *>(dataStartByte.data()), dataStartByte.size());
            o_df.seekp(12, std::ios::beg);
            std::vector<unsigned char> headlen = intToBytes(headlength);
            o_df.write(reinterpret_cast<const char *>(headlen.data()), headlen.size());
            o_df.seekp(16 + headlength, std::ios::beg);


            std::cout << "Done." << std::endl;
            std::cout << "Writing Data..." << std::endl;
            for (int i = 0; i < filecount; ++i) {
                try {
                    std::string externalpath = mapcontents[i].first;
                    std::string internalpath = mapcontents[i].second;
                    std::cout << "[" << outfile << "] <-- [" + externalpath + "]... ";

                    std::vector<uint8_t> bytebuffer;
                    std::vector<unsigned char> filebytes = cpd::readBytesFromTargetFile(externalpath);

                    for (uint8_t val: filebytes) {
                        bytebuffer.push_back(static_cast<uint8_t>(val));
                    }


                    uint8_t *ptr_bytebuffer = new uint8_t[bytebuffer.size()];
                    std::copy(bytebuffer.begin(), bytebuffer.end(), ptr_bytebuffer);
                    o_df.write(reinterpret_cast<const char *>(ptr_bytebuffer), bytebuffer.size());
                    delete[] ptr_bytebuffer;
                    bytebuffer.clear();
                    bytebuffer.shrink_to_fit();


                    std::cout << "OK." << std::endl;
                } catch (std::exception &e) {
                    throw std::runtime_error("Error Processing Map File!");
                }
            }


            o_df.close();
        }


        return 0;
    }
};
