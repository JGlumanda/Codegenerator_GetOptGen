#ifndef CODEGEN_WRITE
#define CODEGEN_WRITE

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include "read.h"

/**
 * @brief Describes a code file.
 * 
 */
class File{
    public:
    std::vector<std::pair<std::string, std::string>> methodWithVar;
    File(std::string fileName, Attributes& attributes);
    ~File();
    std::string startString;
    std::string endString;
    void addWithCurly(std::string scope, bool declaration);
    std::ofstream fileStream;
    protected:
    Attributes& attributes;
};

/**
 * @brief Describes a header file.
 * 
 */
class HeaderFile : public File {
    public:
    HeaderFile(Attributes& attributes);
    //write in Destructor
    void writeCombined();
    void addMethod(std::vector<Option>::const_iterator option);
    private:
    void addHelpText(const std::string addText);
    std::string helpText;
};

/**
 * @brief Describes a source file.
 * 
 */
class SourceFile : File {
    public:
    SourceFile(Attributes& attributes, std::vector<std::pair<std::string,std::string>> methodWithVar);
    //write in Descructor
    void writeCombined();
    void addMethod(std::pair<std::string, std::string> element);
    private:
    void addLongOpt();
    void addShortOpt();
    void addCase(const Option& element, char shortOpt) ;
    void addCase(const Option& element, int shortOpt) ;
    void addCase();
    void addToHelp();
    void excFunc(const Option& element);
    std::vector<std::pair<uint8_t, uint16_t>> refToShortOpt;
    std::string longOptStruct;
    std::string shortOptString;
    std::string cases;
    std::string help;
    std::string methods;
};

#endif
