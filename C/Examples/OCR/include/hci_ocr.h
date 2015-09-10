/** 
 * @file    hci_ocr.h 
 * @brief   HCI_OCR SDK ͷ�ļ�  
 */  

#ifndef __HCI_OCR_HEADER__ 
#define __HCI_OCR_HEADER__

#include "hci_sys.h"

#ifdef __OBJC__
#include <UIKit/UIImage.h>
#endif

#ifdef __cplusplus
extern "C"		
{
#endif

/** @defgroup HCI_OCR ����OCR����API */
/* @{ */

//////////////////////////////////////////////////////////////////////////
// ��������

#define OCR_CAP_PROP_LANG		"lang"		///< OCR�������������ƣ�����
#define OCR_CAP_PROP_DOMAIN		"domain"	///< OCR�������������ƣ�����


#pragma pack (push, ocr_api)
#pragma pack (8) 

	//////////////////////////////////////////////////////////////////////////
	/// @brief ͼ��ṹ��.
	/// @details ͼ��ṹ�壬ͼ������������һ���������ڴ������ڣ�������
	///		�����ֽڶ��룬����ÿ�к󲹳���1��3���ֽڵ���������. 
	typedef struct OCR_IMAGE_T
	{
		_MUST_ _IN_ int nWidth;		///< ͼ����
		_MUST_ _IN_ int nHeight;		///< ͼ��߶�

		/// ֻ��ȡ����ֵ1, 8, 24���ֱ��ʾͼ��Ϊ��ɫͼ���Ҷ�ͼ����ɫͼ
		_MUST_ _IN_ int nBitsPerPixel;
		/// ��ά����ָ��, #pLines[i]ָ���i��ͼ�����ݵ��׵�ַ��i��ȡֵ��Χ��[0, #nHeight)@n
		/// ��ɫͼ���������ֽڴ��˳��ΪRGB����ͼ����������ΪRGBRGB...@n
		/// ͼ���ϵ�(0, 0)��λ��ͼ������Ͻ�:
		///		+ �Ҷ�ͼʱ(0, 0)�������ǵ�һ���ϵĵ�һ���ֽڣ��� #pLines[0][0]��
		/// 	+ ��ɫͼʱ(0, 0)�������ڵ�һ���ϵĵ�һ���ֽڵ����λ��
		/// 	+ ��ɫͼʱ(0, 0)���RGB���ֽ������� #pLines[0][0], #pLines[0][1], #pLines[0][2]
		_MUST_ _IN_ unsigned char ** pLines;

		_OPT_ _IN_ int xResolution;	///< x �ֱ���(DPI)�����δ֪��0
		_OPT_ _IN_ int yResolution;	///< y �ֱ���(DPI)�����δ֪��0 
	}OCR_IMAGE;

#pragma pack (pop, ocr_api)

/**
 * @brief �ַ��ڵ��еĺ�ѡ�������Ŀ
 */
#define CANDIDATE_NUM		5
	
/**
 * @brief	��������
 */
typedef enum _tag_OCR_RECOG_RGNTYPE
{
	OCR_RECOG_RGNTYPE_HORZTEXT = 0,		///< �����ı�
	OCR_RECOG_RGNTYPE_VERTTEXT = 1,		///< �����ı�
	OCR_RECOG_RGNTYPE_TABLE = 2,		///< �������
	OCR_RECOG_RGNTYPE_GRAPH = 3,		///< ͼƬ���򣬲�����ʶ��
	OCR_RECOG_RGNTYPE_AUTOTEXT = 4,		///< �ı������������Զ��жϺ����Ű棬��ǰ�汾��֧��
}
OCR_RECOG_RGNTYPE;

/**
 * @brief	����ʶ������Կ��ƣ��������ѡ�������������ʱ����Ч
 */
typedef enum _tag_OCR_RECOG_LANG
{
	OCR_RECOG_LANG_DEFAULT = 0,		///< ȱʡ���ԣ���ʶ�����ô�����
	OCR_RECOG_LANG_ZH_CN = 1,		///< ������ǿ�Ʋ��ü�������ʶ��
	OCR_RECOG_LANG_ZH_HK = 2,		///< ������ǿ�Ʋ��÷�������ʶ����OCR_RECOG_LANG_TW Ŀǰ������
	OCR_RECOG_LANG_ZH_TW = 3,		///< ������ǿ�Ʋ��÷�������ʶ����OCR_RECOG_LANG_HK Ŀǰ������
	OCR_RECOG_LANG_EN = 4			///< ������ǿ�Ʋ���Ӣ������ʶ��
}
OCR_RECOG_LANG;

/**
 * @brief �߶�����
 */
typedef enum _tag_OCR_TABLE_LINE_TYPE
{
	LINE_TYPE_UNKNOWN = 0,				///< δ֪
	LINE_TYPE_REAL,						///< ʵ��
	LINE_TYPE_DASHED,					///< ����
	LINE_TYPE_DOTTED,					///< ����
	LINE_TYPE_VIRTUAL,					///< û��ʵ�ʵ���, ���߼����е���
	LINE_TYPE_EMPTY		= 0x100,		///< û���ߣ���Ӧ���������Ԫ��ĺϲ�
}
OCR_TABLE_LINE_TYPE;

/**
 * @brief ʶ�����ڵ�����
 */
typedef enum _tag_OCR_RECOG_NODE_TYPE
{
	OCR_RECOG_NODE_PAGE	= 0,		///< ҳ��ڵ�
	OCR_RECOG_NODE_REGION,			///< ����ڵ�
	OCR_RECOG_NODE_CELL,			///< ���Ԫ�ڵ�
	OCR_RECOG_NODE_LINE,			///< �ı��нڵ�
	OCR_RECOG_NODE_CHAR,			///< �ַ��ڵ�
}
OCR_RECOG_NODE_TYPE;

//////////////////////////////////////////////////////////////////////////
// ���ݽṹ����

/**
 * @brief ��ṹ
 */
typedef struct _tag_OCR_RECOG_POINT
{
	int x;			///< X����
	int y;			///< Y����
}
OCR_RECOG_POINT;

/**
 * @brief �б߽���У��ṹ
 */
typedef struct _tag_OCR_CORNERS_RESULT
{
	OCR_RECOG_POINT leftTop;		///< ����У������
	OCR_RECOG_POINT rightTop;		///< ����У������
	OCR_RECOG_POINT leftBottom;		///< ����У������
	OCR_RECOG_POINT rightBottom;	///< ����У������
	int	angle;						///< ��б�Ƕ�
}
OCR_CORNERS_RESULT;

/**
 * @brief ���νṹ
 */
typedef struct _tag_OCR_RECOG_RECT
{
	int left;		///< ��
	int top;		///< ��
	int right;		///< ��
	int bottom;		///< ��
}
OCR_RECOG_RECT;

//----------------------------------------------------------------------------
// ���������Ϣ����

/**
 * @brief	��������߶εĶ���
 * @details	ÿ������� (OCR_RECOG_FRAME_LINE) ���ɶ����������߶���ɣ���������֧����������
 *			\n\n
 *			��һ����������߶ε���ʼ����(nStartX, nStartY)����ǰһ����������߶εĽ�������(nEndX, nEndY), �����������
 */
typedef struct _tag_OCR_RECOG_LINESEGMENT 
{
	/// ��ǰ��������߶ε���ʼX����
	int nStartX;

	/// ��ǰ��������߶ε���ʼY����
	int nStartY;

	/// ��ǰ��������߶ε���ֹX����
	int nEndX;

	/// ��ǰ��������߶ε���ֹY����
	int nEndY;

	/// ���߶εĿ��
	int nThickness;

	/// ���߶ε����ͣ��μ� OCR_TABLE_LINE_TYPE �Ķ���
	OCR_TABLE_LINE_TYPE nType;
} 
OCR_RECOG_LINESEGMENT;

/**
 * @brief	����ߵĶ���
 * @details	���ڿ��ܴ����������������˱�������ö����������߶�(OCR_RECOG_LINESEGMENT) ����ʾ��
 */
typedef struct _tag_OCR_RECOG_FRAMELINE
{ 
	/// ��ϸ�ı�������߶���Ϣ
	OCR_RECOG_LINESEGMENT * pSegment;    //��Ӧ�ı����С�εĵ�һ�ε�ָ��

	/// ��������߶ε���Ŀ
	int nSegmentCount;
} 
OCR_RECOG_FRAMELINE;

/**
 * @brief	���Ԫ����
 * @details	���ڿ��ܴ�����������������������Ϳ��ֻ�ǽ������꣬����ı�������ö����������߶� 
 *			(OCR_RECOG_LINESEGMENT) ����ʾ��
 */
typedef struct _tag_OCR_RECOG_TABLECELL 
{
	/// �����������Ͻ����꣬���ڴ�����б������Ǹ�Ϊ��ȷ�ı��Ԫλ��
	OCR_RECOG_POINT LT;

	/// �����������½����꣬���ڴ�����б������Ǹ�Ϊ��ȷ�ı��Ԫλ��
	OCR_RECOG_POINT LB;

	/// �����������Ͻ����꣬���ڴ�����б������Ǹ�Ϊ��ȷ�ı��Ԫλ��
	OCR_RECOG_POINT RT;

	/// �����������½����꣬���ڴ�����б������Ǹ�Ϊ��ȷ�ı��Ԫλ��
	OCR_RECOG_POINT RB;

	/// Cell�ı�ţ���0��ʼ��������ͬ��ŵ�SubCell�������У��ϲ���һ�������ı��Ԫ
	int nCellNo;    
} 
OCR_RECOG_TABLECELL;

/**
 * @brief	���Ķ���
 */
typedef struct _tag_OCR_RECOG_TABLE 
{
	/// ���е�ˮƽ������
	OCR_RECOG_FRAMELINE  *HLines;	

	/// ���е�ˮƽֱ�߸���
	int nHLine;

	/// ���еĴ�ֱ������
	OCR_RECOG_FRAMELINE  *VLines;

	/// ���еĴ�ֱֱ�߸���
	int nVLine;

	/// ���еı��Ԫ���飬����Ϊ(nHLine-1)*(nVLine-1)�����ڿ����кϲ���Ԫ�������Ŀ�� nCellNoCount ����һ��
	OCR_RECOG_TABLECELL  *pCell;		

	/// ���е�CellNo����Ŀ�����ڿ����кϲ���Ԫ������Ŀ����һ������(nHLine-1)*(nVLine-1)
	int nCellNoCount;
}
OCR_RECOG_TABLE;

/**
 * @brief	ʶ������Ķ���
 */
typedef struct _tag_OCR_RECOG_REGION
{
	/// �������������ͼ���λ�ã����ص�λ
	OCR_RECOG_RECT	rcBound;       	

	/// ��������� (OCR_RECOG_RGNTYPE_xxx)
	/// OCR_RECOG_RGNTYPE_GRAPH ���͵����򲻻����ʶ�𣬵�������ϸ�ڵ���Ϣ�У�����Ϊһ������ڵ����
	OCR_RECOG_RGNTYPE	wRegionType;

	/// ����ʶ������Կ��� (OCR_RECOG_LANG_xxx)������ѡ����������ʶ��ʱ��Ч�����Կ����Ǽ����Ӣ��,
	/// ��ʱ���Ը���ʶ�����ô���ָ��������
	OCR_RECOG_LANG	wRecogLang;
	
	/// ����Ǳ������������Ϣ (��ָ��������������ά������Ҫ��ͼ�Լ�����)
	OCR_RECOG_TABLE  * pTable;		
}
OCR_RECOG_REGION;

/**
 * @brief	������������ķ��ؽ��
 */
typedef struct _tag_OCR_RECOG_LAYOUT_RESULT
{
	/// �����б�
	OCR_RECOG_REGION  * pRegionList;

	/// �������Ŀ
	int    nRegionCount;
}
OCR_LAYOUT_RESULT;

/**
 * @brief	��бУ�������ķ��ؽ��
 */
typedef struct _tag_OCR_RECOG_DESKEW_RESULT
{
	/// ��⵽����б�Ƕ�, ��λΪ 1/100�ȣ�˳ʱ��Ϊ��
	int nSkewAngle;
}
OCR_DESKEW_RESULT;

//----------------------------------------------------------------------------
// �ĵ��������

/**
 * @brief	ҳ��ڵ㶨��
 */
typedef struct _tag_OCR_RECOG_PAGENODE
{
	/// ҳ����
	int nWidth;

	/// ҳ��߶�
	int nHeight;

	/// ����������Զ���бУ������ԭʼ����б�Ƕ�, ��λΪ 1/100�ȣ�˳ʱ��Ϊ��
	int nSkewAngle;
}
OCR_RECOG_PAGENODE;

/**
 * @brief	����ڵ㶨��
 */
typedef struct _tag_OCR_RECOG_REGIONNODE
{
	/// �������������ͼ���λ�ã����ص�λ
	OCR_RECOG_RECT	rcBound;

	/// ��������� (OCR_RECOG_RGNTYPE_xxx)
	unsigned short	wRegionType;	

	/// ����ѡ�����������ʶ����Ч����ʱ�����������ʵ��ʶ������(OCR_RECOG_LANG_xxx)
	/// �����������д����� OCR_RECOG_LANG_DEAFAULT, Ҳ���Ϊʵ�ʵ� OCR_RECOG_LANG_xxxx
	unsigned short	wRecogLang;		
}
OCR_RECOG_REGIONNODE;

/**
 * @brief	���Ԫ�ڵ㶨��
 */
typedef struct _tag_OCR_RECOG_CELLNODE
{
	/// �˱��Ԫ��λ��
	OCR_RECOG_RECT	rcCellBound;	
}
OCR_RECOG_CELLNODE;

/**
 * @brief	�ı��нڵ㶨��
 */
typedef struct _tag_OCR_RECOG_LINENODE
{
	/// ���еĻ��ߡ����Ű���Ϊ�ϱ��ߣ����Ű���Ϊ�����
	int	nBaseLine;
	
	/// ���еĴ�С�����Ű���Ϊ�иߣ����Ű���Ϊ�п�
	int nLineSize;
	
	/// ���е�ƽ���ַ����
	int xChar;

	/// ���е�ƽ���ַ��߶�
	int yChar;
}
OCR_RECOG_LINENODE;

/**
 * @brief	�ַ��ڵ㶨��
 */
typedef struct _tag_OCR_RECOG_CHARNODE
{   
	/// ʶ�������֣�UTF-8���룬��'\0'����
	char * pszChar;

	/// �����ѡ�ֵ�ʶ������UTF-8���룬��'\0'����
	/// ��һ�����п��ܺ�pszcharһ������Ҳ�п��ܲ�һ��
	char * pszCandidateWords[CANDIDATE_NUM];

	/// ���ַ�ǰ��Ŀո��ַ���
	int	nSpaceBefore;

	/// �Ƿ����
	bool bAccept;

	/// ���ַ���ͼ��������
	OCR_RECOG_RECT	rcBlock;
}
OCR_RECOG_CHARNODE;

/**
 * @brief	OCRʶ��������ϸ�ڵ���Ϣ
 */
typedef struct _tag_OCR_RECOG_NODE
{
	///  �ڵ�����
	OCR_RECOG_NODE_TYPE	wType;				

	/// ����ڵ���Ϣ
	union 
	{
		OCR_RECOG_PAGENODE		nodePage;
		OCR_RECOG_REGIONNODE	nodeRegion;
		OCR_RECOG_CELLNODE		nodeCell;
		OCR_RECOG_LINENODE		nodeLine;
		OCR_RECOG_CHARNODE		nodeChar;
	};
} 
OCR_RECOG_NODE;

/**
 * @brief	OCRʶ��ķ��ؽ��
 */
typedef struct _tag_OCR_RECOG_RESULT
{
	/// ʶ�����ı�, UTF-8����, ��'\0'���������������� detailResult ��Ϊ�Ƿ���Ҫ��ϸ��Ϣ����������
	char *	pTextBuf;			

	/// ʶ��������ϸ�ڵ��б�ֻ���������� detailResult ��Ϊ yes ��ʱ����Ч������ΪNULL
	OCR_RECOG_NODE * pNodes;				

	/// ʶ��������ϸ�ڵ����Ŀ��ֻ���������� detailResult ��Ϊ yes ��ʱ����Ч������Ϊ0
	int		nNodeCount;

	///�����ļ�����
	char *	pExportFileBuf;
	///�����ļ�����
	int 	nExportFileBufLen;	

}
OCR_RECOG_RESULT;

/////////////////////////////////////////////////////////////////////////
// �ӿ�API��������

/**  
 * @brief	����OCR���� ��ʼ��
 * @param	pszConfig		ȫ�����ô���ASCII���룬��'\0'����
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS ��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ALREADY_INIT</td><td>OCR SDK��γ�ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>���ò����������趨ֵ�Ƿ����ʽ�����</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_DATAPATH_MISSING</td><td>����Ĳ���OCR��capkey</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_CAPKEY_NOT_MATCH</td><td>ȱ�ٱ����dataPath������</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>û���ҵ�ָ��������</td></tr>
 *		<tr><td>@ref HCI_ERR_LOCAL_LIB_MISSING</td><td>������������ȱʧ��Ҫ�Ŀ���Դ</td></tr>
 *	</table>
 *
 * @par ���ô����壺
 * ���ô�����"�ֶ�=ֵ"����ʽ������һ���ַ���������ֶ�֮����','�������ֶ������ִ�Сд��
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>�ֶ�</b></td>
 *			<td><b>ȡֵ��ʾ��</b></td>
 *			<td><b>ȱʡֵ</b></td>
 *			<td><b>����</b></td>
 *			<td><b>��ϸ˵��</b></td>
 *		</tr>
 *		<tr>
 *			<td>dataPath</td>
 *			<td>opt/myapp/ocr_data</td>
 *			<td>��</td>
 *			<td>��ѧ�ַ�ʶ�𱾵���Դ����·��</td>
 *			<td>��ʹ�ñ�������ʱ�������</td>
 *		</tr> 
 *		<tr>
 *			<td>initCapKeys</td>
 *			<td>ocr.local</td>
 *			<td>��</td>
 *			<td>��ʼ������ı�������</td>
 *			<td>���������';'�������������������Ϸ������򷵻ش��󡣺��Դ�����ƶ�����key�������ʹ�ñ���ʶ���������������</td>
 *		</tr>
 *		<tr>
 *			<td>fileFlag</td>
 *			<td>none, android_so</td>
 *			<td>none</td>
 *			<td>��ȡ�����ļ�����������</td>
 *			<td>��ʹ�ñ�������ʱ�����������ϸ˵���μ������ע��</td>
 *		</tr>
 *	</table>
 *
 *  <b>Android��������</b>
 *  @n
 *  ��fileFlagΪandroid_soʱ�����ر�����Դ�ļ�(�ֵ�ͱ����)ʱ�Ὣ�����Ŀ��ļ�������Ϊso�ļ������м��ء�
 *  ���磬��ʹ�õĿ�Ϊfile.datʱ����ʵ�ʴ򿪵��ļ���Ϊlibfile.dat.so��������Androidϵͳ�£�
 *  �����߿��԰��մ˹��򽫱�����Դ������, ����libsĿ¼�´����apk���ڰ�װ����Щ��Դ�ļ�
 *  �ͻ������/data/data/����/libĿ¼�¡������ֱ�ӽ�dataPath������ָΪ��Ŀ¼���ɡ�
 *
 * @note
 * �����ƶ�ʶ�𣬴����ô�����ֱ�Ӵ�NULL����""
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_init(
		_MUST_ _IN_ const char * pszConfig
		);

/**  
 * @brief	��ʼ�Ự
 * @param	pszConfig		�Ự���ô���ASCII���룬��'\0'����
 * @param	pnSessionId		�ɹ�ʱ���ػỰID
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_CAPKEY_MISSING</td><td>ȱ�ٱ����capKey������</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>������Ƿ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_CAPKEY_NOT_MATCH</td><td>������key��ƥ��</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>���������keyδ�ҵ�</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>δ�ҵ���Ч���Ʒ����ַ</td></tr>
 *		<tr><td>@ref HCI_ERR_TOO_MANY_SESSION</td><td>������Session������������(256)</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�����ڴ�ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>������������ܲ����б��У����˴���֧��</td></tr>
 *		<tr><td>@ref HCI_ERR_LOAD_FUNCTION_FROM_DLL</td><td>Ҫ�����ģ�鲻���ڣ�������Ҫ�Ĺ����ڸ�ģ�鲻����</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_INIT_FAILED</td><td>����ʶ��ʱ��OCR�����ʼ��ʧ��</td></tr>
 *	</table>
 *
 * @par ���ô����壺
 * ���ô�����"�ֶ�=ֵ"����ʽ������һ���ַ���������ֶ�֮����','�������ֶ������ִ�Сд��
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>�ֶ�</b></td>
 *			<td><b>ȡֵ��ʾ��</b></td>
 *			<td><b>ȱʡֵ</b></td>
 *			<td><b>����</b></td>
 *			<td><b>��ϸ˵��</b></td>
 *		</tr>
 *		<tr>
 *			<td>capKey</td>
 *			<td>ocr.cloud</td>
 *			<td>��</td>
 *			<td>OCRʶ������key</td>
 *			<td>�μ� @ref hci_ocr_page ��ÿ��sessionֻ�ܶ���һ�����������ҹ����в��ܸı䡣</td>
 *		</tr>
 *	</table>
 * @n@n
 * ���⣬���ﻹ���Դ�����бУ�������������ʶ����������ΪĬ��������μ� hci_ocr_deskew(), hci_ocr_layout_analysis(), hci_ocr_recog() ��
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_session_start(
		_MUST_ _IN_ const char * pszConfig,
		_MUST_ _OUT_ int *pnSessionId
		);

/**  
 * @brief	��бУ��
 * @param	nSessionId		�ỰID
 * @param	pszConfig		�������ã�ASCII���룬��'\0'��������ΪNULL
 * @param	psOcrDeskewResult	��бУ�������ָ�룬ʹ����Ϻ���ʹ�� hci_ocr_free_deskew_result() ���������ͷ�
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>���ò����������趨ֵ�Ƿ����ʽ�����</td></tr>
 *		<tr><td>@ref HCI_ERR_DATA_SIZE_TOO_LARGE</td><td>��������ݳ����ɴ��������, �ƶ�ʶ�����ݷ����ƶ�ʱ�ж�[0,5M]</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>����keyδ�ҵ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�����ڴ�ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>δ�ҵ���Ч���Ʒ����ַ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>���ӷ�����ʧ�ܣ�����������Ӧ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>���������ʳ�ʱ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>���������ص����ݸ�ʽ����ȷ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_RESPONSE_FAILED</td><td>������������бУ��ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>������������ܲ����б��У����˴���֧��</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>�������棬��бУ������ʧ�ܣ������ƶ�ʶ��ͼ����ת��ת������ֵ����ѹ��ʧ��</td></tr>
 *	</table>
 *
 * @par ���ô����壺
 * ���ô�����"�ֶ�=ֵ"����ʽ������һ���ַ���������ֶ�֮����','�������ֶ������ִ�Сд��
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>�ֶ�</b></td>
 *			<td><b>ȡֵ��ʾ��</b></td>
 *			<td><b>ȱʡֵ</b></td>
 *			<td><b>����</b></td>
 *			<td><b>��ϸ˵��</b></td>
 *		</tr> 
 *		<tr>
 *			<td>imageType</td>
 *			<td>normal, screen</td>
 *			<td>normal</td>
 *			<td>ͼ������</td>
 *			<td>normal: ��ͨͼ��<br/>
 *              screen: ����ͼ��
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>adjustImage</td>
 *			<td>yes, no</td>
 *			<td>no</td>
 *			<td>�Ƿ�ֱ����У��</td>
 *			<td>yes: У��<br/>
 *              no: ��У��
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>binarize</td>
 *			<td>adaptive, global</td>
 *			<td>adaptive</td>
 *			<td>��ֵ���������ԻҶȻ��߲�ɫͼ����Ч</td>
 *			<td>global: ȫ�ֶ�ֵ���㷨����Ҫ���ͨ��ɨ�衢�����Ȼ�ȡ��ͼ��<br/>
 *              adaptive: �ֲ�����Ӧ��ֵ���㷨����Ҫ���ͨ�����ա�����ͷ�Ȼ�ȡ���ܹ���Ӱ��ϴ�ͼ��
 *			</td>
 *		</tr>
 *		<tr>
 *			<td>localProcess</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>�Ƿ���������Ԥ�����ƶ�����OCR֧�ָ�����@n
 *				������Ҫ��ָ��ֵ��</td>
 *			<td>yes: ����Ԥ����<br/>
 *              no: ������Ԥ����
 *			</td>
 *		</tr> 
 *	</table>
 * @n@n
 * ����û�ж�����������ʹ�� session_start ʱ�Ķ��塣��� session_start ʱҲû�ж��壬��ʹ��ȱʡֵ
 *
 * @note
 * ������ adjustImage ������⵽��б�Ǻ��Ƿ��ͼ�������бУ������adjustImage=no��ֻ������б�ǶȲ��Ķ�ͼ�����ݣ�@n
 * ��adjustImage=yes�������У����������б�Ƕ�ͼ�����ݽ���У����
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_deskew(
		_MUST_ _IN_ int nSessionId,
		_OPT_ _IN_ const char * pszConfig,	
		_MUST_ _OUT_ OCR_DESKEW_RESULT * psOcrDeskewResult
		);



/**  
 * @brief	ͼƬ�������루�ݲ�֧�֣�
 * @param	nSessionId		�ỰID
 * @param	pszConfig		��������
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_UNSUPPORT</td><td>�ݲ�֧��</td></tr>
 *	</table>
 *
 */ 

HCI_ERR_CODE HCIAPI hci_ocr_denoise(_MUST_ _IN_ int nSessionId,	_OPT_ _IN_ const char * pszConfig);


/**  
 * @brief	�ͷ���бУ������ڴ�
 * @param	psOcrDeskewResult	��Ҫ�ͷŵ���бУ��������ڴ�ָ��
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_free_deskew_result(
		_MUST_ _IN_ OCR_DESKEW_RESULT * psOcrDeskewResult
		);

/**  
 * @brief	���а������
 * @param	nSessionId		�ỰID
 * @param	pszConfig		�������ã�ASCII���룬��'\0'��������ΪNULL
 * @param	psOcrLayoutResult	������������ָ�룬ʹ����Ϻ���ʹ�� hci_ocr_free_layout_result() ���������ͷ�
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>���ò����������趨ֵ�Ƿ����ʽ�����</td></tr>
 *		<tr><td>@ref HCI_ERR_DATA_SIZE_TOO_LARGE</td><td>��������ݳ����ɴ�������ޣ��ƶ�ʶ�����ݷ����ƶ�ʱ�ж�[0,5M]</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>����keyδ�ҵ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�����ڴ�ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>δ�ҵ���Ч���Ʒ����ַ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>���ӷ�����ʧ�ܣ�����������Ӧ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>���������ʳ�ʱ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>���������ص����ݸ�ʽ����ȷ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_RESPONSE_FAILED</td><td>���������ذ������ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>������������ܲ����б��У����˴���֧��</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>�������棬�����������ʧ�ܣ������ƶ�ʶ��ͼ����ת��ת������ֵ����ѹ��ʧ��</td></tr>
 *	</table>
 *
 * @par ���ô����壺
 * ���ô�����"�ֶ�=ֵ"����ʽ������һ���ַ���������ֶ�֮����','�������ֶ������ִ�Сд��
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>�ֶ�</b></td>
 *			<td><b>ȡֵ��ʾ��</b></td>
 *			<td><b>ȱʡֵ</b></td>
 *			<td><b>����</b></td>
 *			<td><b>��ϸ˵��</b></td>
 *		</tr>
 *		<tr>
 *			<td>imageType</td>
 *			<td>normal, screen</td>
 *			<td>normal</td>
 *			<td>ͼ������</td>
 *			<td>normal: ��ͨͼ��<br/>
 *              screen: ����ͼ��
 *			</td>
 *		</tr>
 *		<tr>
 *			<td>binarize</td>
 *			<td>adaptive, global</td>
 *			<td>adaptive</td>
 *			<td>��ֵ���������ԻҶȻ��߲�ɫͼ����Ч</td>
 *			<td>global: ȫ�ֶ�ֵ���㷨����Ҫ���ͨ��ɨ�衢�����Ȼ�ȡ��ͼ��<br/>
 *              adaptive: �ֲ�����Ӧ��ֵ���㷨����Ҫ���ͨ�����ա�����ͷ�Ȼ�ȡ���ܹ���Ӱ��ϴ�ͼ��
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>localProcess</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>�Ƿ���������Ԥ�����ƶ�����OCR֧�ָ�����@n
 *				������Ҫ��ָ��ֵ��</td>
 *			<td>yes: ����Ԥ����<br/>
 *              no: ������Ԥ����
 *			</td>
 *		</tr> 
 *	</table>
 * @n@n
 * ����û�ж�����������ʹ�� session_start ʱ�Ķ��塣��� session_start ʱҲû�ж��壬��ʹ��ȱʡֵ
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_layout_analysis(
		_MUST_ _IN_ int nSessionId,
		_OPT_ _IN_ const char * pszConfig,	
		_MUST_ _OUT_ OCR_LAYOUT_RESULT * psOcrLayoutResult
		);

/**  
 * @brief	�ͷŰ������������ڴ�
 * @param	psOcrLayoutResult	��Ҫ�ͷŵİ�����������ָ��
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_free_layout_result(
		_MUST_ _IN_ OCR_LAYOUT_RESULT * psOcrLayoutResult
		);

/**  
 * @brief	����OCRʶ��
 * @param	nSessionId			�ỰID
 * @param	pszConfig			ʶ��������ô�����ΪNULL����'\0'
 * @param	pRegions			ʶ�������б�
 * @param	nRegionCount		ʶ���������Ŀ��
 * @param	psOcrRecogResult	ʶ���������ݽṹָ�룬ʹ����Ϻ���ʹ�� hci_ocr_free_recog_result() ���������ͷ�
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_DATA_SIZE_TOO_LARGE</td><td>��������ݳ����ɴ�������ޣ��ƶ�ʶ�����ݷ����ƶ�ʱ�ж�[0,5M]</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>������Ƿ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>����keyδ�ҵ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�����ڴ�ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>δ�ҵ���Ч���Ʒ����ַ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>���ӷ�����ʧ�ܣ�����������Ӧ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>���������ʳ�ʱ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>���������ص����ݸ�ʽ����ȷ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_RESPONSE_FAILED</td><td>������������бУ��ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>������������ܲ����б��У����˴���֧��</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>�������棬ʶ�����ʧ�ܣ������ƶ�ʶ��ͼ����ת��ת������ֵ����ѹ��ʧ��</td></tr>
 *	</table>
 * 
 * @par ���ô����壺
 * ���ô�����"�ֶ�=ֵ"����ʽ������һ���ַ���������ֶ�֮����','�������ֶ������ִ�Сд��������ÿ��������֧��@n@n
 * �б��е����������������֧�ֵ���������ο�@ref hci_ocr_page
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>ģ��</b></td>
 *			<td><b>�ֶ�</b></td>
 *			<td><b>ȡֵ��ʾ��</b></td>
 *			<td><b>ȱʡֵ</b></td>
 *			<td><b>����</b></td>
 *			<td><b>��ϸ˵��</b></td>
 *		</tr> 
 *		<tr>
 *			<td rowspan="2">ͨ������</td>
 *			<td>cutEdge</td>
 *			<td>yes, no </td>
 *			<td>no</td>
 *			<td>ԭʼͼƬ��Ҫ�ñ�</td>
 *			<td>yes,��Ҫ�ñ�,����������Ƭ���ã�ԭʼͼ��Ϊδ�ñ�ͼ��<br/>
 *				no, ����Ҫ�ñߣ�������ͼƬʹ�ã�ԭʼͼ��Ϊ�ñ�ͼ��<br/>
 *			</td>
 *		</tr>
 *		<tr>
 *			<td>detailResult</td>
 *			<td>no, yes</td>
 *			<td>no</td>
 *			<td>�Ƿ񷵻���ϸ���</td>
 *			<td>ȱʡֻ����ʶ�������ı����������Ϊyes����᷵����ϸ�Ľڵ���Ϣ @ref OCR_RECOG_NODE </td>
 *		</tr>
 *		<tr>
 *			<td rowspan="7">�ƶ����ĺͱ����ı�ʶ��</td>
 *			<td>recogRange</td>
 *			<td>gb, letter</td>
 *			<td>all</td>
 *			<td>ʶ��Χ</td>
 *			<td>number: ���� 0 - 9, 10��<br/>
 *				uppercase: ��дӢ����ĸ, A - Z, 26��<br/>
 *				lowercase: СдӢ����ĸ, a - z, 26��<br/>
 *				letter: ��Сд��ĸ, 52��<br/>
 *				alnum: ��Сд��ĸ������, 62��<br/>
 *				gb: ���ļ��庺��<br/>
 *				big5: ���ķ��庺��
 *				all: �����ַ����ĺϼ�,Ӣ������ʱ������gb��big5<br/>
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>autoDeskew</td>
 *			<td>no, yes</td>
 *			<td>no</td>
 *			<td>�Ƿ�ʶ��ǰ�Զ���бУ����֧�֣�û������ʶ�������б���������Ч</td>
 *			<td></td>
 *		</tr>
 *		<tr>
 *			<td>autoLayout</td>
 *			<td>no, yes</td>
 *			<td>no</td>
 *			<td>�Ƿ�ʶ��ǰ�����Զ����������û������ʶ�������б���������Ч</td>
 *			<td></td>
 *		</tr>
 *		<tr>
 *			<td>fullHalf</td>
 *			<td>full, half</td>
 *			<td>half</td>
 *			<td>��������ʶ��ģʽ�����Ӣ����ĸ��������ȫ�ǻ��ǰ��</td>
 *			<td>full: ȫ��<br/>half: ���</td>
 *		</tr>
 *		<tr>
 *			<td>dispCode</td>
 *			<td>nochange, tosimplified</td>
 *			<td>nochange</td>
 *			<td>��������������ת�����ƶ����ĺͱ���OCR֧�ָ�����</td>
 *			<td>nochange: ���岻���仯<br/>tosimplified: ����ת��Ϊ����</td>
 *		</tr> 
 *		<tr>
 *			<td>imageType</td>
 *			<td>normal, screen</td>
 *			<td>normal</td>
 *			<td>ͼ������</td>
 *			<td>normal: ��ͨͼ��<br/>
 *              screen: ����ͼ��
 *			</td>
 *		</tr>  
 *		<tr>
 *			<td>binarize</td>
 *			<td>adaptive, global</td>
 *			<td>adaptive</td>
 *			<td>��ֵ������,�ԻҶȻ��߲�ɫͼ����Ч</td>
 *			<td>global: ȫ�ֶ�ֵ���㷨����Ҫ���ͨ��ɨ�衢�����Ȼ�ȡ��ͼ��<br/>
 *              adaptive: �ֲ�����Ӧ��ֵ���㷨����Ҫ���ͨ�����ա�����ͷ�Ȼ�ȡ���ܹ���Ӱ��ϴ�ͼ��
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>�ƶ�����</td>
 *			<td>localProcess</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>�Ƿ���������Ԥ����(�ƶ˶����ֱ�������Ϊno)@n
 *				������Ҫ��ָ��ֵ��</td>
 *			<td>yes: ����Ԥ����<br/>
 *              no: ������Ԥ����
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>�ƶ˶�����OCR</td>
 *			<td>export</td>
 *			<td> none,xml,pdf,docx,rtf,xlsx,text,html,pptx</td>
 *			<td>none</td>
 *			<td>�����ļ���ʽ����</td>
 *			<td>none:���Ե����ļ���ʽ���ؽ��<br/>
				pdf:pdf��ʽ�ļ�<br/>
				docx:docx��ʽ�ļ�<br/>
				rtf:rtf��ʽ�ļ�<br/>
				xlsx:xlsx��ʽ�ļ�<br/>
				text:text��ʽ�ļ�<br/>
				html:html��ʽ�ļ�<br/>
				pptx:pptx��ʽ�ļ�<br/>
 *			</td>
 *		</tr>
 *		<tr>
 *			<td rowspan="4">ģ��ʶ��</td>
 *			<td>templateId</td>
 *			<td>1</td>
 *			<td>��</td>
 *			<td>����ʶ��ʹ�õ�ģ��ID</td>
 *			<td>���ر�ѡ<br/>
 *				����ʶ��ʱ������������ hci_ocr_load_template()���ص�ID��<br/>
 *				�ƶ�ʶ��ʱ����ʹ��domain����ָ����ʹ�õ�ʶ��ģ��</td>
 *		</tr>
 *		<tr>
 *			<td>templateIndex</td>
 *			<td>0����Ӧģ���ļ��е����ģ��������</td>
 *			<td>��</td>
 *			<td>ֻ���ģ��ʶ����Ч��ģ���ļ��е�ģ��������</td>
 *			<td>��ѡ��ģ���ļ��п����ж��ģ�壬�������֤��Ʊ�ݵȣ�������ָ��ģ����ģ���ļ��е�������</td>
 *		</tr> 
 *		<tr>
 *			<td>templatePageIndex</td>
 *			<td>0����Ӧģ���е����ҳ������</td>
 *			<td>��</td>
 *			<td>ֻ���ģ��ʶ����Ч��ģ��ҳ������</td>
 *			<td>��ѡ��ģ���п��ܴ��ڶ��ģ��ҳ���������֤������ͷ��棬������ָ��ģ��ҳ��ģ���е�������</td>
 *		</tr> 
 *		<tr>
 *			<td>domain</td>
 *			<td>dlcard��vlcard��idcard</td>
 *			<td>��</td>
 *			<td>ָ���ƶ�ģ��ʶ����ʹ�õ�ģ��</td>
 *			<td>�ƶ˱�ѡ<br/>
 *				Ŀǰ����ָ��Ϊdlcard����ʻ֤����vlcard����ʻ֤����idcard�����֤��</td>
 *		</tr>
 *	</table>
 * @n@n
 * ����û�ж�����������ʹ�� session_start ʱ�Ķ��塣��� session_start ʱҲû�ж��壬��ʹ��ȱʡֵ��@n
 * ʹ�ñ���ģ��ʶ��ʱ������cutEdgeΪno�������ڲ����Զ����бߴ���@n
 * �ƶ�ģ��ʶ��ʹ��ԭʼͼ�����ݽ���ʶ�𣬼�SDK�����κ�Ԥ����
 * @note
 * pRegions ���ΪNULL��nRegionCountΪ0���������ô��е� autoLayout, �����true�����̨�����Զ����������
 * �����false��������ͼ����һ�������ĺ��������������ʶ�� @n@n
 * pRegions �����ΪNULL��nRegionCount��Ϊ0�����ô���֧��autoDeskew��autoLayout����������������ǲ�������бУ���Ͱ������, 
 * ��ʹ�ô����pRegions��Ϊʶ������
 *
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_recog(
		_MUST_ _IN_ int nSessionId,	
		_OPT_ _IN_ const char * pszConfig,			
		_OPT_ _IN_ const OCR_RECOG_REGION * pRegions, 
		_OPT_ _IN_ unsigned int nRegionCount,
		_MUST_ _OUT_ OCR_RECOG_RESULT * psOcrRecogResult
		);

/**  
 * @brief	�ͷ�ʶ�����ڴ�
 * @param	psOcrRecogResult	��Ҫ�ͷŵ�ʶ�������ڴ�ָ��
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_free_recog_result(
		_MUST_ _IN_ OCR_RECOG_RESULT * psOcrRecogResult
		);

/**  
 * @brief	�����Ự
 * @param	nSessionId		�ỰID
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_session_stop(
		_MUST_ _IN_ int nSessionId
		);

/**  
 * @brief	����OCR���� ����ʼ��
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_ACTIVE_SESSION_EXIST</td><td>����δstop��Sesssion���޷�����</td></tr>
 *	</table>
 */
HCI_ERR_CODE HCIAPI hci_ocr_release();

/**  
 * @brief	ͨ���ڴ滺�����ñ��Ự�����ͼ��
 * @param	nSessionId			�ỰID
 * @param	pImageData			Ҫ���õı����ڴ滺��
 * @param	uiImageDataSize		Ҫ���õı����ڴ滺��ĳ��ȣ��ֽ�Ϊ��λ
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *	</table>
 */
HCI_ERR_CODE HCIAPI hci_ocr_set_image_buffer(
		_MUST_ _IN_ int nSessionId ,
		_MUST_ _IN_ unsigned char * pImageData, 
		_MUST_ _IN_ unsigned int uiImageDataSize
		);

/**  
 * @brief	ͨ���ļ����ñ��ػ������ͼ��
 * @param	nSessionId			�ỰID
 * @param	pszImageFileName	Ҫ���õı���ͼƬ�ļ�
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *	</table>
 */

HCI_ERR_CODE HCIAPI hci_ocr_set_image_file(
		_MUST_ _IN_ int nSessionId ,
		_MUST_ _IN_ const char * pszImageFileName
		);


/**  
* @brief	��ȡ�Ự�е�ͼ�����ݽṹ���û�����������ʾ
* @param	nSessionId			�ỰID
* @param	pImage				���ص�ͼ��ṹ���û������OCR_IMAGE�ṹ�����ĵ�ַ��ʵ��(������ʾ����Ҫ�޸ķ���ͼ�������)
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
*		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
*		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>���ó�ʼ����Ϊͼ�񻺳�ʱ������������ʱת��ΪOCR_IMAGE�ṹʧ��</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_IMAGE_NOT_SET</td><td>���Ự��δ����ͼ������</td></tr>
*	</table>
*/
HCI_ERR_CODE HCIAPI hci_ocr_get_image( 
									  _MUST_ _IN_ int nSessionID,
									  _MUST_ _OUT_ OCR_IMAGE* pImage
									  );

/**  
* @brief	����һ�������ļ����ļ���ʽ���ļ�����׺ȷ��(Ŀǰ֧��bmp��jpg��png��tif)
* @param	pszImageFileName	���浽�ı���ͼ���ļ���
* @param	pImage				Ҫ�����Դͼ�����ݽṹ
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
*		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_SAVE_IMAGE</td><td>���汾��ͼ���ļ�ʧ��</td></tr>
*	</table>
* @note 
*		iOSƽ̨��֧�ִ˲��������ô˺����᷵�� HCI_ERR_UNSUPPORT�����ϣ����iOS�洢ͼ�����ݣ�
*		��ʹ�� hci_ocr_get_uiImage() ��ȡUIImage��Ȼ��ʹ��UIImage�Ĵ洢�ļ����ܽ��д洢��
*/

HCI_ERR_CODE HCIAPI hci_ocr_save_image_file( 
											_MUST_ _IN_ const char * pszImageFileName,
											_MUST_ _IN_ OCR_IMAGE * pImage
											);

/**  
* @brief	����һ��ģ���ļ���ͨ�������б�����ID
* @param	pszTemplateFile		Ҫ�����ģ���ļ�·��
* @param	pnTemplateID		���ڷ�������ɹ���ȡ��ģ���ļ�ID
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
*		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_LOAD_TEMPLATE_FAILED</td><td>����ģ��ʧ�ܣ��������ļ������ڡ��ļ����ɶ����ļ��Ƿ����߽���ʧ��</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_TEMPLATE_OVERLOAD</td><td>�����ģ�������������ߣ�Ŀǰ��ֻ�ܼ���1��ģ��</td></tr>
*	</table>
* @note 
*		����ģ��ʶ��ʱʹ�ã�����ģ�壬��ȡ��ID���� hci_ocr_session_start()ʱ���ڴ����Ự�����session�ɹ���һ��templateID��
*	�������ͬһ��ģ���ļ������д�����Ϊ�Ƕ����Ķ��ģ���ҵõ����ID����ЩID����Ҫ��Ӧж�ء�

*/
HCI_ERR_CODE hci_ocr_load_template(const char *pszTemplateFile, int *pnTemplateID);




/**  
* @brief	ж��һ���Ѿ������ģ���ļ�
* @param	nTemplateID			Ҫж�ص�ģ���ļ���ID���� hci_ocr_load_template()����
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_TEMPLATE_ID_INVALID</td><td>�����ģ��ID���ڼ��ص�ģ���б���</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_TEMPLATE_USING</td><td>���лỰ��ʹ��Ҫж�ص�ģ��</td></tr>
*	</table>
* @note 
*		�û���ж��ģ��ʱ�����뱣֤ʹ�ø�ģ�������session���Ѿ�ִ�� hci_ocr_session_stop()���������򷵻ش���
*/
HCI_ERR_CODE hci_ocr_unload_template(int nTemplateID);


#ifdef __OBJC__
/**  
 * @brief	װ��һ��UIimageͼ�񣬽��뵽ͼ�����ݽṹ�����õ�ָ���ĻỰ
 * @param	nSessionID			Ҫ����ͼ��ṹ�ĻỰ
 * @param	image				Ҫ�����UIImage����
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>UIiamgeת����ocrimage��ʱ��ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�����ڴ�ʧ��</td></tr>
 *	</table>
 * @note 
 *		��iOSƽ̨֧�ִ˺���
 */
HCI_ERR_CODE HCIAPI hci_ocr_set_image_by_uiImage( 
		_MUST_ _IN_ int nSessionID,
		_MUST_ _IN_ UIImage * image
		);


/**  
 * @brief	��OCR_IMAGE����ת����UIImage����
 * @param	nSessionId			�ỰID
 * @param	image				ת�����UIImage���󣨴˶�����autorelease�ģ�
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����Ĳ������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�����ڴ�ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>OCR_IMAGEת����UIImage��ʱ��ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_UNSUPPORT</td><td>��֧�ִ����OCR_IMAGE��ʽ</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>�����Session�Ƿ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>���ó�ʼ����Ϊͼ�񻺳�ʱ������������ʱת��ΪOCR_IMAGE�ṹʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_IMAGE_NOT_SET</td><td>���Ự��δ����ͼ������</td></tr>
 *	</table>
 * @note 
 *		��iOSƽ̨֧�ִ˺���
 */
HCI_ERR_CODE HCIAPI hci_ocr_get_uiImage( 
		_MUST_ _IN_ int nSessionID,
		_MUST_ _OUT_ UIImage ** image
		);

#endif

/* @} */
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif


#endif
