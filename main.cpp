#include "read.h"
#include "write.h"
#include <cwchar>
#include <locale>
#include <stdint.h>
#include <string>
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <iostream>
#include <memory>
#include <codecvt>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/Xerces_autoconf_config.hpp>

using namespace xercesc;
using namespace std;

int main(int argc, char* argv[]) {
	const char* xml = argv[1];
	const char* headerFile = argv[2];
	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		//
		return 1;
	}
	XercesDOMParser *parser = (new XercesDOMParser);
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	HandlerBase errHandler;
	parser->setErrorHandler(&errHandler);

	try {
		parser->parse(xml);
	}
	catch (...) {
		return(1);
	}

	DOMDocument* doc = parser->getDocument();
	DOMElement* root = doc->getDocumentElement();
	Attributes attributes(root);
	HeaderFile header(attributes);
	return 0;
}