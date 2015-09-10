/** 
 * @file    hci_ocr.h 
 * @brief   HCI_OCR SDK 头文件  
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

/** @defgroup HCI_OCR 灵云OCR能力API */
/* @{ */

//////////////////////////////////////////////////////////////////////////
// 常量定义

#define OCR_CAP_PROP_LANG		"lang"		///< OCR能力的属性名称：语言
#define OCR_CAP_PROP_DOMAIN		"domain"	///< OCR能力的属性名称：领域


#pragma pack (push, ocr_api)
#pragma pack (8) 

	//////////////////////////////////////////////////////////////////////////
	/// @brief 图像结构体.
	/// @details 图像结构体，图像数据需存放在一块连续的内存区域内，但可能
	///		因四字节对齐，导致每行后补充了1到3个字节的无用数据. 
	typedef struct OCR_IMAGE_T
	{
		_MUST_ _IN_ int nWidth;		///< 图像宽度
		_MUST_ _IN_ int nHeight;		///< 图像高度

		/// 只能取三个值1, 8, 24，分别表示图像为单色图、灰度图、彩色图
		_MUST_ _IN_ int nBitsPerPixel;
		/// 二维数组指针, #pLines[i]指向第i行图像数据的首地址，i的取值范围：[0, #nHeight)@n
		/// 彩色图像各点的三字节存放顺序为RGB，即图像数据依次为RGBRGB...@n
		/// 图像上的(0, 0)点位于图像的左上角:
		///		+ 灰度图时(0, 0)点数据是第一行上的第一个字节，即 #pLines[0][0]；
		/// 	+ 单色图时(0, 0)点数据在第一行上的第一个字节的最高位；
		/// 	+ 彩色图时(0, 0)点的RGB三字节依次是 #pLines[0][0], #pLines[0][1], #pLines[0][2]
		_MUST_ _IN_ unsigned char ** pLines;

		_OPT_ _IN_ int xResolution;	///< x 分辨率(DPI)，如果未知赋0
		_OPT_ _IN_ int yResolution;	///< y 分辨率(DPI)，如果未知赋0 
	}OCR_IMAGE;

#pragma pack (pop, ocr_api)

/**
 * @brief 字符节点中的候选字最大项目
 */
#define CANDIDATE_NUM		5
	
/**
 * @brief	区域类型
 */
typedef enum _tag_OCR_RECOG_RGNTYPE
{
	OCR_RECOG_RGNTYPE_HORZTEXT = 0,		///< 横排文本
	OCR_RECOG_RGNTYPE_VERTTEXT = 1,		///< 竖排文本
	OCR_RECOG_RGNTYPE_TABLE = 2,		///< 表格区域
	OCR_RECOG_RGNTYPE_GRAPH = 3,		///< 图片区域，不参与识别
	OCR_RECOG_RGNTYPE_AUTOTEXT = 4,		///< 文本区域，由引擎自动判断横竖排版，当前版本不支持
}
OCR_RECOG_RGNTYPE;

/**
 * @brief	区域识别的语言控制，此项仅在选择了中文引擎的时候有效
 */
typedef enum _tag_OCR_RECOG_LANG
{
	OCR_RECOG_LANG_DEFAULT = 0,		///< 缺省语言，由识别配置串决定
	OCR_RECOG_LANG_ZH_CN = 1,		///< 此区域强制采用简体中文识别
	OCR_RECOG_LANG_ZH_HK = 2,		///< 此区域强制采用繁体中文识别，与OCR_RECOG_LANG_TW 目前无区别
	OCR_RECOG_LANG_ZH_TW = 3,		///< 此区域强制采用繁体中文识别，与OCR_RECOG_LANG_HK 目前无区别
	OCR_RECOG_LANG_EN = 4			///< 此区域强制采用英文引擎识别
}
OCR_RECOG_LANG;

/**
 * @brief 线段类型
 */
typedef enum _tag_OCR_TABLE_LINE_TYPE
{
	LINE_TYPE_UNKNOWN = 0,				///< 未知
	LINE_TYPE_REAL,						///< 实线
	LINE_TYPE_DASHED,					///< 虚线
	LINE_TYPE_DOTTED,					///< 点线
	LINE_TYPE_VIRTUAL,					///< 没有实际的线, 但逻辑上有的线
	LINE_TYPE_EMPTY		= 0x100,		///< 没有线，对应于两个表格单元格的合并
}
OCR_TABLE_LINE_TYPE;

/**
 * @brief 识别结果节点类型
 */
typedef enum _tag_OCR_RECOG_NODE_TYPE
{
	OCR_RECOG_NODE_PAGE	= 0,		///< 页面节点
	OCR_RECOG_NODE_REGION,			///< 区域节点
	OCR_RECOG_NODE_CELL,			///< 表格单元节点
	OCR_RECOG_NODE_LINE,			///< 文本行节点
	OCR_RECOG_NODE_CHAR,			///< 字符节点
}
OCR_RECOG_NODE_TYPE;

//////////////////////////////////////////////////////////////////////////
// 数据结构定义

/**
 * @brief 点结构
 */
