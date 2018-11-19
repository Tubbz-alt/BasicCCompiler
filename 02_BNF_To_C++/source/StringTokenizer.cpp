#include "StringTokenizer.h"

StringTokenizer* StringTokenizer::create(const char* sData, bool bIgnoreBNFNonTerminals)
{
	StringTokenizer* pStringTokenizer = new StringTokenizer(sData, bIgnoreBNFNonTerminals);
	return pStringTokenizer;
}

StringTokenizer::StringTokenizer(const char* sData, bool bIgnoreBNFNonTerminals)
{
	init(sData);
	m_bIgnoreBNFNonTerminals = bIgnoreBNFNonTerminals;
}

void StringTokenizer::init(const char* sData)
{
	m_sData = "";
	if (sData != nullptr)
		m_sData = sData;
	m_iDataLength = m_sData.length();

	m_iCurrPos = m_iSavedPos = 0;
	m_iCurrLine = m_iSavedLine = 0;
	m_iCurrColumn = m_iSavedColumn = 0;

	m_bIgnoreBNFNonTerminals = true;

	m_vTokens.clear();
	m_TokensIterator = m_vTokens.end();
}

void StringTokenizer::setData(const char* sData, bool bIgnoreBNFNonTerminals)
{
	init(sData);
	m_bIgnoreBNFNonTerminals = bIgnoreBNFNonTerminals;
}

bool StringTokenizer::hasMoreTokens()
{
	return (m_TokensIterator != m_vTokens.end());
}

Token StringTokenizer::nextToken()
{
	Token tok = *m_TokensIterator;
	m_TokensIterator++;

	return tok;
}

void StringTokenizer::ignoreBNFNonTerminals(bool bIgnore)
{
	m_bIgnoreBNFNonTerminals = bIgnore;
}

void StringTokenizer::tokenize()
{
	while (true)
	{
		Token tok = getNextToken();

		if( NOT (tok.getType() == TokenType::Type::TK_EOL || tok.getType() == TokenType::Type::TK_WHITESPACE))
			printf("tok = %s ==> %s\n", tok.getText(), TokenType::toString(tok.getType()).c_str());

		m_vTokens.push_back(tok);

		if (tok.getType() == TokenType::Type::TK_EOI)
			break;
	};

	m_TokensIterator = m_vTokens.begin();
}

Token StringTokenizer::getNextToken()
{
	initRead();
	while (true)
	{
		char ch = peek(0);
		return readDefault(ch);
	}
}

void StringTokenizer::initRead()
{
	m_iSavedPos = m_iCurrPos;
	m_iSavedLine = m_iCurrLine;
	m_iSavedColumn = m_iCurrColumn;
}

char StringTokenizer::peek(int iOffset)
{
	int iPeekOffset = m_iCurrPos + iOffset;
	if (iPeekOffset > m_iDataLength)
		return END_OF_INPUT;
	else
		return (char)m_sData[iPeekOffset];
}

void StringTokenizer::consume(int iOffset)
{
	m_iCurrPos += iOffset;
	m_iCurrColumn += iOffset;
}

Token StringTokenizer::createToken(TokenType::Type eTokenType)
{
	std::string sText = m_sData.substr(m_iSavedPos, m_iCurrPos - m_iSavedPos);
	return Token(eTokenType, sText, m_iSavedLine, m_iSavedColumn);
}

Token StringTokenizer::readNumber()
{
	initRead();
	bool bHasDot = false;

	// Read the 1st digit
	consume(1);

	while (true)
	{
		char ch = peek(0);
		if (::isdigit(ch))
			consume(1);
		else
		if (ch == '.' && NOT bHasDot)
		{
			bHasDot = true;
			consume(1);
		}
		else
			break;
	}

	return createToken( bHasDot ? TokenType::Type::TK_FLOAT : TokenType::Type::TK_INTEGER);
}

Token StringTokenizer::readEOL()
{
	char ch = peek(0);
	if (ch == LINE_FEED)
		consume(1);

	m_iCurrLine++;
	m_iCurrColumn = 0;

	return createToken(TokenType::Type::TK_EOL);
}

