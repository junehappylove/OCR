/** 
 * @file    hci_sys.h 
 * @brief   HCI_SYS ����ͷ�ļ�  
 */  

#ifndef _hci_sys_header_
#define _hci_sys_header_

#ifdef __ANDROID__
#include <jni.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup HCI_SYS ����ϵͳAPI */
/* @{ */

/* parameter remark definition */
#ifndef _IN_
#define _IN_
#endif
#ifndef _OUT_
#define _OUT_
#endif
#ifndef _IN_OUT_
#define _IN_OUT_
#endif
#ifndef _MUST_
#define _MUST_
#endif
#ifndef _OPT_
#define _OPT_
#endif

#ifdef _WIN32
#define HCIAPI _stdcall
#else
#define HCIAPI
#endif

typedef long long int64;

/**
*HCI_ERR_CODE  ���ش����붨��
*/
typedef enum {
	HCI_ERR_UNKNOWN = -1,					///< -1: δ֪����ͨ���������

	HCI_ERR_NONE = 0,						///< 0: ��ȷ

	HCI_ERR_PARAM_INVALID,					///< 1: �����Ĵ����������
	HCI_ERR_OUT_OF_MEMORY,					///< 2: �����ڴ�ʧ��

	HCI_ERR_CONFIG_INVALID,					///< 3: ���ô���������
	HCI_ERR_CONFIG_CAPKEY_MISSING,			///< 4: ȱ�ٱ����capKey������
	HCI_ERR_CONFIG_CAPKEY_NOT_MATCH,		///< 5: CAPKEY�뵱ǰ���治ƥ��
	HCI_ERR_CONFIG_DATAPATH_MISSING,		///< 6: ȱ�ٱ����dataPath������
	HCI_ERR_CONFIG_UNSUPPORT,				///< 7: �����֧��

	HCI_ERR_SERVICE_CONNECT_FAILED,			///< 8: ���ӷ�����ʧ�ܣ�����������Ӧ
	HCI_ERR_SERVICE_TIMEOUT,				///< 9: ���������ʳ�ʱ
	HCI_ERR_SERVICE_DATA_INVALID,			///< 10: ���������ص����ݸ�ʽ����ȷ
	HCI_ERR_SERVICE_RESPONSE_FAILED,		///< 11: ���������ز���ʧ��

	HCI_ERR_CAPKEY_NOT_FOUND,				///< 12: û���ҵ�ָ��������

	HCI_ERR_NOT_LOCAL_CAPKEY,				///< 13: ���Ǳ���������KEY
	HCI_ERR_LOCAL_LIB_MISSING,				///< 14: ������������ȱʧ��Ҫ�Ŀ���Դ

	HCI_ERR_URL_MISSING,					///< 15: �Ҳ�����Ӧ����������ַ��������HCI���������ַ��������Դ���ַ�ȣ�
	
	HCI_ERR_SESSION_INVALID,				///< 16: ��Ч�ĻỰ
	HCI_ERR_TOO_MANY_SESSION,				///< 17: �����Ự����(Ŀǰÿ��HCI���������Ự��Ϊ256)
	HCI_ERR_ACTIVE_SESSION_EXIST,			///< 18: ���лỰû��ֹͣ
	HCI_ERR_START_LOG_FAILED,				///< 19: ������־����, ��������־���ò�������·�������ڻ���û��дȨ�޵����

	HCI_ERR_DATA_SIZE_TOO_LARGE,			///< 20: ����������������ɴ��������

	HCI_ERR_LOAD_CODEC_DLL,					///< 21: ����codec�����ʧ��
	HCI_ERR_UNSUPPORT,						///< 22: �ݲ�֧��
	HCI_ERR_LOAD_FUNCTION_FROM_DLL,			///< 23: ���ؿ�ʧ��
    HCI_ERR_TXACCOUNT_NOT_FOUND,            ///< 24: �����˺Ż�ȡʧ��

	//HCI_SYS 
	HCI_ERR_SYS_NOT_INIT = 100,				///< 100: HCI_SYSδ��ʼ��
	HCI_ERR_SYS_ALREADY_INIT,				///< 101: HCI_SYS��γ�ʼ��
	
	HCI_ERR_SYS_CONFIG_AUTHPATH_MISSING,	///< 102: ȱ�ٱ����authPath������
	HCI_ERR_SYS_CONFIG_CLOUDURL_MISSING,	///< 103: ȱ�ٱ����cloudUrl������
	HCI_ERR_SYS_CONFIG_USERID_MISSING,		///< 104: ȱ�ٱ����userId������
	HCI_ERR_SYS_CONFIG_PASSWORD_MISSING,	///< 105: ȱ�ٱ����password������

	HCI_ERR_SYS_CONFIG_PLATFORMID_MISSING,	///< 106: ȱ�ٱ����platformId������
	HCI_ERR_SYS_CONFIG_DEVELOPERID_MISSING,	///< 107: ȱ�ٱ����developerId������
	HCI_ERR_SYS_CONFIG_DEVELOPERKEY_MISSING,///< 108: ȱ�ٱ����developerKey������
	HCI_ERR_SYS_CONFIG_APPNO_MISSING,		///< 109: ȱ�ٱ����appNo������

	HCI_ERR_SYS_USERINFO_INVALID,			///< 110: ��д�û���Ϣ�ļ�����
	HCI_ERR_SYS_AUTHFILE_INVALID,			///< 111: ��ȡ��Ȩ�ļ�����

	HCI_ERR_SYS_CHECKAUTH_RESPONSE_FAILED,	///< 112: ���������ػ�ȡ�ƶ���Ȩʧ��
	HCI_ERR_SYS_REGISTER_RESPONSE_FAILED,	///< 113: ����������ע���û�ʧ�ܣ��ѷ����������汾3.0��
	HCI_ERR_SYS_USING,						///< 114: ��Ȼ��������ʹ�ã���δ����ʼ����
	HCI_ERR_SYS_CONFIG_APPKEY_MISSING,      ///< 115: ȱ�ٱ����appkey������

	/* HCI_ASR */
	HCI_ERR_ASR_NOT_INIT = 200,				///< 200: HCI_ASRû�г�ʼ��
	HCI_ERR_ASR_ALREADY_INIT,				///< 201: HCI_ASR��γ�ʼ��
	HCI_ERR_ASR_CONFIRM_NO_TASK,			///< 202: ʹ��confirm����û��ȷ������
	HCI_ERR_ASR_PARAM_CHECK_FLAG_INVALID,	///< 203: �����CheckFlag������Ѿ���⵽�˵���Ȼ�ٷ������ݣ�����δ�����˵���ͷ���flagΪ��CHECK_FLAG_END��CHECK_FLAG_PROGRESS��
	HCI_ERR_ASR_GRAMMAR_DATA_TOO_LARGE,		///< 204: �﷨����̫��
	HCI_ERR_ASR_ENGINE_NOT_INIT,			///< 205: ASR����������δ��ʼ��(�ѷ����������汾3.8)
	HCI_ERR_ASR_ENGINE_INIT_FAILED,			///< 206: ASR���������ʼ��ʧ��
	HCI_ERR_ASR_OPEN_GRAMMAR_FILE,			///< 207: ��ȡ�﷨�ļ�ʧ��
	HCI_ERR_ASR_LOAD_GRAMMAR_FAILED,		///< 208: �����﷨�ļ�ʧ��
	HCI_ERR_ASR_ENGINE_FAILED,				///< 209: ASR��������ʶ��ʧ��
	HCI_ERR_ASR_GRAMMAR_ID_INVALID,			///< 210: �﷨ID��Ч
	HCI_ERR_ASR_REALTIME_WAITING,			///< 211: ʵʱʶ��ʱδ��⵽��Ƶĩ�ˣ������ȴ�����
	HCI_ERR_ASR_GRAMMAR_OVERLOAD,			///< 212: �����﷨�����Ѵ�����
	HCI_ERR_ASR_GRAMMAR_USING,				///< 213: ���﷨����ʹ����
	HCI_ERR_ASR_REALTIME_END,				///< 214: ʵʱʶ��ʱ��⵽ĩ�ˣ����߻�����������Ҫʹ��NULL��ȡ���
	HCI_ERR_ASR_UPLOAD_NO_DATA,				///< 215: �ϴ���������ʱ���������ϴ������� 
	
	/* HCI_HWR */
	HCI_ERR_HWR_NOT_INIT = 300,				///< 300: HCI_HWRû�г�ʼ��
	HCI_ERR_HWR_ALREADY_INIT,				///< 301: HCI_HWR��γ�ʼ��
	HCI_ERR_HWR_CONFIRM_NO_TASK,			///< 302: ʹ��confirm����û��ȷ������
	HCI_ERR_HWR_ENGINE_INIT_FAILED,			///< 303: HWR���������ʼ��ʧ��
	HCI_ERR_HWR_ENGINE_FAILED,				///< 304: HWR�������������ʶ�𡢻�ȡƴ������ȡ��������ʧ��
	HCI_ERR_HWR_UPLOAD_NO_DATA,				///< 305: û�п������ϴ�������
	HCI_ERR_HWR_ENGINE_SESSION_START_FAILED,///< 306: HWR�������濪���Ựʧ��
	HCI_ERR_HWR_ENGINE_NOT_INIT,			///< 307: SDK��ʼ��ʱδ���뱾������ȴ�ڴ����Ựʱʹ���˱�������(�ѷ����������汾3.8)
	HCI_ERR_HWR_CONFIG_SUBLANG_MISSING,		///< 308: �����������������ֵ�ʱ��δ����sublang
	HCI_ERR_HWR_TOO_MANY_DOMAIN,			///< 309: ������������Ŀ������4��

	
	/* HCI_OCR */
	HCI_ERR_OCR_NOT_INIT = 400,				///< 400: HCI_OCRû�г�ʼ��
	HCI_ERR_OCR_ALREADY_INIT,				///< 401: HCI_OCR��γ�ʼ��
	HCI_ERR_OCR_ENGINE_INIT_FAILED,			///< 402: OCR���������ʼ��ʧ��
	HCI_ERR_OCR_ENGINE_FAILED,				///< 403: OCR���������������бУ�������������ʶ��Ԥ����ѹ����ʧ��
	HCI_ERR_OCR_ENGINE_NOT_INIT,			///< 404: SDK��ʼ��ʱδ���뱾������ȴ�ڴ����Ựʱʹ���˱�������(�ѷ����������汾3.8)
	HCI_ERR_OCR_LOAD_IMAGE,					///< 405: ���뱾���ļ����߱���ͼƬ����ʧ��
	HCI_ERR_OCR_SAVE_IMAGE,					///< 406: ����OCR_IMAGE�������ļ�ʧ��
	HCI_ERR_OCR_IMAGE_NOT_SET,				///< 407: δ����Ҫ�����ͼƬ�ͽ�������бУ�������������ʶ��Ȳ���

	HCI_ERR_OCR_LOAD_TEMPLATE_FAILED,		///< 408: ����ģ���ļ�ʧ��
	HCI_ERR_OCR_TEMPLATE_OVERLOAD,		    ///< 409: ����ģ�������Ѵ����ޣ�1024��
	HCI_ERR_OCR_TEMPLATE_ID_INVALID,		///< 410: �����ģ��ID���ڼ��ص�ģ���б���
	HCI_ERR_OCR_TEMPLATE_USING,			    ///< 411: ���лỰ��ʹ��Ҫж�ص�ģ��
	HCI_ERR_OCR_DETECT_CORNER_FAILED,		///< 412: OCR��ȡ�б���Ϣʧ��
	HCI_ERR_OCR_NORMALIZE_FAILED,			///< 413: OCR�б�ʧ��
	HCI_ERR_OCR_RECOGNIZABLE_FAILED,		///< 414: OCRʶ���жϷ���ʧ��
	HCI_ERR_OCR_IMAGE_NOT_CLEAR,			///< 415: OCRʶ��ͼƬ������


	/* HCI_TTS */
	HCI_ERR_TTS_NOT_INIT = 500,				///< 500: HCI_TTSû�г�ʼ��
	HCI_ERR_TTS_ALREADY_INIT,				///< 501: HCI_TTS��γ�ʼ��
	HCI_ERR_TTS_SESSION_BUSY,				///< 502: TTS�Ự��æ�������ںϳɻص��������ֵ����˺ϳ�
	HCI_ERR_TTS_ENGINE_SESSION_START_FAILED,///< 503: TTS�������濪���Ựʧ��
	HCI_ERR_TTS_ENGINE_FAILED,				///< 504: TTS��������ϳ�ʧ��
    HCI_ERR_TTS_ENGINE_INIT_FAILED,	        ///< 505: TTS(NU)���������ʼ��ʧ��
    HCI_ERR_TTS_ENGINE_NOT_INIT,			///< 506: TTS(NU)����������δ��ʼ��

	/* HCI_MT */
	HCI_ERR_MT_NOT_INIT = 600,				///< 600: HCI_MTû�г�ʼ��
	HCI_ERR_MT_ALREADY_INIT,				///< 601: HCI_MT��γ�ʼ��

	/*HCI_NLU*/
	HCI_ERR_NLU_NOT_INIT = 700,				///< 700: HCI_NLUû�г�ʼ��
	HCI_ERR_NLU_ALREADY_INIT,				///< 701: HCI_NLU��γ�ʼ��
	HCI_ERR_NLU_ENGINE_SESSION_START_FAILED,///< 702: NLU�������濪���Ựʧ��
	HCI_ERR_NLU_ENGINE_FAILED,				///< 703: NLU��������ʶ��ʧ��

	/*HCI_KB*/
	HCI_ERR_KB_NOT_INIT = 800,				///< 800: HCI_KBû�г�ʼ��
	HCI_ERR_KB_ALREADY_INIT,				///< 801: HCI_KB��γ�ʼ��
	HCI_ERR_KB_ENGINE_SESSION_START_FAILED,	///< 802: KB�������濪���Ựʧ��
	HCI_ERR_KB_ENGINE_FAILED,				///< 803: KB��������ʶ��ʧ��
	HCI_ERR_KB_SYLLABLE_INVALID,			///< 804: �ݴ������޷��ж�����
	HCI_ERR_KB_UDB_WORD_EXIST,				///< 805: �Ѿ����û��ʿ��д���
	HCI_ERR_KB_CONFIRM_NO_TASK,				///< 806: ʹ��confirm����û��ȷ������

	/*HCI_VPR*/
	HCI_ERR_VPR_NOT_INIT = 900,				///< 900: HCI_VPRû�г�ʼ��
	HCI_ERR_VPR_ALREADY_INIT,				///< 901: HCI_VPR��γ�ʼ��
	HCI_ERR_VPR_ENGINE_INIT_FAILED,			///< 902: VPR���������ʼ��ʧ��
	HCI_ERR_VPR_ENGINE_FAILED,				///< 903: VPR�������洦��ʧ��
	HCI_ERR_VPR_USERID_NOT_EXIST,			///< 904: VPR�û�������
	HCI_ERR_VPR_ENGINE_SESSION_START_FAILED,///< 905: VPR�������濪���Ựʧ��
	HCI_ERR_VPR_CONFIG_USER_ID_MISSING,		///< 906: VPRȱ��userId������

	/*HCI_FPR*/
	HCI_ERR_FPR_NOT_INIT = 1000,			///< 1000: HCI_FPRû�г�ʼ��
	HCI_ERR_FPR_ALREADY_INIT,				///< 1001: HCI_FPR��γ�ʼ��
	HCI_ERR_FPR_ENGINE_INIT_FAILED,			///< 1002: FPR���������ʼ��ʧ��
	HCI_ERR_FPR_ENGINE_FAILED,				///< 1003: FPR�������洦��ʧ��
	HCI_ERR_FPR_USERID_NOT_EXIST,			///< 1004: FPR�û�������
	HCI_ERR_FPR_ENGINE_SESSION_START_FAILED,///< 1005: FPR�������濪���Ựʧ��
	HCI_ERR_FPR_CONFIG_USER_ID_MISSING,		///< 1006: FPRȱ��userId������
	HCI_ERR_FPR_CONFIG_DPI_MISSING,			///< 1007: FPRȱ��dpi������
	HCI_ERR_FPR_CONFIG_BGCOLOR_MISSING,		///< 1008: FPRȱ��bgColor������
	HCI_ERR_FPR_CONFIG_WIDTH_MISSING,		///< 1009: FPRȱ��width������
	HCI_ERR_FPR_CONFIG_HEIGHT_MISSING,		///< 1010: FPRȱ��height������
	
} HCI_ERR_CODE;

