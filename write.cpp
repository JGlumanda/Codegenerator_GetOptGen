#include "read.h"
#include "write.h"
#include <bits/types/FILE.h>
#include <fstream>
#include <streambuf>
#include <string>

#define NL "\n"
#define SC ";"
#define SP " "

using namespace std;

File::File(std::string fileName, Attributes* attributes) {
    fileStream->open(fileName);
    File::attributes = attributes;
}

File::~File() {
    fileStream->close();
}

void File::addWithCurly(string scope, bool declaration) {
    startString += scope + "{";
    if (declaration) {
        endString = "};" + endString;
    }
    else {
        endString = "}" + endString;
    }
}

static void appendWithSymbol(string str, const string append, const string sym) {
    str.append(append).append(sym);
}

HeaderFile::HeaderFile(std::string fileName, Attributes* attributes) : File(fileName, attributes) {
    //startString
    endString = "#endif";
    startString = "#ifndef GETOPT_CLASS\n#define GETOPT_CLASS\nnamespace ";
    if (File::attributes->NameSpace.length() > 0) {
        addWithCurly(File::attributes->NameSpace, false);
    }
    if (File::attributes->ClassName.length() > 0) {
        addWithCurly(File::attributes->ClassName, true);
        //only then methods
    }
}

void const HeaderFile::writeCombined() {
    // only if class exists
    string finalString = startString + "public" + publicVars + privateVars + "private" + privateVars + endString;
    fileStream->write(finalString.c_str(), finalString.length());
} 

void HeaderFile::addMethod() {
    //modify help Text for each method
    File::attributes->getOption();
}

void HeaderFile::addHelpText() {
    const vector<Option>* options = File::attributes->getOption();
    //ReqOptFunc2
    for (auto & element:attributes->OverAllDescription) {
        appendWithSymbol(helpText, element, NL);
    }
    //ReqOptFunc3
    for (auto & element:attributes->OverAllDescription) {
        appendWithSymbol(helpText, element, NL);
    }
    //ReqOptFunc1
    appendWithSymbol(helpText, attributes->Name, SP);
    appendWithSymbol(helpText, attributes->Mail, SP);
    appendWithSymbol(helpText, attributes->Phone, NL);
}

void const SourceFile::writeCombined() {
    // finalString definition
    string finalString;
    fileStream->write(finalString.c_str(), finalString.length());
}