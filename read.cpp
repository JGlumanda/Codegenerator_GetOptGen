#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <exception>
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
#include <xercesc/util/Xerces_autoconf_config.hpp>
#include "read.h"

using namespace xercesc;
using namespace std;

// need to add:
// whitespace trimming
// debug shortopt
static const int32_t saveStoi(const string str) {
    try {
        return stoi(str);
    }
    catch (...) {
        exit(1);
    }
}

static const string str16to8(const char16_t *u_attr) {
    wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> str_utf16;
    return str_utf16.to_bytes(u_attr);
}

static const void recCommaInts(const string excStr, vector<uint8_t> exc, int8_t start = 0) {
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

static string textToString(const DOMNode* node) {
    const string str = str16to8(node->getTextContent());
    //trim
    if (str.length() != 0) {
        return str;
    }
    else {
        exit(1);
    }
}

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

static DOMElement* childElementByTag(const DOMElement* root, const XMLCh* name) {
    DOMNode* elementNode = (root->getElementsByTagName(name)->item(0));
    if (elementNode != nullptr) {
        return dynamic_cast<DOMElement *> (elementNode);
    }
    else {
        exit(1);
    }
}

static string textNoAttr(const DOMElement* root, const XMLCh* name) {
    DOMNode* node = (root->getElementsByTagName(name)->item(0));
    textNode(node);
    return textToString(node);
}

static void subTextsNoAttr(const DOMElement* root, const XMLCh* name, const XMLCh* subname, vector<string>* texts) {
    DOMElement* element = childElementByTag(root, name);
    DOMNodeList* nodes = element->getElementsByTagName(subname);
    for (uint16_t i = 0; i < nodes->getLength(); i++) {
        element = dynamic_cast<DOMElement *> (nodes->item(i));
        texts->push_back(textToString(element));
    }
}

Option::Option(DOMElement* option) {
    // ReqFunc13 todo
    string refStr = str16to8(option->getAttribute(u"Ref"));
    if (refStr != "") {
        Ref = saveStoi(refStr);
        // ReqFunc11, ReqFunc12
        if (Ref > 63) {
            exit(1);
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
        exit(1);
    }
    Interface = str16to8(option->getAttribute(u"Interface"));
    // seperated by comma
    string exclusion_str = str16to8(option->getAttribute(u"Exclusion"));
    recCommaInts(exclusion_str, Exclusion);
    ConvertTo = str16to8(option->getAttribute(u"ConvertTo"));
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
}

Attributes::~Attributes() {
	XMLPlatformUtils::Terminate(); // this class will call always call the destructor in the scope of main, so place everything here
}

void Attributes::addOption(const Option new_option) {
    for (auto & element:Options) {
        if ((new_option.Ref != -1 && new_option.Ref == element.Ref) || (new_option.ShortOpt != '\0' && new_option.ShortOpt == element.ShortOpt) || (new_option.LongOpt.length() != 0 && new_option.LongOpt == element.LongOpt)) {
            exit(1);
        }
    }
    Options.push_back(new_option);
}

const vector<Option>* Attributes::getOption() const {
    return &Options;
}