//////////////////////////////////////////////////////////////////////////
// ���ݽṹ����

/** 
 * @brief	�����ļ���
 */
typedef struct _tag_CHECK_UPDATE_RESULT_ITEM
{
	/// �ļ�����
	char * pszFileName;
	
	/// ���ص�ַ
	char * pszUrl;

	/// @brief	�Ƿ�ȱ�ٵı������ص��ļ�
	///			TRUE��ȱ�ٵ��ļ����������أ��������޷�ʹ��
	///			FALSE: �����������ļ�
	bool bMissingFile;
} CHECK_UPDATE_RESULT_ITEM;

/**
 * @brief	�����º����ķ��ؽ��
 */
typedef struct _tag_CHECK_UPDATE_RESULT
{
	/// �����ļ����б�
	CHECK_UPDATE_RESULT_ITEM * pItemList;

	/// �����ļ������Ŀ
	unsigned int uiItemCount;
} CHECK_UPDATE_RESULT;

/**
 * @brief	������������
 */
typedef struct _tag_CAPABILITY_PROPERTY_ITEM
{
	/// ���Լ�
	char * pszName;

	/// ����ֵ
	char * pszValue;
} CAPABILITY_PROPERTY_ITEM;

/**
 * @brief	��������Ŀ
 */
typedef struct _tag_CAPABILITY_ITEM
{
	/// ������Ψһ��ʶ
	char* pszCapKey;

	/// �Ƿ��ƶ�����
	bool bCloud;

	/// ����汾����
	char * pszVersion;

	/// ������Ŀ�б�
	CAPABILITY_PROPERTY_ITEM * pPropertyList;

	/// ������Ŀ�б����Ŀ
	unsigned int uiPropertyCount;
} CAPABILITY_ITEM;

