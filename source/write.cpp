#include "../header/read.h"
#include "../header/write.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <streambuf>
#include <string>
#include <sys/types.h>
#include <tuple>
#include <utility>
#include <vector>

#define NL "\n"
#define LNL "\\n"
#define SC ";"
#define SP " "

using namespace std;

/**
 * @brief Construct a new File:: File object
 * 
 * @param fileName 
 * @param attributes 
 */
File::File(std::string fileName, Attributes& attributes) : attributes(attributes) {
    try {
        fileStream.open(fileName, ios::out);
    }
    catch (...) {
        cerr << "Coudn't open file." << endl;
        exit(1);
    }
    this->attributes = attributes;
}

/**
 * @brief Destroy the File:: File object
 * 
 */
File::~File() {
    fileStream.close();
}

/**
 * @brief Adds something with curlies in startstring and endstring. If declaration is true, also adds a semicolon.
 * 
 * @param scope 
 * @param declaration 
 */
void File::addWithCurly(string scope, bool declaration) {
    startString += scope + "{";
    if (declaration) {
        endString = "};" + endString;
    }
    else {
        endString = "}" + endString;
    }
}

/**
 * @brief Appends a string to a string, and adds a symbol at the end.
 * 
 * @param str 
 * @param toAppend 
 * @param sym 
 */
static void appendWithSymbol(string &str, const string toAppend, const string sym) {
    str.append(toAppend).append(sym);
}

/**
 * @brief Construct a new Header File:: Header File object
 * 
 * @param attributes 
 */
HeaderFile::HeaderFile(Attributes& attributes) : File(attributes.HeaderFileName, attributes) {
    //startString
    vector<Option> options = File::attributes.getOptions();
    endString = "\n#endif";
    startString = "#ifndef GETOPT_CLASS\n#define GETOPT_CLASS\n#include <string>\nusing namespace std;\nnamespace ";
    if (File::attributes.NameSpace.length() > 0) {
        addWithCurly(File::attributes.NameSpace, false);
    }
    if (File::attributes.ClassName.length() > 0) {
        addWithCurly("class " + File::attributes.ClassName, true);
    }

    string help= "\"";
    for (vector<Option>::iterator element = options.begin(); element < options.end(); element++) {
        addMethod(element);
        if (element->ShortOpt != '\0') {
            help += string("-") + element->ShortOpt;
        }
        if (!element->LongOpt.empty()) {
            help += "--" + element->LongOpt;
        }
        help += LNL;
    }
    addHelpText(help);

    //ReqOptFunc2
    for (auto & element:attributes.OverAllDescription) {
        appendWithSymbol(helpText, element, LNL);
    }
    //ReqOptFunc3
    for (auto & element:attributes.SampleUsage) {
        appendWithSymbol(helpText, element, LNL);
    }
    //ReqOptFunc1
    help = attributes.Name + " " + attributes.Mail + " " +attributes.Phone + "\";";
    addHelpText(help);
}

/**
 * @brief Converts all - to _. Useful because - is not a legal character.
 * 
 * @param str 
 */
static void minusToUnderScore(string& str) {
    replace(str.begin(), str.end(), '-', '_');
}

/**
 * @brief Adds a header method declaration and it's private variable to the methodWithVar vector.
 * 
 * @param option 
 */
void HeaderFile::addMethod(vector<Option>::const_iterator option) {
    bool longOptEmpty = option->LongOpt.empty();
    bool hasInterface = !option->Interface.empty();
    bool hasExternal = !option->ConnectToExternalMethod.empty();
    bool hasInternal = !option->ConnectToInternalMethod.empty();
    //ReqFunc11,15
    if ((!hasExternal && !hasInternal && !hasInterface ) || hasInterface) {
        string isSet = "bool isSet";
        string var = "int set";
        if (longOptEmpty) {
            isSet += option->ShortOpt;
            var += option->ShortOpt;
        }
        else {
            isSet += option->LongOpt;
            var += option->LongOpt;
        }
        isSet += "()";
        minusToUnderScore(isSet);
        minusToUnderScore(var);
        methodWithVar.push_back(make_pair(isSet, var));
    }
    //ReqFunc12,13
    if (hasInterface) {
        if (option->ArgumentsReq > 0) {
            string var = option->ConvertTo + " " + option->Interface;
            minusToUnderScore(var);
            string getValueOf = option->ConvertTo + " getValueof";
            if (longOptEmpty) {
                getValueOf += option->ShortOpt;
            }
            else {
                getValueOf += option->LongOpt;
            }
            getValueOf += "()";
            if (option->ArgumentsReq == 2) {
                if (option->ConvertTo.empty()) {
                    var += string("=\"") + option->DefaultValue + string("\"");
                }
                else {
                var += "=" + option->DefaultValue;
                }
            }
            minusToUnderScore(getValueOf);
            methodWithVar.push_back(make_pair(getValueOf, var)); 
        }
    if (hasExternal) {
        methodWithVar.push_back(make_pair(string("virtual void") + option->ConnectToExternalMethod + "()", ""));
    }
    }
}

/**
 * @brief formats for SignPerLine
 * 
 * @param addText 
 */
