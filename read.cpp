#include <cstdlib>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "read.h"

using namespace xercesc;
using namespace std;

Attributes::~Attributes() {
	XMLPlatformUtils::Terminate(); // this class will call always call the destructor in the scope of main, so place everthing here
}

void Attributes::setSignPerLine(std::string str) {
    try {
        Attributes::SignPerLine = stoi(str);
    }
    catch (...) {
        exit(EXIT_FAILURE);
    }
}

void Attributes::addOption(std::string option) {
    Option option;
    Attributes::Options.push_back();
}