/**
 * @brief	�����б�
 */
typedef struct _tag_CAPABILITY_LIST 
{
	/// ������Ŀ�б�
	CAPABILITY_ITEM *	pItemList;

	/// ��������
	unsigned int	uiItemCount;
} CAPABILITY_LIST;

//////////////////////////////////////////////////////////////////////////
// �ӿ�API��������

/**
 * @brief	����ϵͳSDK ������ʼ�� 
 * @param	pszConfig	��ʼ�����ô���ASCII�ַ�������'\0'����
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_ALREADY_INIT</td><td>HCI SYS �Ѿ���ʼ������</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CONFIG_INVALID</td><td>���ò����������趨ֵ�Ƿ������ʽ�����</td></tr>
 *		<tr><td>@ref HCI_ERR_START_LOG_FAILED</td><td>������־����, ��������־���ò�������·�������ڻ���û��дȨ�޵����</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�ڴ�������</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_CONFIG_AUTHPATH_MISSING</td><td>ȱ�ٱ����authPath������</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_CONFIG_CLOUDURL_MISSING</td><td>ȱ�ٱ����cloudUrl������</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_CONFIG_USERID_MISSING</td><td>ȱ�ٱ����userId������</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_CONFIG_PASSWORD_MISSING</td><td>ȱ�ٱ����password������</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_CONFIG_DEVELOPERKEY_MISSING</td><td>ȱ�ٱ����developerKey������</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_CONFIG_APPKEY_MISSING</td><td>ȱ�ٱ����appKey������</td></tr>
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
 *			<td>authPath</td>
 *			<td>opt/myapp/info/</td>
 *			<td>��</td>
 *			<td>��Ȩ�ļ�����·��</td>
 *			<td>��������Ŀ¼�½�����<b>��Ȩ�ļ�</b>����Ȩ�ļ��ĸ�����������ע�⡣<br/>
 *				SDK����Ҫ�ڴ�Ŀ¼��дһЩ�����ļ�����˴�Ŀ¼�����пɶ�дȨ��, 
 *              ��ǿ�ҽ��鲻����һ�����Ƴ��Ľ����ϣ�����SD���ϣ���ֹSD�����γ���
 *			</td>
 *		</tr>
 *		<tr>
 *			<td>developerKey</td>
 *			<td>�ַ���</td>
 *			<td>��</td>
 *			<td>��������Կ</td>
 *			<td>�������ɽ�ͨ�����ṩ��</td>
 *		</tr>
 *		<tr>
 *			<td>appKey</td>
 *			<td>�ַ���</td>
 *			<td>��</td>
 *			<td>Ӧ�����</td>
 *			<td>�������ɽ�ͨ�����ṩ��</td>
 *		</tr>
 *		<tr>
 *			<td>cloudUrl</td>
 *			<td>http://api.hcicloud.com:9999</td>
 *			<td>��</td>
 *			<td>�����Ʒ���Ľӿڵ�ַ</td>
 *			<td>����������ֻʹ�ñ�����������SDK��Ȼ����Ҫ�˵�ַ�Խ����ƶ���Ȩ���ϴ����ݵȹ���</td>
 *		</tr>
 *		<tr>
 *			<td>autoCloudAuth</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>�Ƿ��Զ�����ƶ���Ȩ</td>
 *			<td>��������ע��</td>
 *		</tr>
 *		<tr>
 *			<td>appChannel</td>
 *			<td>1</td>
 *			<td>������</td>
 *			<td>Ӧ������</td>
 *			<td>������ͨ��ָ����ͬ��ֵ�����벻ͬ�İ汾�����������ƿ������������Ի�ȡ��ͬ�������ذ�װʹ�õ�һ��ͳ�ơ��ο�@subpage app_channel</td>
 *		</tr>
 *		<tr>
 *			<td>logFilePath</td>
 *			<td>/var/log/</td>
 *			<td>��</td>
 *			<td>��־��·��</td>
 *			<td>��ѡ�������������Ϊ����������־����־����Ϊhci.log</td>
 *		</tr>
 *		<tr>
 *			<td>logFileSize</td>
 *			<td>1 - 1048576 (1024*1024)</td>
 *			<td>32</td>
 *			<td>��־��С</td>
 *			<td>Ĭ��һ����־�ļ�д��󣬵�λΪK</td>
 *		</tr>
 *		<tr>
 *			<td>logFileCount</td>
 *			<td>0 - 1024</td>
 *			<td>5</td>
 *			<td>��־��Ŀ</td>
 *			<td>Ĭ�ϱ������ٸ����ݵ���־�ļ��������򸲸���ɵ���־��Ϊ0��ʾ��ǰ��־���������κα���</td>
 *		</tr>
 * 		<tr>
 *			<td>logLevel</td>
 *			<td>0��1��2��3��4��5</td>
 *			<td>0</td>
 *			<td>��־�ȼ�</td>
 *			<td>0=�ޣ�1=����2=���棬3=��Ϣ��4=ϸ�ڣ�5=���ԣ�SDK�����С�ڵ���logLevel����־��Ϣ</td>
 *		</tr>
 * 		<tr>
 *			<td>netProxy</td>
 *			<td>192.168.10.2</td>
 *			<td>��</td>
 *			<td>HTTP���������IP��ַ</td>
 *			<td>��ѡ</td>
 *		</tr>
 * 		<tr>
 *			<td>netProxyPort</td>
 *			<td>1080</td>
 *			<td>1080</td>
 *			<td>HTTP����������˿�</td>
 *			<td>��ѡ,���netPorxyδ���������Ҳ��������</td>
 *		</tr>
 * 		<tr>
 *			<td>netProxyUser</td>
 *			<td>user</td>
 *			<td>��</td>
 *			<td>HTTP��������������û���</td>
 *			<td>��ѡ,���netPorxyδ���������Ҳ��������</td>
 *		</tr>
 * 		<tr>
 *			<td>netProxyPwd</td>
 *			<td>pwd</td>
 *			<td>��</td>
 *			<td>HTTP�����������������</td>
 *			<td>��ѡ,���netPorxyδ���������Ҳ��������</td>
 *		</tr>
 *		<tr>
 *			<td>autoUpload</td>
 *			<td>yes, no</td>
 *			<td>yes</td>
 *			<td>�Ƿ��Զ��ϴ�������������¼����������</td>
 *			<td>�������������¼�û�������������ݣ��������Ϊyes��������ռ���һ��������ʱ�����Զ��ϴ�������
 *				���Ϊno���򲻻������ϴ������û������ֹ����� hci_upload_user_history() �����ϴ���</td>
 *		</tr>
 *	</table>
 *
 * @par ����ʾ����
 * @code
 * authPath=/data/data/test.company.com/file,cloudUrl=http://api.hcicloud.com:9999,autocloudauth=true,
 * developerKey=abcdefgh,appKey=123,
 * logFilePath=/var/log/,logLevel=5,logFileSize=3,logFileCount=9
 * @endcode
 *			
 * @note
 * <b>Ӧ����Ϣ</b>
 * @n@n
 * ��������developerKey, appKey���������ɽ�ͨ�����ṩ�����ڱ�ʶ��ͬ�Ŀ�����
 * ��Ӧ�ó��򣬸��費ͬ����Ȩ�ͷ���������ṩ�����ṩ���ݺͽ�ͨ���������Ĳ�һ�£��ᵼ�·���ʧ�ܡ�
 * @n@n
 * <b>��Ȩ</b>
 * @n@n
 * ��Ȩ������ȷ��Ӧ�ÿ���ʹ�õ��ƶ˻򱾵ص�HCI��������Ȩ�ļ��а������й���ʱ�����Ȩ������Ҳ���ܰ���
 * һ��������Ȩ���������������ʱ����δ������Ȩ�� �����ʹ�ù涨��������Ȩ������һ����˵��������Ȩ
 * ���������������ڵ���������ˣ�һ������ڹ���ʱ��֮ǰ�����ƶ���Ȩ�����Ը�����Ȩ��
 * @n@n
 * ��autoCloudAuth������Ϊyesʱ��ϵͳ�����Զ�������Ȩ���ĺ�̨�̣߳��Զ�������Ȩ�����Ϊno��
 * ����Ҫ�������ڵ�ǰ��Ȩ�ĵ���ʱ��֮ǰ���� hci_check_auth() �������ֶ�������Ȩ��
 * @n@n
 * ��ͨ���������ṩһ����ʼ��Ȩ�ļ������й涨�˳�ʼ����ʹ�õ���Ȩ��ֻ�ṩ���������������ڴ���Ȩ����
 * ʱ��֮ǰ��һֱ����ʹ��һЩ�����������Ӷ����Բ������κ�������ʡ������˴˳�ʼ��Ȩ�ĵ������ڣ�
 * ��Ȼ��Ҫ������Ȩ�ĸ��¡�
 * @n@n
 * ����authPath�����Androidϵͳ�½���ʹ��/data/data/����/file ·��������г�ʼ��Ȩ�ļ���������
 * ���ʱ���Խ���Ȩ�ļ�����һ��so����libsĿ¼��, �ڰ�װ�󣬴��ļ��ͻ������/data/data/����/libĿ¼�¡�
 * ��Ӧ�õ�һ������ʱ���𽫴��ļ�������/data/data/����/file���漴�ɡ����ڴ��ļ��ᱻ�����ĸ�����Ȩ��
 * ���£���ˣ����Ժ�Ҫ�ٴο������Է������µ���Ȩ�ļ���
 * @n@n
 */ 