typedef struct _tag_OCR_RECOG_POINT
{
	int x;			///< X坐标
	int y;			///< Y坐标
}
OCR_RECOG_POINT;

/**
 * @brief 切边矫正校点结构
 */
typedef struct _tag_OCR_CORNERS_RESULT
{
	OCR_RECOG_POINT leftTop;		///< 左上校点坐标
	OCR_RECOG_POINT rightTop;		///< 右上校点坐标
	OCR_RECOG_POINT leftBottom;		///< 左下校点坐标
	OCR_RECOG_POINT rightBottom;	///< 右下校点坐标
	int	angle;						///< 倾斜角度
}
OCR_CORNERS_RESULT;

/**
 * @brief 矩形结构
 */
typedef struct _tag_OCR_RECOG_RECT
{
	int left;		///< 左
	int top;		///< 上
	int right;		///< 右
	int bottom;		///< 下
}
OCR_RECOG_RECT;

//----------------------------------------------------------------------------
// 版面相关信息定义

/**
 * @brief	表格线子线段的定义
 * @details	每根表格线 (OCR_RECOG_FRAME_LINE) 是由多个表格线子线段组成，这样可以支持弯曲的线
 *			\n\n
 *			后一个表格线子线段的起始坐标(nStartX, nStartY)等于前一个表格线子线段的结束坐标(nEndX, nEndY), 这里是冗余的
 */
typedef struct _tag_OCR_RECOG_LINESEGMENT 
{
	/// 当前表格线子线段的起始X坐标
	int nStartX;

	/// 当前表格线子线段的起始Y坐标
	int nStartY;

	/// 当前表格线子线段的终止X坐标
	int nEndX;

	/// 当前表格线子线段的终止Y坐标
	int nEndY;

	/// 子线段的宽度
	int nThickness;

	/// 子线段的类型，参见 OCR_TABLE_LINE_TYPE 的定义
	OCR_TABLE_LINE_TYPE nType;
} 
OCR_RECOG_LINESEGMENT;

/**
 * @brief	表格线的定义
 * @details	由于可能存在弯曲的情况，因此表格线是用多个表格线子线段(OCR_RECOG_LINESEGMENT) 来表示的
 */
typedef struct _tag_OCR_RECOG_FRAMELINE
{ 
	/// 详细的表格线子线段信息
	OCR_RECOG_LINESEGMENT * pSegment;    //对应的表格线小段的第一段的指针

	/// 表格线子线段的数目
	int nSegmentCount;
} 
OCR_RECOG_FRAMELINE;

/**
 * @brief	表格单元定义
 * @details	由于可能存在弯曲的情况，这里的坐标和宽度只是近似坐标，具体的表格线是用多个表格线子线段 
 *			(OCR_RECOG_LINESEGMENT) 来表示的
 */
typedef struct _tag_OCR_RECOG_TABLECELL 
{
	/// 表格区域的左上角坐标，由于存在倾斜，这个是更为精确的表格单元位置
	OCR_RECOG_POINT LT;

	/// 表格区域的左下角坐标，由于存在倾斜，这个是更为精确的表格单元位置
	OCR_RECOG_POINT LB;

	/// 表格区域的右上角坐标，由于存在倾斜，这个是更为精确的表格单元位置
	OCR_RECOG_POINT RT;

	/// 表格区域的左下角坐标，由于存在倾斜，这个是更为精确的表格单元位置
	OCR_RECOG_POINT RB;

	/// Cell的编号，从0开始，具有相同编号的SubCell连续排列，合并成一个完整的表格单元
	int nCellNo;    
} 
OCR_RECOG_TABLECELL;

/**
 * @brief	表格的定义
 */
typedef struct _tag_OCR_RECOG_TABLE 
{
	/// 所有的水平线数组
	OCR_RECOG_FRAMELINE  *HLines;	

	/// 所有的水平直线个数
	int nHLine;

	/// 所有的垂直线数组
	OCR_RECOG_FRAMELINE  *VLines;

	/// 所有的垂直直线个数
	int nVLine;

	/// 所有的表格单元数组，总数为(nHLine-1)*(nVLine-1)，由于可能有合并单元，因此数目和 nCellNoCount 并不一致
	OCR_RECOG_TABLECELL  *pCell;		

	/// 所有的CellNo的数目，由于可能有合并单元，此数目并不一定等于(nHLine-1)*(nVLine-1)
	int nCellNoCount;
}
OCR_RECOG_TABLE;

/**
 * @brief	识别区域的定义
 */
typedef struct _tag_OCR_RECOG_REGION
{
	/// 区域相对于整个图像的位置，像素单位
	OCR_RECOG_RECT	rcBound;       	

	/// 区域的类型 (OCR_RECOG_RGNTYPE_xxx)
	/// OCR_RECOG_RGNTYPE_GRAPH 类型的区域不会参与识别，但是在详细节点信息中，会作为一个区域节点存在
	OCR_RECOG_RGNTYPE	wRegionType;

	/// 区域识别的语言控制 (OCR_RECOG_LANG_xxx)，仅在选择中文引擎识别时有效，可以控制是简繁体或纯英文,
	/// 此时可以覆盖识别配置串所指定的语言
	OCR_RECOG_LANG	wRecogLang;
	
	/// 如果是表格区域，其表格信息 (此指针总是由引擎来维护，不要试图自己管理)
	OCR_RECOG_TABLE  * pTable;		
}
OCR_RECOG_REGION;