void HeaderFile::addHelpText(const string addText) {
    //ReqFunc4
    //fix with existing \n
    if(addText.find_first_of("\n") != string::npos) {
        helpText += addText.substr(0,attributes.SignPerLine) + LNL;
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
    string finalString = startString, methods = "protected: virtual void printHelp(); virtual void unknownOption(const std::string& unknown_option); virtual void parseOptions(int argc, char** argv);public:", vars = "private: string helpText=";
    vars += helpText;
    for (auto &element : methodWithVar) {
        appendWithSymbol(methods, element.first, SC);
        appendWithSymbol(vars, element.second, SC);
    }
    finalString += methods + vars + endString;
    fileStream << finalString << endl;
}

SourceFile::SourceFile(Attributes& attributes, vector<pair<string,string>> methodWithVar) : File(attributes.SourceFileName, attributes) {
    this->methodWithVar = methodWithVar;
    startString = "#include \"" + attributes.HeaderFileName + "\"\n#include <iostream>\n#include <string>\n#include <getopt.h>\n#include <vector>\n#include <algorithm>\n";
    if (!attributes.NameSpace.empty()) {
        startString += "using namespace std;\nusing namespace " + attributes.NameSpace + ";\n";
    }
    longOptStruct = "static struct option long_options[]={";
    uint16_t i = 1;
    for (auto &element : attributes.getOptions()) {
        string longOpt = "{";
        if (!element.LongOpt.empty()) {
            longOpt += string("\"") + element.LongOpt + string("\"") + "," + to_string(element.ArgumentsReq);
            longOpt += ","s;
            if (!element.Interface.empty() || (element.Interface.empty() && element.ConnectToExternalMethod.empty() && element.ConnectToInternalMethod.empty())) {
                longOpt += "&set" + element.LongOpt + ",";
                minusToUnderScore(longOpt);
            }
            else {
                longOpt += "0,";
            }
            if (!(element.ShortOpt == '\0')) {
                longOpt += string("'") + element.ShortOpt + string("'");
                shortOptString += element.ShortOpt + string(element.ArgumentsReq, ':');
                addCase(element, element.ShortOpt);
                refToShortOpt.push_back(make_pair(element.Ref, element.ShortOpt));
            }
            else {
                longOpt += string("'") + to_string(i) + string("'");
                i++;
                shortOptString += to_string(i) + string(element.ArgumentsReq, ':');
                addCase(element, i);
                refToShortOpt.push_back(make_pair(element.Ref, i));
            }
            longOptStruct += longOpt + string("},");
        }
        else {
            shortOptString += element.ShortOpt + string(element.ArgumentsReq, ':');
        }
    }
    longOptStruct += "{0,0,0,0}};";
    for (auto &element : methodWithVar) {
        addMethod(element);
    }
    methods += "void " + attributes.ClassName + "::printHelp(){cout << helpText << endl;exit(1);}";
    methods += "void " + attributes.ClassName + "::unknownOption(const string& unknown_option) { cerr << \"Unknown Option:\" << unknown_option << \"\\ntry --help\"<< endl;exit(1);}";
    methods += "void " + attributes.ClassName+ "::parseOptions(int argc,char** argv) {int optind = 0;vector<int> history;while(1) {" + longOptStruct;
    methods += "int c=getopt_long(argc,argv,\"" +  shortOptString + "\",long_options,&optind);history.push_back(c);if (c==-1) {break;}switch(c) {";
    methods += cases + "case'?':unknownOption(string(optarg));}}}";
}

/**
 * @brief Adds Exclusions to generated File.
 * 
 * @param element 
 */
void SourceFile::excFunc(const Option& element) {
    uint16_t c;
    if (!element.Exclusion.empty()) {
        cases += "if (";
        for (auto &refShort : refToShortOpt) {
            if (*find(element.Exclusion.begin(), element.Exclusion.end(), refShort.first) != *element.Exclusion.end()){
                cases += "*find(history.begin(), history.end(), " + to_string(refShort.second) + ") != *history.end()||";
                c = refShort.second;
            }
        }
        cases += "false){cerr<<\"illegal combination with \"<<" + to_string(c) +"<<\"endl\";}";
    }
}

/**
 * @brief Wrapper for case generation.
 * 
 * @param element 
 * @param cases 
 */
static void caseFunc(const Option& element, string& cases) {
    if (!element.Interface.empty() && element.ArgumentsReq) {
        if(element.ConvertTo == "int")
            cases += "try{" + element.Interface + "=stoi(optarg);}catch(...){cerr<<\"Argument not an int\"<<endl;}";
        else {
            cases += element.Interface + "=optarg;";
        }
    }
    else if (!element.ConnectToExternalMethod.empty()) {
        cases += element.ConnectToExternalMethod + "();";
    }
    else if (!element.ConnectToInternalMethod.empty()) {
        cases += element.ConnectToInternalMethod + "();";
    }
    else {
        cases += "break;";
    }
}

/**
 * @brief Adds a case.
 * 
 * @param element 
 * @param shortOpt 
 */
void SourceFile::addCase(const Option& element, int shortOpt) {
    excFunc(element);
    cases += string("case ") + to_string(shortOpt) + ":";
    caseFunc(element, cases);
}

/**
 * @brief Adds a case.
 * 
 * @param element 
 * @param shortOpt 
 */
void SourceFile::addCase(const Option& element, char shortOpt) {
    excFunc(element);
    cases += string("case '") + shortOpt + "':";
    caseFunc(element, cases);
}

/**
 * @brief Writes source file.
 * 
 */
void SourceFile::writeCombined() {
    string combined = startString + methods;
    fileStream << combined << endl;
}

/**
 * @brief Adds a generated method.
 * 
 * @param element 
 */
void SourceFile::addMethod(pair<string, string> element) {
        string type = element.first.substr(0, element.second.find_first_of(" ") + 1);
        string funcName = element.first.substr(element.first.find_first_of(" ") + 1);
        string name = element.second.substr( element.second.find_first_of(" ") + 1);
        methods += type + " " + attributes.ClassName + "::" + funcName + "{";
        methods += "return " + name.substr(0, name.find_first_of("=")) + ";}\n";
}
