#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <exception>
#include <iostream>
#include <locale>
#include <codecvt>
#include <sstream>
#include <stdexcept>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <iterator>
#include <xercesc/util/Xerces_autoconf_config.hpp>
#include "../header/read.h"

using namespace xercesc;
using namespace std;

/**
 * @brief Exits with message to stderr.
 * 
 * @param message 
 */
void exitWithMessage(const string message) {
    cerr << message << endl;
    exit(1);
}

/**
 * @brief Wrapper around stoi.
 * 
 * @param str 
 * @return const int32_t 
 */
static const int32_t saveStoi(const string str) {
    try {
        return stoi(str);
    }
    catch (...) {
        exitWithMessage("Bad Integer Value");
    }
}

/**
 * @brief Converts an utf16 char to an utf8 string
 * 
 * @param u_attr 
 * @return const string 
 */
static const string str16to8(const char16_t *u_attr) {
    wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> str_utf16;
    return str_utf16.to_bytes(u_attr);
}

/**
 * @brief Checks if s1 is numerically bigger then s2.
 * 
 * @param s1 
 * @param s2 
 * @return true 
 * @return false 
 */
static bool biggerStr(const char* s1, const char* s2) {
    if (!s1 || !s2) return false;
    while (*s1 && *s2) {
        if (*s1 > *s2) {
            return true;
        }
        if (*s1 < *s2) {
            return false;
        }
        s1++;
        s2++;
    }
    return false;
}

/**
 * @brief Recursivly extracts all Intergers from a String, seperated with "'".
 * 
 * @param excStr 
 * @param exc 
 * @param start 
 */
static const void recCommaInts(const string excStr, vector<uint8_t>& exc, int8_t start = 0) {
    int8_t end = excStr.find(",", start);
    if (end == -1) {
        string subStr = excStr.substr(start, excStr.length() - start);
        bool allDigit = true;
        if (allDigit) {
            exc.push_back(saveStoi(subStr));
        }
    }
    else {
        exc.push_back(saveStoi(excStr.substr(start, end - start)));
        recCommaInts(excStr, exc, end + 1);
    }
}

/**
 * @brief Extracts text content from text node.
 * 
 * @param node 
 * @return string 
 */
static string textToString(const DOMNode* node) {
    const string str = str16to8(node->getTextContent());
    if (!str.empty()) {
        return str;
    }
    else {
        exitWithMessage("Empty String Field");
    }
}

/**
 * @brief From an (element) node, get the text node.
 * 
 * @param node 
 */
static void textNode(DOMNode* node) {
    while (true) {
        node = node->getNextSibling();
        if (node == nullptr) {
            exit(1);
        }
        if (node->getNodeType() == DOMNode::TEXT_NODE) {
            break;
        }
    }
}

/**
 * @brief Gets the child Element  of an Element by its tag name.
 * 
 * @param root 
 * @param name 
 * @return DOMElement* 
 */
static DOMElement* childElementByTag(const DOMElement* root, const XMLCh* name) {
    DOMNode* elementNode = (root->getElementsByTagName(name)->item(0));
    if (elementNode != nullptr) {
        return dynamic_cast<DOMElement *> (elementNode);
    }
    else {
        exitWithMessage("Bad XML Structure");
    }
}

/**
 * @brief Gets the text from he text node of an element.
 * 
 * @param root 
 * @param name 
 * @return string 
 */
static string textNoAttr(const DOMElement* root, const XMLCh* name) {
    DOMNode* node = (root->getElementsByTagName(name)->item(0));
    textNode(node);
    return textToString(node);
}

/**
 * @brief textNoAttr for all subelements, which share the subtag name.
 * 
 * @param root
 * @param name 
 * @param subname 
 * @param texts 
 */
static void subTextsNoAttr(const DOMElement* root, const XMLCh* name, const XMLCh* subname, vector<string>* texts) {
    DOMElement* element = childElementByTag(root, name);
    DOMNodeList* nodes = element->getElementsByTagName(subname);
    for (uint16_t i = 0; i < nodes->getLength(); i++) {
        element = dynamic_cast<DOMElement *> (nodes->item(i));
        texts->push_back(textToString(element));
    }
}

/**
 * @brief All the Option parsing.
 * 
 * @param option 
 */
