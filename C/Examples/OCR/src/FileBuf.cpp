#include "FileBuf.h"
#include <stdio.h>
#include <malloc.h>
#include <memory.h>

CFileBuf::CFileBuf()
{
	m_pBuf = NULL;
	m_nLen = 0;
}

CFileBuf::~CFileBuf()
{
	Free();
}

bool CFileBuf::Load(const char * pszLibName, int nExtraBytes)
{
	FILE * fp = fopen(pszLibName, "rb");
	if (fp == NULL)
		return false;

	fseek(fp,0,SEEK_END);
	m_nLen=ftell(fp);
	fseek(fp,0,SEEK_SET);

	if (m_nLen == 0)
	{
		fclose(fp);
		return false;
	}

	m_pBuf = (unsigned char*)malloc(m_nLen + nExtraBytes);
	if (m_pBuf == NULL)
	{
		fclose(fp);
		return false;
	}

	fread( m_pBuf, 1, m_nLen, fp);
	if (ferror(fp) != 0)
	{
		fclose(fp);
		free(m_pBuf);
		m_pBuf = NULL;
		return false;
	}

	fclose(fp);

	if (nExtraBytes != 0)
	{
		memset(m_pBuf + m_nLen, 0, nExtraBytes);
		m_nLen += nExtraBytes;
	}
	return true;
}

void CFileBuf::Free()
{
	if (m_pBuf != NULL)	
	{					
		free(m_pBuf);
		m_pBuf = NULL;
	}

	m_nLen = 0;
}

