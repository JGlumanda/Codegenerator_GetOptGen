#include "read.h"
#include "write.h"
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#define NL "\n"
#define SC ";"
#define SP " "

using namespace std;

File::File(std::string fileName, Attributes& attributes) : attributes(attributes) {
    try {
        fileStream.open(fileName, ios::out);
    }
    catch (...) {
        exit(1);
    }
    this->attributes = attributes;
}

File::~File() {
    fileStream.close();
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

HeaderFile::HeaderFile(Attributes& attributes) : File(attributes.HeaderFileName, attributes) {
    //startString
    vector<Option> options = File::attributes.getOptions();
    endString = "#endif";
    startString = "#ifndef GETOPT_CLASS\n#define GETOPT_CLASS\nnamespace ";
    if (File::attributes.NameSpace.length() > 0) {
        addWithCurly(File::attributes.NameSpace, false);
    }
    if (File::attributes.ClassName.length() > 0) {
        addWithCurly(File::attributes.ClassName, true);
        //only then methods
    }

    for (vector<Option>::iterator element = options.begin(); element > options.end(); element++) {
        addMethod(element);
    }

    //helpText
    //ReqOptFunc2
    for (auto & element:attributes.OverAllDescription) {
        appendWithSymbol(helpText, element, NL);
    }
    //ReqOptFunc3
    for (auto & element:attributes.SampleUsage) {
        appendWithSymbol(helpText, element, NL);
    }
    //ReqOptFunc1
    appendWithSymbol(helpText, attributes.Name, SP);
    appendWithSymbol(helpText, attributes.Mail, SP);
    appendWithSymbol(helpText, attributes.Phone, NL);
}

void HeaderFile::addMethod(vector<Option>::const_iterator option) {
    bool longOptEmpty = option->LongOpt.empty();
    bool hasInterface = !option->Interface.empty();
    bool hasExternal = !option->ConnectToExternalMethod.empty();
    bool hasInternal = !option->ConnectToInternalMethod.empty();
    //ReqFunc11,15
    if ((!hasExternal && !hasInternal && !hasInterface ) || hasInterface) {
        string isSet = "bool isSet";
        if (longOptEmpty) {
            isSet += option->ShortOpt;
        }
        else {
            isSet += option->LongOpt;
        }
        isSet += "()";
        methodWithVar.push_back(make_pair(isSet, "bool"));
    }
    //ReqFunc12,13
    if (hasInterface) {
        string val;
        string getValueOf = option->ConvertTo + " getValueof";
        if (option->ArgumentsReq) {
            if (longOptEmpty) {
                getValueOf += option->ShortOpt;
            }
            else {
                getValueOf += option->LongOpt;
            }
            getValueOf += "()";
            val = option->Interface;
        }
        if (option->ArgumentsReq == 2) {
            val += "=" + option->DefaultValue;
        }
        methodWithVar.push_back(make_pair(getValueOf, val));
    }
}

void HeaderFile::addHelpText(const string addText) {
    //signPerLine here
    helpText += addText;
}

