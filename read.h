#ifndef CODEGEN_READ
#define CODEGEN_READ
#include <cstdint>
#include <typeinfo>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <string>
#include <memory>
#include <vector>

class Option {
public:
    char ShortOpt;
    std::string LongOpt, Interface, DefaultValue, ConnectToInternalMethod, ConnectToExternalMethod, HasArguments, Description;

    void setRef(std::string str);
    void setExclusion(std::string str);
    void setConvertTo(std::string str);
    std::int8_t getRef();
    std::shared_ptr<int> getExclusion;
    std::type_info getConvertTo;
private:
    std::int8_t Ref;
    std::shared_ptr<int> Exclusion;
    std::type_info ConvertTo;
};

class Attributes {
public:
    ~Attributes();

    std::string Name, Phone, Mail, HeaderFileName, SourceFileName, NameSpace, ClassName, OverAllDescription, SampleUsage;
    void setSignPerLine(std::string str);
    void addOption(Option option);
private:
    int16_t SignPerLine;
    std::vector<Option> Options;
};


#endif