Option::Option(DOMElement* option) {
    // ReqFunc13 todo
    string refStr = str16to8(option->getAttribute(u"Ref"));
    if (!refStr.empty()) {
        Ref = saveStoi(refStr);
        // ReqFunc11, ReqFunc12
        if (Ref > 63) {
            exitWithMessage("Bad Reference Value");
        }
    }
    else {
        Ref = -1;
    }
    string shortOpt = str16to8(option->getAttribute(u"ShortOpt"));
    uint16_t shlen = shortOpt.length();
    if (shlen == 1) {
        ShortOpt = shortOpt[0];
    }
    else {
        ShortOpt = '\0';
    }
    LongOpt = str16to8(option->getAttribute(u"LongOpt"));
    uint16_t llen = LongOpt.length();
    //either long or short opt
    if (llen == 1 || (llen == 0 && shlen == 0)) {
        exitWithMessage("No Option selected (Short, Long))");
    }
    Interface = str16to8(option->getAttribute(u"Interface"));
    // seperated by comma
    string exclusion_str = str16to8(option->getAttribute(u"Exclusion"));
    if (!exclusion_str.empty()) {
        recCommaInts(exclusion_str, Exclusion);
    }
    string convertType = str16to8(option->getAttribute(u"ConvertTo"));
    //ReqFunc26,27,28
    if (convertType.empty()) {
        ConvertTo = "std::string";
    }
    else if (convertType == "Integer") {
        ConvertTo = "int";
    }
    else if (convertType == "Bool") {
        ConvertTo = "bool";
    }
    else {
        exitWithMessage("Bad convert type");
    }
    string argStr = str16to8(option->getAttribute(u"HasArguments"));
    //these are simular to the definitions in getopt_long
    if (argStr == "optional") {
        ArgumentsReq = 2;
    }
    else if (argStr == "required") {
        ArgumentsReq = 1;
    }
    else {
        ArgumentsReq = 0;
    }
    if (ArgumentsReq != 0) {
        DefaultValue = str16to8(option->getAttribute(u"DefaultValue"));
    }
    ConnectToInternalMethod = str16to8(option->getAttribute(u"ConnectToInternalMethod"));
    ConnectToExternalMethod = str16to8(option->getAttribute(u"ConnectToExternalMethod"));
    Description = str16to8(option->getAttribute(u"Description"));
}

/**
 * @brief For sorting only. Compares by ShortOpt first, if both have none, compares by LongOpt. 
 * 
 * @param o1 
 * @param o2 
 * @return true 
 * @return false 
 */
static bool compByShort(const Option& o1, const Option& o2) {
    bool o1ShortZero = o1.ShortOpt == '\0';
    bool o2ShortZero = o2.ShortOpt == '\0';
    if (o1ShortZero && o2ShortZero) {
        return biggerStr(o2.LongOpt.c_str(), o1.LongOpt.c_str());
    }
    else if (o1ShortZero && !o2ShortZero) {
        return false;
    }
    else if (!o1ShortZero && o2ShortZero) {
        return true;
    }
    else {
        return o1.ShortOpt < o2.ShortOpt;
    }
}

Attributes::~Attributes() {
    XMLPlatformUtils::Terminate();
}

/**
 * @brief All the parsing happens here.
 * 
 * @param root 
 */
Attributes::Attributes(const DOMElement* root) {
    string signPerLineStr = str16to8(root->getAttribute(u"SignPerLine"));
    if (signPerLineStr.length() > 25) {
        SignPerLine = saveStoi(signPerLineStr);
    }
    
    HeaderFileName = textNoAttr(root, u"HeaderFileName");
    SourceFileName = textNoAttr(root, u"SourceFileName");
    NameSpace = textNoAttr(root, u"NameSpace");
    ClassName = textNoAttr(root, u"ClassName");

    subTextsNoAttr(root, u"OverAllDescription", u"Block", &OverAllDescription);
    subTextsNoAttr(root, u"SampleUsage", u"Sample", &SampleUsage);

    DOMNodeList* options = childElementByTag(root, u"Options")->getElementsByTagName(u"Option");
    for (uint16_t i = 0; i < options->getLength(); i++) {
        DOMElement* option = dynamic_cast<DOMElement *>(options->item(i));
        const Option new_option(option);
        addOption(new_option);
    }
    sort(sortedOptions.begin(), sortedOptions.end(), compByShort);
}

/**
 * @brief Adds a new option into the vector.
 * 
 * @param new_option 
 */
void Attributes::addOption(const Option new_option) {
    static vector<Option>::iterator position;
    vector<Option>::iterator element;
    for (element = sortedOptions.begin(); element != sortedOptions.end(); element++) {
        bool hasShort = new_option.ShortOpt != '\0';
        if ((new_option.Ref != -1 && new_option.Ref == element->Ref) || ( hasShort && new_option.ShortOpt == element->ShortOpt) || (new_option.LongOpt.length() != 0 && new_option.LongOpt == element->LongOpt)) {
            exitWithMessage("Dupplicate option");
        }
    }
    sortedOptions.push_back(new_option);
}

/**
 * @brief returns pointer to options
 * 
 * @return const vector<Option>* 
 */
const vector<Option>& Attributes::getOptions() const {
    return sortedOptions;
}