#ifndef CODEGEN_WRITE
#define CODEGEN_WRITE

#include <fstream>
#include <string>
#include "read.h"

class File{
    public:
    File(std::string fileName, Attributes*attributes);
    ~File();
    std::string startString;
    std::string endString;
    static const Attributes *attributes;
    void addWithCurly(std::string scope, bool declaration);
    virtual void writeCombined();
    protected:
    std::ofstream* fileStream;
};

class HeaderFile : File {
    public:
    HeaderFile(std::string fileName, Attributes* attributes);
    ~HeaderFile();
    //write in Destructor
    void writeCombined();
    void addMethod(std::string method);
    void addPrivate(std::string priVar);
    void addPublic(std::string pubVar);
    private:
    std::string publicVars;
    std::string privateVars;
    std::string publicMethods;
    std::string helpText;
};

class SourceFile : File {
    public:
    SourceFile(std::string fileName, Attributes* attributes);
    ~SourceFile();
    //write in Descructor
    void writeCombined();
    void addLongOpt();
    void addShortOpt();
    void addCase();
    void addToHelp();
    private:
    std::string longOptStruct;
    std::string shortOptString;
    std::string cases;
    std::string help;
    std::string methods;
};

#endif
