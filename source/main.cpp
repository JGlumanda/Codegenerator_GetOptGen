#include "../header/read.h"
#include "../header/write.h"
#include <utility>
#include <cwchar>
#include <locale>
#include <stdint.h>
#include <string>
#include <iostream>
#include <xercesc/util/PlatformUtils.hpp>
#include <utility>
#include <vector>
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
#include <getopt.h>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/Xerces_autoconf_config.hpp>

using namespace xercesc;
using namespace std;

//string
//connectToExternalMethod
//with argumetn
//Autor Name, Mail, Phone
//cmake
//helpText

/**
 * @brief Main Logic
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char** argv) {
	int c;
	int option_index = 0;
	string helpText = "--path <path>\n--help";
	static const char* xml;
	while(1) {
		static struct option long_options[] = {
			{"path", required_argument, 0, 1},
			{"help", no_argument, 0, 'h'},
			{0, 0, 0, 0}
		};
		c = getopt_long(argc, argv, "1:h", long_options, &option_index);
		if (c == -1) {
			break;
		}
		switch(c) {
			case 1:
				xml = optarg;
				break;
			case 'h':
				cout << helpText << endl;
				break;
			case '?':
				cout << "Unknown option, try --help"<< endl;
				break;
			default:
				break;
		}
	}
	if (xml != nullptr) {
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
		XMLPlatformUtils::Terminate();
		HeaderFile header(attributes);
		SourceFile source(attributes, header.methodWithVar);
		header.writeCombined();
		source.writeCombined();
		}
	return 0;
}