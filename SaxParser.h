// SaxParser.h: interface for the SaxParser class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <istream>
#include <string>
#include <vector>


std::string ToXML(std::string str);

class XSPHandler
{
public:
	virtual void OnOpenTag() = 0;
	virtual void OnCloseTag() = 0;
	virtual void OnNotLeadingChar(char c) = 0;
	virtual void OnDocumentBegin() = 0;
	virtual void OnDocumentEnd() = 0;
	virtual void OnElementBegin(const std::string szName) = 0;
	virtual void OnElementEnd(const  std::string szName) = 0;
	virtual void OnCloseSingleElement(const  std::string szName) = 0;
	virtual void OnAttribute(const std::string szName, const std::string szValue) = 0;
	virtual void OnText(const std::string szValue) = 0;
	virtual void OnCDATA(const std::string szValue) = 0;
	virtual void OnComment(const std::string szText) = 0;
	virtual void OnDeclaration(const std::string szVersion,const std::string szEncoding, const std::string szStandAlone) = 0;
	virtual void OnProcessing(const std::string szValue) = 0;
	virtual ~XSPHandler() {}
};

//Encoding types. Important: 16-bit encoding is not supported! (may be in the next version)
enum SPEncoding : int {
	SPENC_UNKNOWN = 0,	/* Unknown encoding (Autodetection) */
	SPENC_LEGACY  = 1,	/* Some kind of 8-bit encoding*/
	SPENC_UTF_8	  = 2	/* UTF-8 */
};

class SaxParserException : public std::exception
{
public:
    const char *what() const noexcept override;

    unsigned int GetCode() const { return m_nCode; }
    unsigned int GetLine() const { return m_nLine; }
    unsigned int GetColumn() const { return m_nColumn; }

private:
    friend class CSaxParser;

public:
    SaxParserException(unsigned int code, unsigned int line, unsigned int column, const std::string str) :
        m_nCode(code), m_nLine(line), m_nColumn(column), m_str(str)
    {

    }

    unsigned int m_nCode;
    unsigned int m_nLine;
    unsigned int m_nColumn;
    std::string m_str;

	enum : int {
		OK                  =  0,
		EMPTY               =  1,
		INVALID_FORMAT      =  2,
		INVALID_INSTANCE    =  3,
		TOO_BIG_VALUE       =  4,
		PROCESSING_NAME     =  5,
		RESERVED_NAME       =  6,
		MISSING_CLOSING     =  7,
		INVALID_DECL        =  8,
		VERSION             =  9,
		ENCODING            = 10,
		ELEMENT_NAME        = 11,
		MATCH               = 12,
		COMMENT             = 13,
		CDATA               = 14,
		EMPTY_REF           = 15,
		REF_SYMBOL          = 16,
		ATTR_NAME           = 17,
		ATTR_DESCR          = 18,
		ROOT_DATA           = 19,
		UNKNOWN_ENTITY      = 20,
		ROOT_CLOSE          = 21,
		DTD_SUPPORT         = 22,
		EOF                 = 23,
		CDATA_CLOSE         = 24,
		MISSING_SEMI        = 25,
		MISSING_QUOTE       = 26,
		TEXT_BEFORE_ROOT    = 27,
		TEXT_AFTER_ROOT     = 28,
		WHITESPASE_OPEN     = 29,
		WHITESPASE_CLOSE    = 30,
		WHITESPACE_PROCESS  = 31,    
		COMMENT_CLOSE       = 32,
		ENCODING32          = 33,
		ENTITY_DOC_OPEN     = 34,
		CDATA_DOC_OPEN      = 35,
		DECLARATION_CLOSE   = 36,
		PROCESSING_CLOSE    = 37,
		DUBLICATE_ATTRIBUTE = 38,
		INPUT_DATA_ERROR    = 39
	};
};

class SaxParser
{
public:
	SaxParser();
	
	void SetLimitValue(unsigned int nLimit); //Set a limit for value length (tag content). 0 means the limit is not set
	unsigned int GetLimitValue() const; //Return a limit for value length 
    void Parse(std::istream* pStream, XSPHandler* pHandler, int encoding = SPENC_UNKNOWN); //Parse a document
	
private:
    unsigned int m_nLine;
    unsigned int m_nColumn;
    unsigned int m_nLimit;
    std::vector<std::string> m_StackItems;
    int m_nEncoding;
    bool m_bReadHeader;
    bool m_bEmpty;

    XSPHandler* m_pHandler;
    std::istream* m_pStream;
    
    char GetChar();
    char ReadChar();
	void ReadSignature();
	void SkipWhiteSpace();
	bool IsEOF();
	void IncLine();
	void IncColumn();
    unsigned int GetLine() const;
    unsigned int GetColumn() const;
    void ThrowException(unsigned int code);
    void ThrowException(unsigned int code, std::string str);
    void RethrowException(SaxParserException& e, unsigned int nCheckCode, unsigned int nSubstituteCode);

	void PutBack(unsigned char c);
		
	void EnterProcessing();
	void EnterDeclaration();
	void EnterOpenElement(char c);
	void EnterClosingElement();
	void EnterComment();
	void EnterCDATA();
	void EnterDTD(char c);
	void EnterEntity(void* pData);
	void EnterAttribute(void* pAttr, char c);

    class SPBuffer
    {
    public:
        SPBuffer();
        void Put(char c);
        char Extract();
        char Get(unsigned int pos) const;
        void PutBack(char c);
        unsigned char Current() const;
        bool IsEmpty() const;
        char operator[](unsigned int i) const;
        const std::string GetData() const;
        void Clear();
    private:
        unsigned int m_nCount;
        unsigned int m_nPos;
        char m_Data[4];
    };

    SPBuffer m_buffer;

};