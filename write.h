#ifndef CODEGEN_WRITE
#define CODEGEN_WRITE

#include <fstream>
#include <string>
#include <vector>
#include "read.h"

class File{
    public:
    File(std::string fileName, Attributes& attributes);
    ~File();
    std::string startString;
    std::string endString;
    void addWithCurly(std::string scope, bool declaration);
    protected:
    Attributes& attributes;
    std::ofstream fileStream;
    std::vector<std::pair<std::string, std::string>> methodWithVar;
};

class HeaderFile : File {
    public:
    HeaderFile(Attributes& attributes);
    //write in Destructor
    void writeCombined() const;
    void addMethod(std::vector<Option>::const_iterator option);
    private:
    void addHelpText(const std::string addText);
    std::string helpText;
};

class SourceFile : File {
    public:
    SourceFile(std::string fileName, Attributes* attributes);
    //write in Descructor
    void writeCombined() const;
    void addMethod(std::vector<Option>& option);
    private:
    void addLongOpt();
    void addShortOpt();
    void addCase();
    void addToHelp();
    std::string longOptStruct;
    std::string shortOptString;
    std::string cases;
    std::string help;
    std::string methods;
};

#endif
