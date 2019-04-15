#include "XML.h"
#include <cassert>
#define  MY_VEC_FOR_EACH(vec) for(unsigned int i = 0; i < vec.size(); ++i)
#define INVALID_VALUE_CHAR -1

void XmlNode::clearData()
{
	MY_VEC_FOR_EACH(m_vecChild)
	{
		if (m_vecChild[i] != NULL)
		{
			m_vecChild[i]->clearData();
			delete m_vecChild[i];
			m_vecChild[i] = NULL;
		}
	}
	m_mapParams.clear();
	m_vecChild.clear();
}

XmlFile::XmlFile()
{
	m_pXmlNodehead = NULL;
}

void XmlFile::clearData()
{
	m_strContentSrc.clear();
	m_strContentWithoutComment.clear();
	m_strFileUrl.clear();
	
	if (m_pXmlNodehead != NULL)
	{
		m_pXmlNodehead->clearData();
		delete m_pXmlNodehead;
		m_pXmlNodehead = NULL;
	}
}

void XmlFile::readFile(const char * fileUrl)
{
	m_strFileUrl = fileUrl;
	
	ifstream OpenFile(fileUrl);
	ostringstream buf;

	char ch1 = 0xef;
	char ch2 = 0xbb;
	char ch3 = 0xbf;
	//utf-8 with BOM编码
	
	std::string str;
	char ch;
	int index = 0;
	//取前三个字符
	while (OpenFile.get(ch))
	{
		if (index < 3)
		{
			str.push_back(ch);
		}
		else
		{
			buf.put(ch);
		}
		index++;
	}

	assert(str.size() == 3);//文件字符个数>=3

	m_strContentSrc.clear();
	if (str[0] != ch1 || str[1] != ch2 || str[2] != ch3)//utf-8 without BOM编码
	{
		m_strContentSrc.append(str);
	}

	OpenFile.close();
	m_strContentSrc.append(buf.str());
}

void XmlFile::eraseComment()
{
	//<!-- XXX -->之间的都需要删除
	bool bCommentBegin = false;
	bool bHasComment = false;
	unsigned int size = m_strContentSrc.size();
	std::string strTmp(m_strContentSrc);

	unsigned int i = 0;
	//检测注释字符
	while (i < size)
	{
		if (bCommentBegin)
		{
			int ansEnd = eraseComment_checkEnd(i, size);
			if (ansEnd > 0)
			{
				bCommentBegin = false;
				for (int k = 0; k <= ansEnd; ++k)
				{
					strTmp[i + k] = INVALID_VALUE_CHAR;
				}
				i = i + ansEnd;
			}
			else
			{
				strTmp[i] = INVALID_VALUE_CHAR;
			}
		}
		else
		{
			int ansBegin = eraseComment_checkBegin(i, size);
			if (ansBegin > 0)
			{
				bCommentBegin = true;
				bHasComment = true;
				for (int k = 0; k <= ansBegin; ++k)
				{
					strTmp[i + k] = INVALID_VALUE_CHAR;
				}
				
				i = i + ansBegin;
			}
		}

		i++;
	}

	//检测完毕
	i = 0;
	unsigned int j = 0;
	m_strContentWithoutComment = m_strContentSrc;
	
	if (bHasComment)
	{
		for (i = 0; i < size; ++i)
		{
			if (strTmp[i] == INVALID_VALUE_CHAR)
			{
				continue;
			}
			else
			{
				m_strContentWithoutComment[j] = strTmp[i];
				j++;
			}
		}

		m_strContentWithoutComment.erase(j, size - j);
	}
}

