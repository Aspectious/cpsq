#ifndef CPDT_LIBRARY_H
#define CPDT_LIBRARY_H
#define VERSION_CPDL "2.1"
#define VERSION_CPDF "2"
#include <streambuf>
#include <string>
#include <vector>
#include <iostream>

namespace cpd {
    class uDFHeaderEntry {
    public:
        std::string path;
        int offsetaddress;
        int length;
        uDFHeaderEntry(std::string path, int offsetAddress, int length) {
            this->path = path;
            this->offsetaddress = offsetAddress;
            this->length = length;
        }
    };

    class uDFStreamBuf : public std::streambuf {
        public:
        uDFStreamBuf(unsigned char* data, std::size_t size) {
            char *begin = reinterpret_cast<char* >(data);
            setg(begin, begin, begin + size);
        }
    };

    class uDFStream {
        std::vector<unsigned char> bytes;
        uDFStreamBuf buf;
        std::istream stream;

    public:
        uDFStream(std::vector<unsigned char> data) : bytes(std::move(data)),
              buf(bytes.data(), bytes.size()), stream(&buf) {}

        std::istream& get() { return stream; }

        // Optional: mimic ifstream interface
        std::size_t size() const { return bytes.size(); }
        void seek(std::streampos pos) { stream.seekg(pos); }
        bool eof() const { return stream.eof(); }

    };

    class uDFLib {
    private:
        std::vector<uDFHeaderEntry> header;
        long long dataoffset;
        std::string filepath;
    public:
        uDFLib() = default;
        uDFLib(std::string filepath);
        std::shared_ptr<uDFStream> fetchEntry(std::string internalLoc);
    };

    uDFLib dfLoad(std::string file);
    class uDFFileEntry;
    int dfCompile(std::string mapfile, std::string outfile);
}

#endif // CPDT_LIBRARY_H