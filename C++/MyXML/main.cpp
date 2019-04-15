#include "XML.h"

class XmlFile;

int main()
{
	XmlFile xmlFile;
	xmlFile.readFile("test.xml");
	xmlFile.eraseComment();
	xmlFile.buildXmlTree();

	xmlFile.printTree();
	system("pause");
}
