#ifndef CODEGEN_READ
#define CODEGEN_READ
#include <cstdint>
#include <typeinfo>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <string>
#include <uchar.h>
#include <memory>
#include <vector>
#include <xercesc/util/Xerces_autoconf_config.hpp>

class Option {
public:
    Option(xercesc::DOMElement option);

    char ShortOpt;
    std::string LongOpt, Interface, DefaultValue, ConnectToInternalMethod, ConnectToExternalMethod, Description, ConvertTo;
    std::uint8_t ArgumentsReq;
    std::uint8_t Ref;
    std::vector<uint8_t> Exclusion;
};

class Attributes {
public:
    Attributes();
    ~Attributes();

    std::string Name, Phone, Mail, HeaderFileName, SourceFileName, NameSpace, ClassName, OverAllDescription, SampleUsage;
    int16_t SignPerLine;
    void setSignPerLine(std::string str);
    void addOption(Option option);
private:
    std::vector<Option> Options;
};


#endif