//size = m_strContentSrc.size()
//检测是否为 "<!--"和 "<?"
int XmlFile::eraseComment_checkBegin(int index, int size)
{
	if (m_strContentSrc[index] == '<'
		&& index + 1 < size && m_strContentSrc[index + 1] == '!'
		&& index + 2 < size && m_strContentSrc[index + 2] == '-'
		&& index + 3 < size && m_strContentSrc[index + 3] == '-')
	{
		return 3;
	}
	else if (m_strContentSrc[index] == '<'
		&& index + 1 < size && m_strContentSrc[index + 1] == '?')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//检测"-->"和 "?>"
int XmlFile::eraseComment_checkEnd(int index, int size)
{
	if (m_strContentSrc[index] == '-'
		&& index + 1 < size && m_strContentSrc[index + 1] == '-'
		&& index + 2 < size && m_strContentSrc[index + 2] == '>')
	{
		return 2;
	}
	else if (m_strContentSrc[index] == '?'
		&& index + 1 < size && m_strContentSrc[index + 1] == '>')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//使用堆栈
void XmlFile::buildXmlTree()
{
	m_pXmlNodehead = new XmlNode();
	assert(m_pXmlNodehead != NULL);

	unsigned int size = m_strContentWithoutComment.size();
	
	stack<XmlNode *> myStack;
	myStack.push(m_pXmlNodehead);

	int endIndex = -1;
	XmlNode *pNode = NULL;
	XmlNode *pTop = NULL;
	unsigned int i = 0;
	while (i < size)
	{
		if (m_strContentWithoutComment[i] == '<')
		{
			switch (handleNode(i, size, pNode, endIndex))
			{
			case XML_NODE_TYPE_HEAD:
				//头标签
				pNode->m_nHeadRightKuoIndex = endIndex;
				pTop = myStack.top();
				pTop->m_vecChild.push_back(pNode);
				myStack.push(pNode);
				break;
			case XML_NODE_TYPE_TAIL:
				//尾标签,此时堆栈顶部是头标签
				pTop = myStack.top();
				pTop->m_nTailLeftKuoIndex = i;
				if (pTop->m_vecChild.size() == 0)//叶子节点才去取值
				{
					pTop->m_strValue.assign(
						m_strContentWithoutComment,
						pTop->m_nHeadRightKuoIndex + 1,
						pTop->m_nTailLeftKuoIndex - pTop->m_nHeadRightKuoIndex - 1);
				}

				i = endIndex;
				myStack.pop();
				break;
			case XML_NODE_TYPE_SINGLE:
				//单标签 <Node xxx />, 只需给栈顶节点添加子节点即可
				pTop = myStack.top();
				pTop->m_vecChild.push_back(pNode);
				break;
			default:
				break;
			}
		}
		i++;
	}
	m_pXmlNodehead = m_pXmlNodehead;
}

//处理"<ssdsdfef dfsef>",可能是标签开始，也可能是标签结束
XML_NODE_TYPE XmlFile::handleNode(int beginIndex, int size, XmlNode *&pNode, int &endIndex)
{
	endIndex = -1;
	pNode = NULL;

	for (unsigned int i = beginIndex; i < size; ++i)
	{
		if (m_strContentWithoutComment[i] == '>')
		{
			endIndex = i;
			break;
		}
	}
	
	assert(endIndex > 0);
	assert(beginIndex + 1 < size);

	/*两种格式 
	1 <Node>abc</Node>
	2 <Node name="abc" age="18"/>
	*/
	//结束标签
	if (m_strContentWithoutComment[beginIndex + 1] == '/')
	{
		return XML_NODE_TYPE_TAIL;//</Node>
	}
	else if (m_strContentWithoutComment[endIndex - 1] == '/')
	{
		pNode = createNode(beginIndex, endIndex - 1);//<Node filename="abc.bit" md5="bfdsd32f"/>
		return XML_NODE_TYPE_SINGLE;
	}
	else//建立新的XmlNode
	{
		pNode = createNode(beginIndex, endIndex); //<Node filename = "abc.bit" md5 = "bfdsd32f">
		return XML_NODE_TYPE_HEAD;
	}
}

//注意: <PlayerLevel name="test1">180</PlayerLevel>, 其中'='等号两边是没有空格的
XmlNode * XmlFile::createNode(int beginIndex, int endIndex)
{
	/*
	//存在这样的情况 <device api="3" name="HM NOTE 1LTETD"/>, name里面的value有空格
	string str(m_strContentWithoutComment, beginIndex + 1, endIndex - beginIndex - 1);
	string strTemp;

	vector<string> vecStr;
	stringstream strStream;
	
	strStream << str;
	while (strStream >> strTemp)
	{
		vecStr.push_back(strTemp);
	}

	assert(vecStr.size() > 0);
	
	XmlNode *pNode = new XmlNode();
	if (pNode)
	{
		pNode->m_strKey = vecStr[0];
		if (vecStr.size() > 1)//除了标签,还有属性
		{
			for (unsigned int i = 1; i < vecStr.size(); ++i)
			{
				getKeyValue(vecStr[i], pNode);
			}
		}
	}

	return pNode;*/

	//存在这样的情况 <device api="3" name="HM NOTE 1LTETD"/>, name里面的value有空格
	bool bHasSpace = false;//value里面是否含有space
	bool bSpaceBegin = false;
	char ch;
	for (unsigned int i = beginIndex + 1; i < endIndex; ++i)
	{
		ch = m_strContentWithoutComment[i];
		if (ch == '"')
		{
			if (bSpaceBegin == false)
			{
				bSpaceBegin = true;
			}
			else
			{
				bSpaceBegin = false;
			}
		}
		else if (ch == ' ')//空格
		{
			if (bSpaceBegin == true)//处于双引号之间
			{
				m_strContentWithoutComment[i] = INVALID_VALUE_CHAR;
				bHasSpace = true;
			}
		}
	}
	
	string str(m_strContentWithoutComment, beginIndex + 1, endIndex - beginIndex - 1);
	string strTemp;

	vector<string> vecStr;
	stringstream strStream;

	strStream << str;
	while (strStream >> strTemp)
	{
		vecStr.push_back(strTemp);
	}

	assert(vecStr.size() > 0);

	XmlNode *pNode = new XmlNode();
	if (pNode)
	{
		pNode->m_strKey = vecStr[0];
		if (vecStr.size() > 1)//除了标签,还有属性
		{
			for (unsigned int i = 1; i < vecStr.size(); ++i)
			{
				if (bHasSpace)
				{
					replaceChar(vecStr[i], INVALID_VALUE_CHAR, ' ');//将替换的字符恢复成空格
				}
				getKeyValue(vecStr[i], pNode);
			}
		}
	}

	return pNode; 
}

void XmlFile::replaceChar(string &str, char src, char dst)
{
	MY_VEC_FOR_EACH(str)
	{
		if (str[i] == src)
		{
			str[i] = dst;
		}
	}
}

//name="test1"
void XmlFile::getKeyValue(string &str, XmlNode *pNode)
{
	int posEqual = -1;
	MY_VEC_FOR_EACH(str)
	{
		if (str[i] == '=')
		{
			posEqual = i;
			break;
		}
		else if (str[i] == INVALID_VALUE_CHAR)
		{
			str[i] = ' ';//在这里给他恢复一下
		}
	}

	assert(posEqual > 0);
	string key(str, 0, posEqual);
	string value(str, posEqual + 2, str.size() - posEqual - 3);//取引号中间的内容
	pNode->m_mapParams[key] = value;
}

void XmlFile::printTree()
{
	_printTree(m_pXmlNodehead, 0);//递归
}

void XmlFile::_printTree(XmlNode *pNode, int depth)
{
	if (pNode)
	{
		for (int i = 0; i < depth; ++i)
		{
			cout << "  ";
		}
		cout << pNode->m_strKey;
		if (pNode->m_strValue != "")
		{
			cout << " : " << pNode->m_strValue;
		}
		for (map<string, string>::iterator iter= pNode->m_mapParams.begin(); iter != pNode->m_mapParams.end(); ++iter)
		{
			cout << " " << iter->first << "=" << iter->second;
		}
		
		cout << endl;

		MY_VEC_FOR_EACH(pNode->m_vecChild)
		{
			_printTree(pNode->m_vecChild[i], depth + 1);
		}
	}
}