Token StringTokenizer::readString()
{
	consume(1);
	initRead();

	while (true)
	{
		char ch = peek(0);
		if (ch == '"')
			break;
		if (ch == CARRIAGE_RETURN)
		{
			consume(1);
			if (peek(0) == LINE_FEED)
				consume(1);

			m_iCurrLine++;
			m_iCurrColumn = 1;
		}
		else
		if (ch == LINE_FEED)
		{
			consume(1);
			m_iCurrLine++;
			m_iCurrColumn = 1;
		}
		else
			consume(1);
	}

	Token tok = createToken(TokenType::Type::TK_STRING);
	consume(1);

	return tok;
}

Token StringTokenizer::readCharacter()
{
	consume(1);
	initRead();

	char ch = peek(0);
	consume(1);

	Token tok = createToken(TokenType::Type::TK_CHARACTER);
	consume(1);

	return tok;
}

Token StringTokenizer::readSingleLineComment()
{
	consume(2);
	initRead();

	while (true)
	{
		char ch = peek(0);
		if (ch == CARRIAGE_RETURN || ch == LINE_FEED)
			break;
		else
			consume(1);
	}

	return createToken(TokenType::Type::TK_SINGLELINECOMMENT);
}

Token StringTokenizer::readMultiLineComment()
{
	consume(2);
	initRead();

	while (true)
	{
		char ch = peek(0);
		if (ch == '*' && peek(1) == '/')
		{
			break;
		}
		else
			consume(1);
	}
	
	Token tok = createToken(TokenType::Type::TK_MULTILINECOMMENT);
	consume(2);

	return tok;
}

