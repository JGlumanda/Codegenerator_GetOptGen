#include <stdint.h>
#include <string>
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <iostream>
#include <memory>

using namespace xercesc;
using namespace std;

int main() {
	const char* fileName;
	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		//
		return 1;
	}
	std::unique_ptr<XercesDOMParser> parser(new XercesDOMParser);
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	HandlerBase errHandler;
	parser->setErrorHandler(&errHandler);

	try {
		parser->parse(fileName);
	}
	catch (...) {
		return(1);
	}

	DOMDocument* doc = parser->getDocument();
	DOMElement* e_root = doc->getDocumentElement();

	XMLPlatformUtils::Terminate();
	return 0;
}

