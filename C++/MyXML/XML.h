#pragma once
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include<fstream>
#include <iostream>
#include<stack>
#include <sstream>

using namespace std;

enum XML_NODE_TYPE
{
	XML_NODE_TYPE_HEAD = 0,//<Node>
	XML_NODE_TYPE_TAIL = 1,//</Node>
	XML_NODE_TYPE_SINGLE =2,//<Node filename="abc.bit" md5="bfdsd32f"/>
};

class XmlNode {
public:
	void clearData();
public:
	std::string m_strKey;
	std::string m_strValue;//只有叶子节点才会有value
	int m_nHeadRightKuoIndex;//<Node>abc</Node>,需要让这些字符在同一行,不要有换行
	int m_nTailLeftKuoIndex;//用来赋值m_strValue的
	vector<XmlNode *> m_vecChild;//因为子标签可以是一样的名字,所以不用map,用multi也没必要,子标签不是很多，查找次数不会很多
	map<std::string, std::string> m_mapParams;//参数(key, value)
	//暂时不给一个父指针了
};

class XmlFile {
public:
	XmlFile();
	void clearData();

	void readFile(const char * fileUrl);
	void eraseComment();
		int eraseComment_checkBegin(int index, int size);
		int eraseComment_checkEnd(int index, int size);
	
	void buildXmlTree();
	XML_NODE_TYPE handleNode(int beginIndex,int size, XmlNode *&pNode, int &endIndex);
	XmlNode * createNode(int beginIndex, int endIndex);
	void replaceChar(string &str, char src, char dst);
	void getKeyValue(string &str, XmlNode *pNode);
	

	void printTree();
	void _printTree(XmlNode *pNode, int depth);

public://一些中间数据
	std::string m_strContentSrc;//原始数据
	std::string m_strContentWithoutComment;//去除注释

public://
	std::string m_strFileUrl;
	map<std::string, XmlNode *> m_mapNodes;//暂时无用,string为node::m_strKey
	XmlNode *m_pXmlNodehead;//主要是因为xml文件的第一级节点可能有多个
};
