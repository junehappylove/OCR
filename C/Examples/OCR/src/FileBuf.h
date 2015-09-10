#ifndef __FILE_UTIL_H__
#define __FILE_UTIL_H__

class CFileBuf
{
public:
	unsigned char * m_pBuf;
	int	   m_nLen;

public:
	CFileBuf();
	~CFileBuf();
	virtual bool Load(const char * pszLibName, int nExtraBytes = 0);
	virtual void Free();
};

#endif // __FILE_UTIL_H__