#ifdef __ANDROID__
HCI_ERR_CODE HCIAPI hci_init( 
		_MUST_ _IN_ const char * pszConfig ,
		_MUST_ _IN_ JavaVM *vm, 
		_MUST_ _IN_ jobject context);
#else
HCI_ERR_CODE HCIAPI hci_init(
		_MUST_ _IN_ const char * pszConfig
		);
#endif
/**  
 * @brief	�ֶ���������Ȩ
 * @details	����������ʱ������ͨ�� hci_get_auth_expire_time()�õ�����Ȩ�Ĺ���ʱ�䣬
 *			ͨ�� hci_get_capability() �õ�����Ȩ�Ŀ�ʹ�õ�HCI������
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS ��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>���ӷ�����ʧ�ܣ�����������Ӧ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>���������ʳ�ʱ</td></tr>
 *		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>���������ص����ݸ�ʽ����ȷ</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_CHECKAUTH_RESPONSE_FAILED</td><td>��ȡ�ƶ���Ȩʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_REGISTER_RESPONSE_FAILED</td><td>ע���û�ʧ��</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�ڴ�������</td></tr>
 *	</table>
 */
HCI_ERR_CODE HCIAPI hci_check_auth();

/**  
 * @brief	�����Ȩ����ʱ��
 * @details	�õ���ʱ��� time() ���ظ���һ�£�ָ 1970-01-01 00:00:00 UTC ֮���������
 * @param	pnExpireTime		���������ָ�룬��Ź���ʱ��
 * @return
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS ��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_AUTHFILE_INVALID</td><td>��ȡ��Ȩ�ļ�����</td></tr>
 *	</table>
 */
