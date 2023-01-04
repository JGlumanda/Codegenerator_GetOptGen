#ifndef CODEGEN_READ
#define CODEGEN_READ
#include <cstdint>
#include <typeinfo>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <string>
#include <memory>
#include <vector>
#include <xercesc/util/Xerces_autoconf_config.hpp>

class Option {
public:
    Option(xercesc::DOMElement* option);

    char ShortOpt;
    std::string LongOpt, Interface, DefaultValue, ConnectToInternalMethod, ConnectToExternalMethod, Description, ConvertTo;
    std::uint8_t ArgumentsReq;
    std::int16_t Ref;
    std::vector<uint8_t> Exclusion;
};

class Attributes {
public:
    Attributes(const xercesc::DOMElement* root);
    ~Attributes();

    std::string Name, Phone, Mail, HeaderFileName, SourceFileName, NameSpace, ClassName;
    std::vector<std::string> OverAllDescription, SampleUsage;
    int16_t SignPerLine = 79;
    void setSignPerLine(std::string str);
    void addOption(Option option);
    const std::vector<Option>* getOption() const;
private:
    std::vector<Option> Options;
};


#endif