/**
 * @brief	版面分析函数的返回结果
 */
typedef struct _tag_OCR_RECOG_LAYOUT_RESULT
{
	/// 区域列表
	OCR_RECOG_REGION  * pRegionList;

	/// 区域的数目
	int    nRegionCount;
}
OCR_LAYOUT_RESULT;

/**
 * @brief	倾斜校正函数的返回结果
 */
typedef struct _tag_OCR_RECOG_DESKEW_RESULT
{
	/// 检测到的倾斜角度, 单位为 1/100度，顺时针为正
	int nSkewAngle;
}
OCR_DESKEW_RESULT;

//----------------------------------------------------------------------------
// 文档结果定义

/**
 * @brief	页面节点定义
 */
typedef struct _tag_OCR_RECOG_PAGENODE
{
	/// 页面宽度
	int nWidth;

	/// 页面高度
	int nHeight;

	/// 如果进行了自动倾斜校正，其原始的倾斜角度, 单位为 1/100度，顺时针为正
	int nSkewAngle;
}
OCR_RECOG_PAGENODE;

/**
 * @brief	区域节点定义
 */
typedef struct _tag_OCR_RECOG_REGIONNODE
{
	/// 区域相对于整个图像的位置，像素单位
	OCR_RECOG_RECT	rcBound;

	/// 区域的类型 (OCR_RECOG_RGNTYPE_xxx)
	unsigned short	wRegionType;	

	/// 仅对选择中文引擎的识别有效，此时这里是区域的实际识别语言(OCR_RECOG_LANG_xxx)
	/// 假如区域定义中传入是 OCR_RECOG_LANG_DEAFAULT, 也会变为实际的 OCR_RECOG_LANG_xxxx
	unsigned short	wRecogLang;		
}
OCR_RECOG_REGIONNODE;

/**
 * @brief	表格单元节点定义
 */
typedef struct _tag_OCR_RECOG_CELLNODE
{
	/// 此表格单元的位置
	OCR_RECOG_RECT	rcCellBound;	
}
OCR_RECOG_CELLNODE;

/**
 * @brief	文本行节点定义
 */
typedef struct _tag_OCR_RECOG_LINENODE
{
	/// 此行的基线。横排版中为上边线，竖排版中为左边线
	int	nBaseLine;
	
	/// 此行的大小，横排版中为行高，竖排版中为行宽
	int nLineSize;
	
	/// 此行的平均字符宽度
	int xChar;

	/// 此行的平均字符高度
	int yChar;
}
OCR_RECOG_LINENODE;

/**
 * @brief	字符节点定义
 */
typedef struct _tag_OCR_RECOG_CHARNODE
{   
	/// 识别结果单字，UTF-8编码，以'\0'结束
	char * pszChar;

	/// 多个候选字的识别结果，UTF-8编码，以'\0'结束
	/// 第一个字有可能和pszchar一样，但也有可能不一样
	char * pszCandidateWords[CANDIDATE_NUM];

	/// 此字符前面的空格字符数
	int	nSpaceBefore;

	/// 是否可信
	bool bAccept;

	/// 此字符在图像中区域
	OCR_RECOG_RECT	rcBlock;
}
OCR_RECOG_CHARNODE;

/**
 * @brief	OCR识别结果的详细节点信息
 */
