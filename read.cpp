#include <cstdlib>
#include <locale>
#include <codecvt>
#include <sstream>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <uchar.h>
#include <xercesc/util/Xerces_autoconf_config.hpp>
#include "read.h"

using namespace xercesc;
using namespace std;

static const string getAttrStr(const char16_t *u_attr) {
    wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> str_utf16;
    str_utf16.to_bytes(u_attr);
}

static const void recCommaInts(const string excStr, vector<uint8_t> exc, uint8_t start = 0) {
    uint8_t end = excStr.find(",", start);
    if (start == -1) {
        exc.push_back(stoi(excStr.substr(start, excStr.length() - start)));
    }
    else {
        exc.push_back(stoi(excStr.substr(start, end - start)));
        recCommaInts(excStr, exc, end);
    }
}

Option::Option(DOMElement option) {
    // ReqFunc13 todo
    string refStr = getAttrStr(option.getAttribute(u"Ref"));
    if (refStr != "") {
        Ref = stoi(refStr);
    }
    // ReqFunc11, ReqFunc12
    if (Ref > 63) {
        exit(1);
    }
    string shortOpt = getAttrStr(option.getAttribute(u"ShortOpt"));
    uint16_t shlen = shortOpt.length();
    if (shlen == 1) {
        ShortOpt = shortOpt[0];
    }
    else {
        ShortOpt = '\0';
    }
    LongOpt = getAttrStr(option.getAttribute(u"LongOpt"));
    uint16_t llen = LongOpt.length();
    if (llen > 1 || (llen == 0 && shlen == 0)) {
        exit(1);
    }
    Interface = getAttrStr(option.getAttribute(u"Interface"));
    // seperated by comma
    string exclusion_str = getAttrStr(option.getAttribute(u"Exklusion"));
    recCommaInts(exclusion_str, Exclusion);
    ConvertTo = getAttrStr(option.getAttribute(u"ConvertTo"));
    string argStr = getAttrStr(option.getAttribute(u"HasArguments"));
    if (argStr == "optional") {
        ArgumentsReq = 1;
    }
    else if (argStr == "required") {
        ArgumentsReq = 2;
    }
    else {
        ArgumentsReq = 0;
    }
    if (ArgumentsReq != 0) {
        DefaultValue = getAttrStr(option.getAttribute(u"DefaultValue"));
    }
    ConnectToInternalMethod = getAttrStr(option.getAttribute(u"ConnectToInternalMethod"));
    ConnectToExternalMethod = getAttrStr(option.getAttribute(u"ConnectToExternalMethod"));
    Description = getAttrStr(option.getAttribute(u"Description"));
}

Attributes::~Attributes() {
	XMLPlatformUtils::Terminate(); // this class will call always call the destructor in the scope of main, so place everything here
}

void Attributes::addOption(Option new_option) {
    for (auto & element:Options) {
        if (new_option.Ref == element.Ref || new_option.ShortOpt == element.ShortOpt || new_option.LongOpt == element.LongOpt) {
            exit(1);
        }
    }
    Attributes::Options.push_back(new_option);
}