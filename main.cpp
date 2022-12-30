#include <locale>
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
#include <uchar.h>
#include <codecvt>

XERCES_CPP_NAMESPACE_USE
using namespace std;
static const string getAttrStr(const char16_t *u_attr) {
    wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> str_utf16;
    return str_utf16.to_bytes(u_attr);
}


int main(int argc, char* argv[]) {
	const char* fileName = argv[1];
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
		parser->parse(fileName);
	}
	catch (...) {
		return(1);
	}

	DOMDocument* doc = parser->getDocument();
	DOMElement* e_root = doc->getDocumentElement();

	string s = getAttrStr(e_root->getAttribute(u"SignPerLine"));
	cout << s << endl;
	XMLPlatformUtils::Terminate();
	return 0;
}

