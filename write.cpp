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

static void appendWithSymbol(string &str, const string toAppend, const string sym) {
    str.append(toAppend).append(sym);
}

HeaderFile::HeaderFile(Attributes& attributes) : File(attributes.HeaderFileName, attributes) {
    //startString
    vector<Option> options = File::attributes.getOptions();
    endString = "\n#endif";
    startString = "#ifndef GETOPT_CLASS\n#define GETOPT_CLASS\nnamespace ";
    if (File::attributes.NameSpace.length() > 0) {
        addWithCurly(File::attributes.NameSpace, false);
    }
    if (File::attributes.ClassName.length() > 0) {
        addWithCurly(File::attributes.ClassName, true);
        //only then methods
    }

    for (vector<Option>::iterator element = options.begin(); element < options.end(); element++) {
        addMethod(element);
        //addMethodtoHelpText
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
    //rework
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
        string var = "bool ";
        if (longOptEmpty) {
            isSet += option->ShortOpt;
            var += option->ShortOpt;
        }
        else {
            isSet += option->LongOpt;
            var += option->LongOpt;
        }
        isSet += "()";
        methodWithVar.push_back(make_pair(isSet, var));
    }
    //ReqFunc12,13
    if (hasInterface) {
        if (option->ArgumentsReq > 0) {
            string var = option->ConvertTo + " " + option->Interface;
            string getValueOf = option->ConvertTo + " getValueof";
            if (longOptEmpty) {
                getValueOf += option->ShortOpt;
            }
            else {
                getValueOf += option->LongOpt;
            }
            getValueOf += "()";
            if (option->ArgumentsReq == 2) {
                var += "=" + option->DefaultValue;
            }
            methodWithVar.push_back(make_pair(getValueOf, var)); 
        }
    }
}

void HeaderFile::addHelpText(const string addText) {
    //ReqFunc4
    if (addText.length() > attributes.SignPerLine) {
        helpText += addText.substr(0,attributes.SignPerLine) + "\n";
        addHelpText(addText.substr(attributes.SignPerLine));
    }
    else {
        helpText += addText;
    }
}

/**
 * @brief Combines the HeaderFileString and writes it to the File.
 * 
 */
void HeaderFile::writeCombined() {
    string finalString = startString;
    string methods = "public:", vars;
    // fix
    for (auto &element : methodWithVar) {
        appendWithSymbol(methods, element.first, SC);
        appendWithSymbol(vars, element.second, SC);
    }
    finalString += methods + "private:" + vars + endString;
    fileStream << finalString << endl;
}

