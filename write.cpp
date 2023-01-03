#include "read.h"
#include "write.h"
#include <bits/types/FILE.h>
#include <fstream>
#include <streambuf>
#include <string>

using namespace std;

File::File(std::string fileName, Attributes* attributes) {
    fileStream->open(fileName);
    File::attributes = attributes;
}

File::~File() {
    writeCombined();
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

static void appendWithSemicolon(string str, string append) {
    str.append(append).append(";");
}

HeaderFile::HeaderFile(std::string fileName, Attributes* attrs) : File(fileName, attrs) {
    //startString
    endString = "#endif";
    startString = "#ifndef GETOPT_CLASS\n#define GETOPT_CLASS\nnamespace ";
    addWithCurly(attributes->NameSpace, false);
    addWithCurly(attributes->ClassName, true);
}

void HeaderFile::writeCombined() {
    string finalString = startString + "public" + publicVars + privateVars + "private" + privateVars + endString;
    fileStream->write(finalString.c_str(), finalString.length());
} 

void SourceFile::writeCombined() {
    // finalString definition
    string finalString;
    fileStream->write(finalString.c_str(), finalString.length());
}