Token StringTokenizer::readDefault(char ch0)
{
	char ch1 = peek(1);

	if(ch0 == END_OF_INPUT)																		{ return createToken(TokenType::Type::TK_EOI); }
	else if(ch0 == 'w' && ch1 == 'h' && peek(2) == 'i' && peek(3) == 'l' && peek(4) == 'e')		{ initRead(); consume(5); return createToken(TokenType::Type::TK_WHILE); }
	else if(ch0 == 'p' && ch1 == 'r' && peek(2) == 'i' && peek(3) == 'n' && peek(4) == 't')		{ initRead(); consume(5); return createToken(TokenType::Type::TK_PRINT); }
	else if(ch0 == 'f' && ch1 == 'o' && peek(2) == 'r')											{ initRead(); consume(3); return createToken(TokenType::Type::TK_FOR); }
	else if(ch0 == 'i' && ch1 == 'f')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_IF); }
	else if(ch0 == 'e' && ch1 == 'l' && peek(2) == 's' && peek(3) == 'e')						{ initRead(); consume(4); return createToken(TokenType::Type::TK_ELSE); }
	else if(ch0 == WHITESPACE_SPACE || ch0 == WHITESPACE_TAB)									{ initRead(); consume(1); return createToken(TokenType::Type::TK_WHITESPACE); }

	else if(ch0 == CARRIAGE_RETURN && ch1 == LINE_FEED)											{ initRead(); consume(2); return readEOL(); }
	else if(ch0 == LINE_FEED)																	{ return readEOL(); }
	else if(isdigit(ch0) || (ch0 == '-' && isdigit(ch1)))										{ return readNumber(); }
	else if(ch0 == '/' && ch1 == '/')															{ return readSingleLineComment(); }
	else if(ch0 == '/' && ch1 == '*')															{ return readMultiLineComment(); }
	else if(isBNFCode(true) && NOT m_bIgnoreBNFNonTerminals)									{ return readBNFCode(); }
	else if(ch0 == '"')																			{ return readString(); }
	else if(ch0 == '\'' && peek(2) == '\'')														{ return readCharacter(); }

	else if(ch0 == '(')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_LPAREN); }
	else if(ch0 == ')')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_RPAREN); }
	else if(ch0 == '{')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_LBRACE); }
	else if(ch0 == '}')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_RBRACE); }
	else if(ch0 == '[')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_LSQBRACKET); }
	else if(ch0 == ']')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_RSQBRACKET); }

	else if(ch0 == ';')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_SEMICOL);}
	else if(ch0 == ',')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_COMMA); }

	else if(ch0 == '*' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_MULEQ); }
	else if(ch0 == '*')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_MUL); }
	else if(ch0 == '/' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_DIVEQ); }
	else if(ch0 == '/')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_DIV); }
	else if(ch0 == '%')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_MOD); }
	else if(ch0 == '+' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_ADDEQ); }
	else if(ch0 == '+' && ch1 == '+')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_INCR); }
	else if(ch0 == '+')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_ADD); }
	else if(ch0 == '-' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_SUBEQ); }
	else if(ch0 == '-' && ch1 == '-')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_DECR); }
	else if(ch0 == '-')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_SUB); }
	else if(ch0 == '<' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_LTEQ); }

	else if(ch0 == '<' && NOT m_bIgnoreBNFNonTerminals)											{ return readBNFNonTerminal(); }
	
	else if(ch0 == '<')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_LT); }
	else if(ch0 == '>' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_GTEQ); }
	else if(ch0 == '>')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_GT); }
	else if(ch0 == '=' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_EQ); }
	else if(ch0 == '=')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_ASSIGN); }
	else if(ch0 == '!' && ch1 == '=')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_NEQ); }
	else if(ch0 == '!')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_NOT); }
	else if(ch0 == '&' && ch1 == '&')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_LOGICALAND); }
	else if(ch0 == '&')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_BITWISEAND); }
	else if(ch0 == '|' && ch1 == '|')															{ initRead(); consume(2); return createToken(TokenType::Type::TK_LOGICALOR); }
	else if(ch0 == '|')																			{ initRead(); consume(1); return createToken(TokenType::Type::TK_BITWISEOR); }
	
	else if(isalpha(ch0) || ch0 == '_')															{ return readIdentifier(); }
	else if(ch0 == ':' && ch1 == ':' && peek(2) == '=' && NOT m_bIgnoreBNFNonTerminals)			{ initRead(); consume(3); return createToken(TokenType::Type::TK_BNFASSIGNMENT); }
	else																						{ initRead(); consume(1); return createToken(TokenType::Type::TK_UNKNOWN); }
}

Token StringTokenizer::readIdentifier()
{
	initRead();
	consume(1);

	while (true)
	{
		char ch = peek(0);
		if (isalpha(ch) || ch == '_' || isdigit(ch))
			consume(1);
		else
			break;
	}

	return createToken(TokenType::Type::TK_IDENTIFIER);
}

Token StringTokenizer::readBNFNonTerminal()
{
	consume(1);
	initRead();

	while (true)
	{
		char ch = peek(0);
		if (ch != '>')
		{
			consume(1);
		}
		else
			break;
	}

	Token tok = createToken(TokenType::Type::TK_BNFNONTERMINAL);
	consume(1);

	return tok;
}

bool StringTokenizer::isBNFCode(bool bStartTag)
{
	int iPos = 0;
	return (	peek(iPos++) == '"'
				&&
				peek(iPos++) == '<'
				&&
				(NOT bStartTag ? peek(iPos++) == '/' : true)
				&&
				peek(iPos++) == 'c'
				&&
				peek(iPos++) == 'o'
				&&
				peek(iPos++) == 'd'
				&&
				peek(iPos++) == 'e'
				&&
				peek(iPos++) == '>'
				&&
				peek(iPos++) == '"'
			);
}

Token StringTokenizer::readBNFCode()
{
	consume(8);
	initRead();

	while (true)
	{
		char ch = peek(0);
		if (ch == '"' && isBNFCode(false))
		{
			break;
		}
		else
			consume(1);
	}

	Token tok = createToken(TokenType::Type::TK_BNFCODE);
	consume(9);

	return tok;
}

StringTokenizer::~StringTokenizer()
{

}