HCI_ERR_CODE HCIAPI hci_get_auth_expire_time(
		_MUST_ _OUT_ int64 * pnExpireTime
		);

/**  
 * @brief	���ʹ��ĳ�����������Ƿ�ȱ��ĳЩ��Դ�ļ�������Դ�ļ��Ƿ��а汾����
 * @details	Ŀǰ�汾ֻ����Ƿ�ȱ����Դ�ļ�
 * @param	pszLocalCapKey			Ӧ��ϣ��ʹ�õı�������, ֻ�ܴ���һ��������
 * @param	pszDataPath				��鱾����Դ��·��, ��·��Ӧ���͸���������init������������ô��е�dataPathһ��
 * @param	psCheckUpdateResult		���������ָ�룬�����Ҫ���ػ���µ��ļ��б��������ʹ�� hci_free_update_result() �ͷ�
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS ��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>û���ҵ���������</td></tr>
 *		<tr><td>@ref HCI_ERR_NOT_LOCAL_CAPKEY</td><td>�����������Ǳ�������</td></tr>
 *		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>�޷��ҵ��û�ע�����������ַ</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td></td>�ڴ�������</tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_check_update(
		_MUST_ _IN_ const char * pszLocalCapKey,
		_MUST_ _IN_ const char * pszDataPath,
		_MUST_ _OUT_ CHECK_UPDATE_RESULT * psCheckUpdateResult
		);

/**  
 * @brief	�ͷŸ����ļ��б�ռ�õ��ڴ�
 * @param	psCheckUpdateResult		��Ҫ�ͷŵĸ����ļ��б�ṹ��ָ��
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_free_update_result(
		_MUST_ _IN_ CHECK_UPDATE_RESULT * psCheckUpdateResult
		);


/**
 * @brief	��ȡĳ��HCI���Ϳ��õ������б�
 * @param	pszHciType				����ѯ��HCI���������� hwr, tts, ocr, asr)������������';'������
 *									���ΪNULL���߿մ�����ʾȫ����ѯ
 * @param	psCapabilityList		���������ָ�룬��ŷ��ص������б�ʹ����Ϻ�Ӧ��ʹ�� hci_free_capability_list() �ͷ�
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS ��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td></td>�ڴ�������</tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_get_capability_list(
	    _OPT_ _IN_ const char * pszHciType,
		_MUST_ _OUT_ CAPABILITY_LIST	* psCapabilityList
		);

/**
 * @brief	�ͷ������б���ڴ�
 * @param	psCapabilityList		��Ҫ�ͷŵ������б��ڴ�
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_free_capability_list(
		_MUST_ _IN_ CAPABILITY_LIST	* psCapabilityList
		);

/**
 * @brief	��������Key��ȡ����������Ŀ
 * @param	pszKey					����ѯ������key��ֻ����һ��key������ΪNULL��մ�
 * @param	psCapabilityItem		���������ָ�룬��ŷ��ص���������Ŀ��ʹ����Ϻ�Ӧ��ʹ�� hci_free_capability_item() �ͷ�
 * @return	������
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS ��δ��ʼ��</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *		<tr><td>@ref HCI_ERR_CAPKEY_NOT_FOUND</td><td>û���ҵ���������</td></tr>
 *		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td></td>�ڴ�������</tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_get_capability(
	    _MUST_ _IN_ const char * pszKey,
		_MUST_ _OUT_ CAPABILITY_ITEM	* psCapabilityItem
		);

/**
 * @brief	�ͷ���������Ŀ���ڴ�
 * @param	psCapabilityItem		��Ҫ�ͷŵ���������Ŀ�ڴ�
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_PARAM_INVALID</td><td>����������Ϸ�</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_free_capability_item(
		_MUST_ _IN_ CAPABILITY_ITEM	* psCapabilityItem
		);

/**  
* @brief	�ϴ������û�¼������
* @return	������
* @return	
* @n
*	<table>
*		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
*		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS��δ��ʼ��</td></tr>
*		<tr><td>@ref HCI_ERR_URL_MISSING</td><td>δ�ҵ���Ч���Ʒ����ַ</td></tr>
*		<tr><td>@ref HCI_ERR_OUT_OF_MEMORY</td><td>�����ڴ�ʧ��</td></tr>
*		<tr><td>@ref HCI_ERR_SERVICE_CONNECT_FAILED</td><td>���ӷ�����ʧ�ܣ�����������Ӧ</td></tr>
*		<tr><td>@ref HCI_ERR_SERVICE_TIMEOUT</td><td>���������ʳ�ʱ</td></tr>
*		<tr><td>@ref HCI_ERR_SERVICE_DATA_INVALID</td><td>���������ص����ݸ�ʽ����ȷ</td></tr>
*		<tr><td>@ref HCI_ERR_SERVICE_RESPONSE_FAILED</td><td>����������ʶ��ʧ��</td></tr>
*	</table>
*/ 
HCI_ERR_CODE HCIAPI hci_upload_user_history();

/**  
 * @brief	����ϵͳSDK ����ʼ��
 * @return	������
 * @return	
 * @n
 *	<table>
 *		<tr><td>@ref HCI_ERR_NONE</td><td>�����ɹ�</td></tr>
 *		<tr><td>@ref HCI_ERR_SYS_NOT_INIT</td><td>HCI SYS ��δ��ʼ��</td></tr>
  *		<tr><td>@ref HCI_ERR_SYS_USING</td><td>��Ȼ��������ʹ�ã���δ����ʼ����</td></tr>
 *	</table>
 */ 
HCI_ERR_CODE HCIAPI hci_release();


/**  
 * @brief	��ȡ�������Ӧ����ϸ������Ϣ
 * @return	������Ϣ
 * @note
 * ������Ϣ��utf8�����Ӣ���ַ���,�������Ĵ�����û��������sdk�ж���
 * �᷵�ء�Unkown Error��
 */ 
const char * HCIAPI hci_get_error_info(HCI_ERR_CODE nErrorCode);

/**  
 * @brief	��ȡsdk�汾��Ϣ
 * @return	sdk�汾��
 * 
 */ 
const char * HCIAPI hci_get_sdk_version();



/* @} */
//////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
};
#endif

#endif