typedef struct _tag_OCR_RECOG_NODE
{
	///  节点类型
	OCR_RECOG_NODE_TYPE	wType;				

	/// 具体节点信息
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
 * @brief	OCR识别的返回结果
 */
typedef struct _tag_OCR_RECOG_RESULT
{
	/// 识别结果文本, UTF-8编码, 以'\0'结束，不管配置项 detailResult 设为是否需要详细信息，这项都会存在
	char *	pTextBuf;			

	/// 识别结果的详细节点列表，只有在配置项 detailResult 设为 yes 的时候有效，否则为NULL
	OCR_RECOG_NODE * pNodes;				

	/// 识别结果的详细节点的数目，只有在配置项 detailResult 设为 yes 的时候有效，否则为0
	int		nNodeCount;

	///导出文件内容
	char *	pExportFileBuf;
	///导出文件长度
	int 	nExportFileBufLen;	

}
OCR_RECOG_RESULT;

/////////////////////////////////////////////////////////////////////////
// 接口API函数定义

/**  
 * @brief	灵云OCR能力 初始化
 * @param	pszConfig		全局配置串，ASCII编码，以'\0'结束
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS 尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ALREADY_INIT</td><td>OCR SDK多次初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>配置参数有误，如设定值非法或格式错误等</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_DATAPATH_MISSING</td><td>传入的不是OCR的capkey</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_CAPKEY_NOT_MATCH</td><td>缺少必需的dataPath配置项</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>没有找到指定的能力</td></tr>
 *		<tr><td>@ref HCI_ERR_LOCAL_LIB_MISSING</td><td>本地能力引擎缺失必要的库资源</td></tr>
 *	</table>
 *
 * @par 配置串定义：
 * 配置串是由"字段=值"的形式给出的一个字符串，多个字段之间以','隔开。字段名不分大小写。
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>字段</b></td>
 *			<td><b>取值或示例</b></td>
 *			<td><b>缺省值</b></td>
 *			<td><b>含义</b></td>
 *			<td><b>详细说明</b></td>
 *		</tr>
 *		<tr>
 *			<td>dataPath</td>
 *			<td>opt/myapp/ocr_data</td>
 *			<td>空</td>
 *			<td>光学字符识别本地资源所在路径</td>
 *			<td>不使用本地能力时，此项不传</td>
 *		</tr> 
 *		<tr>
 *			<td>initCapKeys</td>
 *			<td>ocr.local</td>
 *			<td>空</td>
 *			<td>初始化所需的本地能力</td>
 *			<td>多个能力以';'隔开，传入的能力必须合法，否则返回错误。忽略传入的云端能力key。如果不使用本地识别能力，此项不传。</td>
 *		</tr>
 *		<tr>
 *			<td>fileFlag</td>
 *			<td>none, android_so</td>
 *			<td>none</td>
 *			<td>获取本地文件名的特殊标记</td>
 *			<td>不使用本地能力时，此项不传。详细说明参见下面的注释</td>
 *		</tr>
 *	</table>
 *
 *  <b>Android特殊配置</b>
 *  @n
 *  当fileFlag为android_so时，加载本地资源文件(字典和编码库)时会将正常的库文件名更改为so文件名进行加载。
 *  例如，当使用的库为file.dat时，则实际打开的文件名为libfile.dat.so，这样在Android系统下，
 *  开发者可以按照此规则将本地资源改名后, 放在libs目录下打包入apk。在安装后，这些资源文件
 *  就会放置在/data/data/包名/lib目录下。则可以直接将dataPath配置项指为此目录即可。
 *
 * @note
 * 若是云端识别，此配置串可以直接传NULL或者""
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_init(
		_MUST_ _IN_ const char * pszConfig
		);

/**  
 * @brief	开始会话
 * @param	pszConfig		会话配置串，ASCII编码，以'\0'结束
 * @param	pnSessionId		成功时返回会话ID
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_CAPKEY_MISSING</td><td>缺少必需的capKey配置项</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>配置项非法</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_CAPKEY_NOT_MATCH</td><td>能力与key不匹配</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>传入的能力key未找到</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>未找到有效的云服务地址</td></tr>
 *		<tr><td>@ref HCI_ERR_TOO_MANY_SESSION</td><td>创建的Session数量超出限制(256)</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>分配内存失败</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>传入的配置在总参数列表中，但此处不支持</td></tr>
 *		<tr><td>@ref HCI_ERR_LOAD_FUNCTION_FROM_DLL</td><td>要载入的模块不存在，或者需要的功能在该模块不存在</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_INIT_FAILED</td><td>本地识别时，OCR引擎初始化失败</td></tr>
 *	</table>
 *
 * @par 配置串定义：
 * 配置串是由"字段=值"的形式给出的一个字符串，多个字段之间以','隔开。字段名不分大小写。
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>字段</b></td>
 *			<td><b>取值或示例</b></td>
 *			<td><b>缺省值</b></td>
 *			<td><b>含义</b></td>
 *			<td><b>详细说明</b></td>
 *		</tr>
 *		<tr>
 *			<td>capKey</td>
 *			<td>ocr.cloud</td>
 *			<td>无</td>
 *			<td>OCR识别能力key</td>
 *			<td>参见 @ref hci_ocr_page 。每个session只能定义一种能力，并且过程中不能改变。</td>
 *		</tr>
 *	</table>
 * @n@n
 * 另外，这里还可以传入倾斜校正、版面分析和识别的配置项，作为默认配置项。参见 hci_ocr_deskew(), hci_ocr_layout_analysis(), hci_ocr_recog() 。
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_session_start(
		_MUST_ _IN_ const char * pszConfig,
		_MUST_ _OUT_ int *pnSessionId
		);

/**  
 * @brief	倾斜校正
 * @param	nSessionId		会话ID
 * @param	pszConfig		参数配置，ASCII编码，以'\0'结束，可为NULL
 * @param	psOcrDeskewResult	倾斜校正结果的指针，使用完毕后，需使用 hci_ocr_free_deskew_result() 函数进行释放
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>配置参数有误，如设定值非法或格式错误等</td></tr>
 *		<tr><td>@ref HCI_ERR_DATA_SIZE_TOO_LARGE</td><td>传入的数据超过可处理的上限, 云端识别数据发往云端时判断[0,5M]</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>能力key未找到</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>分配内存失败</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>未找到有效的云服务地址</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>连接服务器失败，服务器无响应</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>服务器访问超时</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>服务器返回的数据格式不正确</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_RESPONSE_FAILED</td><td>服务器返回倾斜校正失败</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>传入的配置在总参数列表中，但此处不支持</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>本地引擎，倾斜校正过程失败，或者云端识别图像旋转、转换、二值化和压缩失败</td></tr>
 *	</table>
 *
 * @par 配置串定义：
 * 配置串是由"字段=值"的形式给出的一个字符串，多个字段之间以','隔开。字段名不分大小写。
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>字段</b></td>
 *			<td><b>取值或示例</b></td>
 *			<td><b>缺省值</b></td>
 *			<td><b>含义</b></td>
 *			<td><b>详细说明</b></td>
 *		</tr> 
 *		<tr>
 *			<td>imageType</td>
 *			<td>normal, screen</td>
 *			<td>normal</td>
 *			<td>图像类型</td>
 *			<td>normal: 普通图像<br/>
 *              screen: 截屏图像
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>adjustImage</td>
 *			<td>yes, no</td>
 *			<td>no</td>
 *			<td>是否直接做校正</td>
 *			<td>yes: 校正<br/>
 *              no: 不校正
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>binarize</td>
 *			<td>adaptive, global</td>
 *			<td>adaptive</td>
 *			<td>二值化方法，对灰度或者彩色图像有效</td>
 *			<td>global: 全局二值化算法，主要针对通过扫描、截屏等获取的图像<br/>
 *              adaptive: 局部自适应二值化算法，主要针对通过拍照、摄像头等获取的受光照影响较大图像
 *			</td>
 *		</tr>
 *		<tr>
 *			<td>localProcess</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>是否先做本地预处理，云端中文OCR支持该配置@n
 *				现在主要是指二值化</td>
 *			<td>yes: 进行预处理<br/>
 *              no: 不进行预处理
 *			</td>
 *		</tr> 
 *	</table>
 * @n@n
 * 这里没有定义的配置项，会使用 session_start 时的定义。如果 session_start 时也没有定义，则使用缺省值
 *
 * @note
 * 配置项 adjustImage 决定检测到倾斜角后是否对图像进行倾斜校正，若adjustImage=no则只返回倾斜角度不改动图像数据；@n
 * 若adjustImage=yes，则进行校正并根据倾斜角对图像数据进行校正。
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_deskew(
		_MUST_ _IN_ int nSessionId,
		_OPT_ _IN_ const char * pszConfig,	
		_MUST_ _OUT_ OCR_DESKEW_RESULT * psOcrDeskewResult
		);



/**  
 * @brief	图片美化降噪（暂不支持）
 * @param	nSessionId		会话ID
 * @param	pszConfig		保留配置
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_UNSUPPORT</td><td>暂不支持</td></tr>
 *	</table>
 *
 */ 

HCI_ERR_CODE HCIAPI hci_ocr_denoise(_MUST_ _IN_ int nSessionId,	_OPT_ _IN_ const char * pszConfig);


/**  
 * @brief	释放倾斜校正结果内存
 * @param	psOcrDeskewResult	需要释放的倾斜校正结果的内存指针
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>输入参数不合法</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_free_deskew_result(
		_MUST_ _IN_ OCR_DESKEW_RESULT * psOcrDeskewResult
		);

/**  
 * @brief	进行版面分析
 * @param	nSessionId		会话ID
 * @param	pszConfig		参数配置，ASCII编码，以'\0'结束，可为NULL
 * @param	psOcrLayoutResult	版面分析结果的指针，使用完毕后，需使用 hci_ocr_free_layout_result() 函数进行释放
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>配置参数有误，如设定值非法或格式错误等</td></tr>
 *		<tr><td>@ref HCI_ERR_DATA_SIZE_TOO_LARGE</td><td>传入的数据超过可处理的上限，云端识别数据发往云端时判断[0,5M]</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>能力key未找到</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>分配内存失败</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>未找到有效的云服务地址</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>连接服务器失败，服务器无响应</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>服务器访问超时</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>服务器返回的数据格式不正确</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_RESPONSE_FAILED</td><td>服务器返回版面分析失败</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>传入的配置在总参数列表中，但此处不支持</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>本地引擎，版面分析过程失败，或者云端识别图像旋转、转换、二值化和压缩失败</td></tr>
 *	</table>
 *
 * @par 配置串定义：
 * 配置串是由"字段=值"的形式给出的一个字符串，多个字段之间以','隔开。字段名不分大小写。
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>字段</b></td>
 *			<td><b>取值或示例</b></td>
 *			<td><b>缺省值</b></td>
 *			<td><b>含义</b></td>
 *			<td><b>详细说明</b></td>
 *		</tr>
 *		<tr>
 *			<td>imageType</td>
 *			<td>normal, screen</td>
 *			<td>normal</td>
 *			<td>图像类型</td>
 *			<td>normal: 普通图像<br/>
 *              screen: 截屏图像
 *			</td>
 *		</tr>
 *		<tr>
 *			<td>binarize</td>
 *			<td>adaptive, global</td>
 *			<td>adaptive</td>
 *			<td>二值化方法，对灰度或者彩色图像有效</td>
 *			<td>global: 全局二值化算法，主要针对通过扫描、截屏等获取的图像<br/>
 *              adaptive: 局部自适应二值化算法，主要针对通过拍照、摄像头等获取的受光照影响较大图像
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>localProcess</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>是否先做本地预处理，云端中文OCR支持该配置@n
 *				现在主要是指二值化</td>
 *			<td>yes: 进行预处理<br/>
 *              no: 不进行预处理
 *			</td>
 *		</tr> 
 *	</table>
 * @n@n
 * 这里没有定义的配置项，会使用 session_start 时的定义。如果 session_start 时也没有定义，则使用缺省值
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_layout_analysis(
		_MUST_ _IN_ int nSessionId,
		_OPT_ _IN_ const char * pszConfig,	
		_MUST_ _OUT_ OCR_LAYOUT_RESULT * psOcrLayoutResult
		);

/**  
 * @brief	释放版面分析结果的内存
 * @param	psOcrLayoutResult	需要释放的版面分析结果的指针
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>输入参数不合法</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_free_layout_result(
		_MUST_ _IN_ OCR_LAYOUT_RESULT * psOcrLayoutResult
		);

/**  
 * @brief	进行OCR识别
 * @param	nSessionId			会话ID
 * @param	pszConfig			识别参数配置串，可为NULL或者'\0'
 * @param	pRegions			识别区域列表，
 * @param	nRegionCount		识别区域的数目，
 * @param	psOcrRecogResult	识别结果的数据结构指针，使用完毕后，需使用 hci_ocr_free_recog_result() 函数进行释放
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_DATA_SIZE_TOO_LARGE</td><td>传入的数据超过可处理的上限，云端识别数据发往云端时判断[0,5M]</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>配置项非法</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>能力key未找到</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>分配内存失败</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>未找到有效的云服务地址</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>连接服务器失败，服务器无响应</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>服务器访问超时</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>服务器返回的数据格式不正确</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_RESPONSE_FAILED</td><td>服务器返回倾斜校正失败</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_UNSUPPORT</td><td>传入的配置在总参数列表中，但此处不支持</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>本地引擎，识别过程失败，或者云端识别图像旋转、转换、二值化和压缩失败</td></tr>
 *	</table>
 * 
 * @par 配置串定义：
 * 配置串是由"字段=值"的形式给出的一个字符串，多个字段之间以','隔开。字段名不分大小写。并不是每个能力都支持@n@n
 * 列表中的所有配置项，各能力支持的配置项请参考@ref hci_ocr_page
 * @n@n
 *	<table>
 *		<tr>
 *			<td><b>模块</b></td>
 *			<td><b>字段</b></td>
 *			<td><b>取值或示例</b></td>
 *			<td><b>缺省值</b></td>
 *			<td><b>含义</b></td>
 *			<td><b>详细说明</b></td>
 *		</tr> 
 *		<tr>
 *			<td rowspan="2">通用配置</td>
 *			<td>cutEdge</td>
 *			<td>yes, no </td>
 *			<td>no</td>
 *			<td>原始图片需要裁边</td>
 *			<td>yes,需要裁边,对于拍照照片适用，原始图像为未裁边图像<br/>
 *				no, 不需要裁边，非拍照图片使用，原始图像为裁边图像<br/>
 *			</td>
 *		</tr>
 *		<tr>
 *			<td>detailResult</td>
 *			<td>no, yes</td>
 *			<td>no</td>
 *			<td>是否返回详细结果</td>
 *			<td>缺省只返回识别结果的文本串，如果设为yes，则会返回详细的节点信息 @ref OCR_RECOG_NODE </td>
 *		</tr>
 *		<tr>
 *			<td rowspan="7">云端中文和本地文本识别</td>
 *			<td>recogRange</td>
 *			<td>gb, letter</td>
 *			<td>all</td>
 *			<td>识别范围</td>
 *			<td>number: 数字 0 - 9, 10个<br/>
 *				uppercase: 大写英文字母, A - Z, 26个<br/>
 *				lowercase: 小写英文字母, a - z, 26个<br/>
 *				letter: 大小写字母, 52个<br/>
 *				alnum: 大小写字母和数字, 62个<br/>
 *				gb: 中文简体汉字<br/>
 *				big5: 中文繁体汉字
 *				all: 所有字符集的合集,英文能力时不包含gb和big5<br/>
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>autoDeskew</td>
 *			<td>no, yes</td>
 *			<td>no</td>
 *			<td>是否识别前自动倾斜校正，支持，没有设置识别区域列表的情况下有效</td>
 *			<td></td>
 *		</tr>
 *		<tr>
 *			<td>autoLayout</td>
 *			<td>no, yes</td>
 *			<td>no</td>
 *			<td>是否识别前进行自动版面分析，没有设置识别区域列表的情况下有效</td>
 *			<td></td>
 *		</tr>
 *		<tr>
 *			<td>fullHalf</td>
 *			<td>full, half</td>
 *			<td>half</td>
 *			<td>设置中文识别模式下输出英文字母、数字是全角还是半角</td>
 *			<td>full: 全角<br/>half: 半角</td>
 *		</tr>
 *		<tr>
 *			<td>dispCode</td>
 *			<td>nochange, tosimplified</td>
 *			<td>nochange</td>
 *			<td>设置输出结果简繁体转换，云端中文和本地OCR支持该配置</td>
 *			<td>nochange: 简繁体不做变化<br/>tosimplified: 繁体转换为简体</td>
 *		</tr> 
 *		<tr>
 *			<td>imageType</td>
 *			<td>normal, screen</td>
 *			<td>normal</td>
 *			<td>图像类型</td>
 *			<td>normal: 普通图像<br/>
 *              screen: 截屏图像
 *			</td>
 *		</tr>  
 *		<tr>
 *			<td>binarize</td>
 *			<td>adaptive, global</td>
 *			<td>adaptive</td>
 *			<td>二值化方法,对灰度或者彩色图像有效</td>
 *			<td>global: 全局二值化算法，主要针对通过扫描、截屏等获取的图像<br/>
 *              adaptive: 局部自适应二值化算法，主要针对通过拍照、摄像头等获取的受光照影响较大图像
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>云端中文</td>
 *			<td>localProcess</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>是否先做本地预处理(云端多语种必须设置为no)@n
 *				现在主要是指二值化</td>
 *			<td>yes: 进行预处理<br/>
 *              no: 不进行预处理
 *			</td>
 *		</tr> 
 *		<tr>
 *			<td>云端多语种OCR</td>
 *			<td>export</td>
 *			<td> none,xml,pdf,docx,rtf,xlsx,text,html,pptx</td>
 *			<td>none</td>
 *			<td>导出文件格式设置</td>
 *			<td>none:不以导出文件方式返回结果<br/>
				pdf:pdf格式文件<br/>
				docx:docx格式文件<br/>
				rtf:rtf格式文件<br/>
				xlsx:xlsx格式文件<br/>
				text:text格式文件<br/>
				html:html格式文件<br/>
				pptx:pptx格式文件<br/>
 *			</td>
 *		</tr>
 *		<tr>
 *			<td rowspan="4">模板识别</td>
 *			<td>templateId</td>
 *			<td>1</td>
 *			<td>无</td>
 *			<td>本地识别使用的模板ID</td>
 *			<td>本地必选<br/>
 *				本地识别时，本参数是由 hci_ocr_load_template()返回的ID；<br/>
 *				云端识别时，请使用domain参数指定欲使用的识别模板</td>
 *		</tr>
 *		<tr>
 *			<td>templateIndex</td>
 *			<td>0到对应模板文件中的最大模板索引号</td>
 *			<td>无</td>
 *			<td>只针对模板识别生效，模板文件中的模板索引号</td>
 *			<td>必选，模板文件中可能有多个模板，比如身份证、票据等，本参数指定模板在模板文件中的索引号</td>
 *		</tr> 
 *		<tr>
 *			<td>templatePageIndex</td>
 *			<td>0到对应模板中的最大页索引号</td>
 *			<td>无</td>
 *			<td>只针对模板识别生效，模板页索引号</td>
 *			<td>必选，模板中可能存在多个模板页，比如身份证的正面和反面，本参数指定模板页在模板中的索引号</td>
 *		</tr> 
 *		<tr>
 *			<td>domain</td>
 *			<td>dlcard，vlcard，idcard</td>
 *			<td>无</td>
 *			<td>指定云端模板识别欲使用的模板</td>
 *			<td>云端必选<br/>
 *				目前可以指定为dlcard（驾驶证）、vlcard（行驶证）、idcard（身份证）</td>
 *		</tr>
 *	</table>
 * @n@n
 * 这里没有定义的配置项，会使用 session_start 时的定义。如果 session_start 时也没有定义，则使用缺省值。@n
 * 使用本地模板识别时请配置cutEdge为no，引擎内部会自动作切边处理。@n
 * 云端模板识别使用原始图像数据进行识别，即SDK不作任何预处理。
 * @note
 * pRegions 如果为NULL或nRegionCount为0，则检查配置串中的 autoLayout, 如果是true，则后台进行自动版面分析，
 * 如果是false，则将整幅图像当做一个完整的横排正文区域进行识别 @n@n
 * pRegions 如果不为NULL且nRegionCount不为0，配置串不支持autoDeskew和autoLayout两个参数，因此总是不进行倾斜校正和版面分析, 
 * 而使用传入的pRegions作为识别区域
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
 * @brief	释放识别结果内存
 * @param	psOcrRecogResult	需要释放的识别结果的内存指针
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>输入参数不合法</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_free_recog_result(
		_MUST_ _IN_ OCR_RECOG_RESULT * psOcrRecogResult
		);

/**  
 * @brief	结束会话
 * @param	nSessionId		会话ID
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_ocr_session_stop(
		_MUST_ _IN_ int nSessionId
		);

/**  
 * @brief	灵云OCR能力 反初始化
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_ACTIVE_SESSION_EXIST</td><td>尚有未stop的Sesssion，无法结束</td></tr>
 *	</table>
 */
HCI_ERR_CODE HCIAPI hci_ocr_release();

/**  
 * @brief	通过内存缓冲设置本会话处理的图像
 * @param	nSessionId			会话ID
 * @param	pImageData			要设置的本地内存缓冲
 * @param	uiImageDataSize		要设置的本地内存缓冲的长度，字节为单位
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *	</table>
 */
HCI_ERR_CODE HCIAPI hci_ocr_set_image_buffer(
		_MUST_ _IN_ int nSessionId ,
		_MUST_ _IN_ unsigned char * pImageData, 
		_MUST_ _IN_ unsigned int uiImageDataSize
		);

/**  
 * @brief	通过文件设置本回话处理的图像
 * @param	nSessionId			会话ID
 * @param	pszImageFileName	要设置的本地图片文件
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *	</table>
 */

HCI_ERR_CODE HCIAPI hci_ocr_set_image_file(
		_MUST_ _IN_ int nSessionId ,
		_MUST_ _IN_ const char * pszImageFileName
		);


/**  
* @brief	获取会话中的图像数据结构，用户可以用来显示
* @param	nSessionId			会话ID
* @param	pImage				返回的图像结构，用户定义的OCR_IMAGE结构体对象的地址做实参(用于显示，不要修改返回图像的数据)
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
*		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
*		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>设置初始数据为图像缓冲时，本函数返回时转换为OCR_IMAGE结构失败</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_IMAGE_NOT_SET</td><td>本会话还未设置图像数据</td></tr>
*	</table>
*/
HCI_ERR_CODE HCIAPI hci_ocr_get_image( 
									  _MUST_ _IN_ int nSessionID,
									  _MUST_ _OUT_ OCR_IMAGE* pImage
									  );

/**  
* @brief	保存一个本地文件，文件格式由文件名后缀确定(目前支持bmp、jpg、png、tif)
* @param	pszImageFileName	保存到的本地图像文件名
* @param	pImage				要保存的源图像数据结构
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
*		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_SAVE_IMAGE</td><td>保存本地图像文件失败</td></tr>
*	</table>
* @note 
*		iOS平台不支持此操作，调用此函数会返回 HCI_ERR_UNSUPPORT。如果希望在iOS存储图像数据，
*		请使用 hci_ocr_get_uiImage() 获取UIImage，然后使用UIImage的存储文件功能进行存储。
*/

HCI_ERR_CODE HCIAPI hci_ocr_save_image_file( 
											_MUST_ _IN_ const char * pszImageFileName,
											_MUST_ _IN_ OCR_IMAGE * pImage
											);

/**  
* @brief	载入一个模板文件，通过参数列表返回其ID
* @param	pszTemplateFile		要载入的模板文件路径
* @param	pnTemplateID		用于返回载入成功获取的模板文件ID
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
*		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_LOAD_TEMPLATE_FAILED</td><td>载入模板失败，可能是文件不存在、文件不可读、文件非法或者解码失败</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_TEMPLATE_OVERLOAD</td><td>载入的模板数量超过上线，目前暂只能加载1个模板</td></tr>
*	</table>
* @note 
*		本地模板识别时使用，载入模板，获取其ID，在 hci_ocr_session_start()时用于创建会话。多个session可共用一个templateID。
*	多次载入同一个模板文件不会有错，被认为是独立的多个模板且得到多个ID，这些ID都需要对应卸载。

*/
HCI_ERR_CODE hci_ocr_load_template(const char *pszTemplateFile, int *pnTemplateID);




/**  
* @brief	卸载一个已经载入的模板文件
* @param	nTemplateID			要卸载的模板文件的ID，由 hci_ocr_load_template()得来
* @return
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_TEMPLATE_ID_INVALID</td><td>传入的模板ID不在加载的模板列表中</td></tr>
*		<tr><td>@ref HCI_ERR_OCR_TEMPLATE_USING</td><td>还有会话在使用要卸载的模板</td></tr>
*	</table>
* @note 
*		用户在卸载模板时，必须保证使用该模板的所有session都已经执行 hci_ocr_session_stop()操作，否则返回错误。
*/
HCI_ERR_CODE hci_ocr_unload_template(int nTemplateID);


#ifdef __OBJC__
/**  
 * @brief	装载一个UIimage图像，解码到图像数据结构并设置到指定的会话
 * @param	nSessionID			要设置图像结构的会话
 * @param	image				要载入的UIImage对象
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>UIiamge转换成ocrimage的时候失败</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>申请内存失败</td></tr>
 *	</table>
 * @note 
 *		仅iOS平台支持此函数
 */
HCI_ERR_CODE HCIAPI hci_ocr_set_image_by_uiImage( 
		_MUST_ _IN_ int nSessionID,
		_MUST_ _IN_ UIImage * image
		);


/**  
 * @brief	将OCR_IMAGE数据转换成UIImage数据
 * @param	nSessionId			会话ID
 * @param	image				转换后的UIImage对象（此对象是autorelease的）
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>操作成功</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_NOT_INIT</td><td>HCI OCR尚未初始化</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>传入的参数不合法</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>申请内存失败</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>OCR_IMAGE转换成UIImage的时候失败</td></tr>
 *		<tr><td>@ref HCI_ERR_UNSUPPORT</td><td>不支持传入的OCR_IMAGE格式</td></tr>
 *		<tr><td>@ref HCI_ERR_SESSION_INVALID</td><td>传入的Session非法</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_ENGINE_FAILED</td><td>设置初始数据为图像缓冲时，本函数返回时转换为OCR_IMAGE结构失败</td></tr>
 *		<tr><td>@ref HCI_ERR_OCR_IMAGE_NOT_SET</td><td>本会话还未设置图像数据</td></tr>
 *	</table>
 * @note 
 *		仅iOS平台支持此函数
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
