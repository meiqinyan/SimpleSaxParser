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

enum SPECode : int {
	SPE_OK                  =  0,
	SPE_EMPTY               =  1,
	SPE_INVALID_FORMAT      =  2,
	SPE_INVALID_INSTANCE    =  3,
	SPE_TOO_BIG_VALUE       =  4,
	SPE_PROCESSING_NAME     =  5,
	SPE_RESERVED_NAME       =  6,
	SPE_MISSING_CLOSING     =  7,
	SPE_INVALID_DECL        =  8,
	SPE_VERSION             =  9,
	SPE_ENCODING            = 10,
	SPE_ELEMENT_NAME        = 11,
	SPE_MATCH               = 12,
	SPE_COMMENT             = 13,
	SPE_CDATA               = 14,
	SPE_EMPTY_REF           = 15,
	SPE_REF_SYMBOL          = 16,
	SPE_ATTR_NAME           = 17,
	SPE_ATTR_DESCR          = 18,
	SPE_ROOT_DATA           = 19,
	SPE_UNKNOWN_ENTITY      = 20,
	SPE_ROOT_CLOSE          = 21,
	SPE_DTD_SUPPORT         = 22,
	SPE_EOF                 = 23,
	SPE_CDATA_CLOSE         = 24,
	SPE_MISSING_SEMI        = 25,
	SPE_MISSING_QUOTE       = 26,
	SPE_TEXT_BEFORE_ROOT    = 27,
	SPE_TEXT_AFTER_ROOT     = 28,
	SPE_WHITESPASE_OPEN     = 29,
	SPE_WHITESPASE_CLOSE    = 30,
	SPE_WHITESPACE_PROCESS  = 31,    
	SPE_COMMENT_CLOSE       = 32,
	SPE_ENCODING32          = 33,
	SPE_ENTITY_DOC_OPEN     = 34,
	SPE_CDATA_DOC_OPEN      = 35,
	SPE_DECLARATION_CLOSE   = 36,
	SPE_PROCESSING_CLOSE    = 37,
	SPE_DUBLICATE_ATTRIBUTE = 38,
	SPE_INPUT_DATA_ERROR    = 39
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