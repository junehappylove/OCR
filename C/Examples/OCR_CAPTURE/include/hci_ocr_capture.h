/**
 * @file    hci_ocr_capture.h
 * @brief   HCI_ocr_capture SDK ͷ�ļ�
 */

#ifndef __HCI_OCR_CAPTURE_HEADER__
#define __HCI_OCR_CAPTURE_HEADER__

#include "hci_sys.h"
#include "hci_ocr.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /** @defgroup HCI_ocr_capture_API ����OCR������API */
    /* @{ */
    
    /**
     *CAPTURE_EVENT  ��չ�ӿڻص��¼�����
     */
    typedef enum
    {
        CAPTURE_EVENT_CAPTURE_BEGIN,		///< 0: ��������ʼ����ͼƬ����
        CAPTURE_EVENT_CAPTURE_FINISH,		///< 1: ɨ�����
        CAPTURE_EVENT_RECOGNIZE_BEGIN,		///< 2: ��ʼʶ��
        CAPTURE_EVENT_RECOGNIZE_MORE,		///< 3: ����ͼƬû��ʶ��
        CAPTURE_EVENT_RECOGNIZE_FINISH,		///< 4: ����ͼƬʶ�����
        CAPTURE_EVENT_RECOGNIZE_COMPLETE,	///< 5: ����ʶ�����
        CAPTURE_EVENT_ENGINE_ERROR,			///< 6: �������һ��ΪHCI_OCRģ�����
        CAPTURE_EVENT_DEVICE_ERROR,			///< 7: �豸����,Ŀǰwindows�ӿڶ�Ϊͬ��ʵ�֣���˶�ͨ������ֵ���أ����ٽ��и��ִ���ص�
        CAPTURE_EVENT_MALLOC_ERROR,			///< 8: ����ռ�ʧ��
        CAPTURE_EVENT_PREVIEW_BEGIN,        ///< 9: ��ʼԤ��ͼ��
        CAPTURE_EVENT_PREVIEW_DETECT_CORNER_POINTS_SUCCESS,  ///< 10: ��⵽Ԥ��ͼƬ֡���б���Ϣ
        CAPTURE_EVENT_PREVIEW_DETECT_CAN_BE_RECOGNIZE_SUCCESS,  ///< 11: ��⵽Ԥ��ͼƬ֡��ʶ��
        CAPTURE_EVENT_FOCUS_BEGIN,          ///< 12: ��ʼ�Խ�
        CAPTURE_EVENT_FOCUS_SUCCESS,        ///< 13: �Խ��ɹ�
        CAPTURE_EVENT_FOCUS_FAIL,           ///< 14: �Խ�ʧ��
        CAPTURE_EVENT_CAPTURE_DETECT_CORNER_POINTS_SUCCESS,    ///< 15: ���ջ�õ�ͼƬ����б���Ϣ�ɹ�  ��δʹ��
        CAPTURE_EVENT_CAPTURE_DETECT_CORNER_POINTS_FAIL,       ///< 16: ���ջ�õ�ͼƬ����б���Ϣʧ��  ��δʹ��
    } CAPTURE_EVENT;
    
    /**
     *CAPTURE_ERR_CODE  ��չ�ӿڴ����붨��
     */
    typedef enum
    {
        CAPTURE_ERR_UNKNOWN = -1,					///< -1: δ֪���󣬲������
        CAPTURE_ERR_NONE = 0,						///< 0: �ɹ�
        CAPTURE_ERR_ALREADY_INITED,                 ///< 1: �Ѿ���ʼ��
        CAPTURE_ERR_NOT_INITED,                     ///< 2: ��ͼ��û�г�ʼ��
        CAPTURE_ERR_ENGINE_INIT,					///< 3: �����ʼ��ʧ��
        CAPTURE_ERR_DEVICE_INIT,					///< 4: �豸��ʼ������
        CAPTURE_ERR_CAPTURE,                        ///< 5: ɨ�����
        CAPTURE_ERR_LOAD_TEMPLATE,                  ///< 6: ����ģ�����
        CAPTURE_ERR_UNLOAD_TEMPLATE,                ///< 7: ж��ģ�����
        CAPTURE_ERR_SESSION_START,                  ///< 8: ��������ʧ��
        CAPTURE_ERR_RECOGNIZE,                      ///< 9: ʶ��ʧ��
        CAPTURE_ERR_LOAD_IMAGE,                     ///< 10: ����ͼƬʧ��
        CAPTURE_ERR_UNSUPPORT,                      ///< 11: �ӿ��ݲ�֧��
        CAPTURE_ERR_RELEASE,                        ///< 12: �ͷ�ʧ��
        CAPTURE_ERR_BUSY,                           ///< 13: ״̬æµ���������ջ�ʶ��
        CAPTURE_ERR_CAPTUREONE,                     ///< 14: ����ʧ��
        CAPTURE_ERR_PARAM_INVALID,                  ///< 15: �����Ƿ�
        CAPTURE_ERR_OUT_OF_MEMORY,                  ///< 16: �ڴ治��
    } CAPTURE_ERR_CODE;
    
    //////////////////////////////////////////////////////////////////////////
    // �ص���������
    
    /**
     * @brief	 �¼��仯�ص�
     * @note
     * CAPTURE_EVENT_RECOGNIZE_FINISH �¼������ڸûص��д�������ֻ���� Callback_CaptureEventRecogFinish �д���
     * CAPTURE_EVENT_ENGINE_ERROR��CAPTURE_EVENT_DEVICE_ERROR �¼������ڸûص��д�����ֻ���� Callback_CaptureEventError �д���
     * @param	eCaptureEvent		�ص�ʱ��֪ͨ�¼�
     * @param	pUsrParam			�û��Զ������
     */
    typedef void (HCIAPI * Callback_CaptureEventStateChange)(
                                                             _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                                                             _OPT_ _IN_ void * pUsrParam );
    
    /**
     * @brief	ͼƬץȡ�ص�
     * @note
     * @param	pszImageData		ͼ������
     * @param	uiImageLen			ͼ�����ݳ���
     * @param	bHasConers			�Ƿ��б�,ɨ���Ƿ���true
     * @param	pCornersResult	    ͼ������,ɨ���Ƿ���NULL
     * @param	pUsrParam	        �û��Զ������
     */
    typedef void (HCIAPI * Callback_CaptureCapturing)(
                                                      _MUST_ _IN_ void * pszImageData,
                                                      _MUST_ _IN_ unsigned int uiImageLen,
                                                      _MUST_ _IN_ bool bHasConers,
                                                      _OPT_ _IN_ OCR_CORNERS_RESULT *pCornersResult,
                                                      _OPT_ _IN_ void * pUsrParam);
    
    /**
     * @brief	������ʶ����ɻص�
     * @note
     * ʶ��ɹ���ص�
     * @param	eCaptureEvent		ʶ������¼�
     * @param	pRecogResult		ʶ����
     * @param	pUsrParam			�û��Զ������
     */
    typedef void (HCIAPI * Callback_CaptureEventRecogFinish)(
                                                             _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                                                             _MUST_ _IN_ OCR_RECOG_RESULT *pRecogResult,
                                                             _OPT_ _IN_ void * pUsrParam );
    
    /**
     * @brief	OCR SDK����ص�
     * @note
     * �����¼� CAPTURE_EVENT_ENGINE_ERROR �� CAPTURE_EVENT_DEVICE_ERROR ʱ�ص�
     * @param	eCaptureEvent		�ص�ʱ��֪ͨ�¼�
     * @param	nErrorCode			������
     * @param	pUsrParam			�û��Զ������
     */
    typedef void (HCIAPI * Callback_CaptureEventError)(
                                                       _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                                                       _MUST_ _IN_ int eErrorCode,
                                                       _OPT_ _IN_ void * pUsrParam );
    
    /**
     * @brief	�������ص������Ľṹ��
     */
    typedef struct _CAPTURE_CALLBACK_PARAM {
        Callback_CaptureEventStateChange pfnStateChange;
        void * pvStateChangeUsrParam;
        Callback_CaptureCapturing pfnCapturing;
        void * pvCapturingUsrParam;
        Callback_CaptureEventRecogFinish pfnRecogFinish;
        void * pvRecogFinishUsrParam;
        Callback_CaptureEventError pfnError;
        void *pvErrorUsrParam;
    } CAPTURE_CALLBACK_PARAM;
    
    //////////////////////////////////////////////////////////////////////////
    // �ӿ�API��������
    
    /**
     * @brief	������SDK��ʼ��
     * @param	pszOCRSdkConfig		��������ʼ�����ô�
     * @param	psCallbackParam		�ص������ļ���
     * @par �������ô����壺
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
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_init(
                                                 _MUST_ _IN_ const char * pszOCRSdkConfig,
                                                 _MUST_ _IN_ CAPTURE_CALLBACK_PARAM *psCallbackParam);
    
    /**
     * @brief	OCR Capture����ģ���ļ�
     * @param    pszTemplateFile    ģ��·��
     * @param    pnTemplateId    ģ���ʶID
     */
    
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_load_template(
                                                          _MUST_ _IN_ const char * pszTemplateFile,
                                                          _MUST_ _OUT_ int * pnTemplateId
                                                          );
    
    /**
     * @brief	OCR Captureж��ģ���ļ�
     * @param    nTemplateId    ģ���ʶID
     */
    
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_unload_template(
                                                            _MUST_ _IN_  int nTemplateId
                                                            );
    
    /**
     * @brief	OCR Capture����ɨ�貢ʶ��
     * @param	pszConfig     ʶ��������
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
     *           <td rowspan="3">ʶ��ͨ��</td>
     *			<td>deviceName</td>
     *			<td>S225</td>
     *			<td>��</td>
     *			<td>�豸���ƣ�Windowsƽ̨��Ч��</td>
     *			<td>�����д��ʹ��Ĭ���豸��Windowsƽ̨��Ч��</td>
     *		</tr>
     *		<tr>
     *			<td>showSettingUi</td>
     *			<td>yes,no</td>
     *			<td>yes</td>
     *			<td>�Ƿ���start����ʾ���ý��棨Windowsƽ̨��Ч��</td>
     *			<td>�Ƿ���start����ʾ���ý��棨Windowsƽ̨��Ч��</td>
     *		</tr>
     *       <tr>
     *			<td>capKey</td>
     *			<td>ocr.cloud</td>
     *			<td>��</td>
     *			<td>OCRʶ������key</td>
     *			<td>�μ� @ref hci_ocr_page ��ÿ��sessionֻ�ܶ���һ�����������ҹ����в��ܸı䡣</td>
     *		</tr>
     *		<tr>
     *			<td rowspan="9">�ı�ʶ��</td>
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
     *			<td>�Ƿ��Զ���бУ�����Զ�ʶ��ʱ��֧��</td>
     *			<td></td>
     *		</tr>
     *		<tr>
     *			<td>autoLayout</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>�Ƿ�����Զ�����������Զ�ʶ��ʱ��֧��</td>
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
     *			<td>��������������ת��</td>
     *			<td>nochange: ���岻���仯<br/>tosimplified: ����ת��Ϊ����</td>
     *		</tr>
     *		<tr>
     *			<td>detailResult</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>�Ƿ񷵻���ϸ���</td>
     *			<td>ȱʡֻ����ʶ�������ı����������Ϊyes����᷵����ϸ�Ľڵ���Ϣ @ref OCR_RECOG_NODE </td>
     *		</tr>
     *		<tr>
     *			<td>binarize</td>
     *			<td>adaptive, global</td>
     *			<td>adaptive</td>
     *			<td>��ֵ�����������ԻҶȻ��߲�ɫͼ����Ч</td>
     *			<td>global: ȫ�ֶ�ֵ���㷨����Ҫ���ͨ��ɨ�衢�����Ȼ�ȡ��ͼ��<br/>
     *              adaptive: �ֲ�����Ӧ��ֵ���㷨����Ҫ���ͨ�����ա�����ͷ�Ȼ�ȡ���ܹ���Ӱ��ϴ�ͼ��
     *			</td>
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
     *			<td>localProcess</td>
     *			<td>yes, no</td>
     *			<td>yes</td>
     *			<td>�ƶ�����������ͼ��ʱ
     *				�Ƿ���������Ԥ����������Ҫ��ָ��ֵ��</td>
     *			<td>yes: ��ֵ��<br/>
     *              no: �����ж�ֵ��
     *			</td>
     *		</tr>
     *		<tr>
     *			<td rowspan="3">ģ��ʶ��</td>
     *			<td>templateID</td>
     *			<td>1</td>
     *			<td>��</td>
     *			<td>ģ���ļ���ID</td>
     *			<td>���غ��ƶ�ģ��ʶ��֧�֣���capKeyΪocr.local.template��ocr.cloud.templateʱ��ѡ������������ԡ�<br/>
     *				����ʶ��ʱ������������ hci_ocr_capture_load_template()���ص�ID��<br/>
     *				�ƶ�ʶ��ʱ������ѯ��ͨ����������ƿ����������˽�</td>
     *		</tr>
     *		<tr>
     *			<td>templateIndex</td>
     *			<td>0����Ӧģ���ļ��е����ģ��������</td>
     *			<td>��</td>
     *			<td>ֻ���ģ��ʶ����Ч��ģ���ļ��е�ģ��������</td>
     *			<td>ģ���ļ��п����ж��ģ�壬�������֤��Ʊ�ݵȣ�������ָ��ģ����ģ���ļ��е�������</td>
     *		</tr>
     *		<tr>
     *			<td>templatePageIndex</td>
     *			<td>0����Ӧģ���е����ҳ������</td>
     *			<td>��</td>
     *			<td>ֻ���ģ��ʶ����Ч��ģ��ҳ������</td>
     *			<td>ģ���п��ܴ��ڶ��ģ��ҳ���������֤������ͷ��棬������ָ��ģ��ҳ��ģ���е�������</td>
     *		</tr>
     *	</table>
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_start(
                                                  _MUST_ _IN_ const char * pszConfig);
    
    /**
     * @brief	��ȡԤ�����棬�ýӿ�Windows��֧��(iOSר��)
     * @param	preview		�û������UIView��Ԥ�����ݽ�չʾ�ڴ�view��
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_get_preview(void * preview);
    
    /**
     * @brief	ֹͣ�����ʶ��
     *
     *
     * ��ʶ����Ƭ��ģ��ʱ����ͼ��������ʶ���������������ͼ�����ݣ��ýӿ�Windows��֧��
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_stop_and_recog();
    
    /**
     * @brief	ֹͣ�������ʶ��
     *
     *
     * �ýӿ�Windows��֧��
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_stop();
    
    /**
     * @brief	OCR Capture   ֧��ʶ����ͼƬʶ��ͨ���ص����ش�����
     * @param	pszConfig     ʶ��������
     * @param	pImageData    ͼ�����ݣ�����ͨ��isFile�ֶ���ָ��ʹ���ļ�·������ͼ�������ڴ�
     * @param    uiImageLen    ����ͼ�����ݵĳ��ȣ�����ӿڴ������·������������Ч
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
     *			<td rowspan="3">ʶ��ͨ��</td>
     *			<td>capKey</td>
     *			<td>ocr.cloud</td>
     *			<td>��</td>
     *			<td>OCRʶ������key</td>
     *			<td>�μ� @ref hci_ocr_page ��ÿ��sessionֻ�ܶ���һ�����������ҹ����в��ܸı䡣</td>
     *		</tr>
     *		<tr>
     *			<td>cutEdge</td>
     *			<td>yes, no </td>
     *			<td>no</td>
     *			<td>ԭʼͼƬ��Ҫ�ñߣ�������ֻ�����ڱ���ʶ��</td>
     *			<td>yes,��Ҫ�ñ�,����������Ƭ���ã�ԭʼͼ��Ϊδ�ñ�ͼ��<br/>
     *				no, ����Ҫ�ñߣ�������ͼƬʹ�ã�ԭʼͼ��Ϊ�ñ�ͼ��<br/>
     *			</td>
     *		</tr>
     *		<tr>
     *			<td>isFile</td>
     *			<td>yes,no</td>
     *			<td>no</td>
     *			<td>�Ƿ��ļ�·������</td>
     *			<td>�Ƿ����ļ�·������</td>
     *		</tr> 
     *		<tr>
     *			<td rowspan="9">�ı�ʶ��</td>
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
     *			<td>�Ƿ��Զ���бУ�����Զ�ʶ��ʱ��֧��</td>
     *			<td></td>
     *		</tr>
     *		<tr>
     *			<td>autoLayout</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>�Ƿ�����Զ�����������Զ�ʶ��ʱ��֧��</td>
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
     *			<td>��������������ת��</td>
     *			<td>nochange: ���岻���仯<br/>tosimplified: ����ת��Ϊ����</td>
     *		</tr> 
     *		<tr>
     *			<td>detailResult</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>�Ƿ񷵻���ϸ���</td>
     *			<td>ȱʡֻ����ʶ�������ı����������Ϊyes����᷵����ϸ�Ľڵ���Ϣ @ref OCR_RECOG_NODE </td>
     *		</tr>
     *		<tr>
     *			<td>binarize</td>
     *			<td>adaptive, global</td>
     *			<td>adaptive</td>
     *			<td>��ֵ�����������ԻҶȻ��߲�ɫͼ����Ч</td>
     *			<td>global: ȫ�ֶ�ֵ���㷨����Ҫ���ͨ��ɨ�衢�����Ȼ�ȡ��ͼ��<br/>
     *              adaptive: �ֲ�����Ӧ��ֵ���㷨����Ҫ���ͨ�����ա�����ͷ�Ȼ�ȡ���ܹ���Ӱ��ϴ�ͼ��
     *			</td>
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
     *			<td>localProcess</td>
     *			<td>yes, no</td>
     *			<td>yes</td>
     *			<td>�ƶ�����������ͼ��ʱ
     *				�Ƿ���������Ԥ����������Ҫ��ָ��ֵ��</td>
     *			<td>yes: ��ֵ��<br/>
     *              no: �����ж�ֵ��
     *			</td>
     *		</tr> 
     *		<tr>
     *			<td rowspan="3">ģ��ʶ��</td>
     *			<td>templateID</td>
     *			<td>1</td>
     *			<td>��</td>
     *			<td>ģ���ļ���ID</td>
     *			<td>���غ��ƶ�ģ��ʶ��֧�֣���capKeyΪocr.local.template��ocr.cloud.templateʱ��ѡ������������ԡ�<br/>
     *				����ʶ��ʱ������������ hci_ocr_capture_load_template()���ص�ID��<br/>
     *				�ƶ�ʶ��ʱ������ѯ��ͨ����������ƿ����������˽�</td>
     *		</tr>
     *		<tr>
     *			<td>templateIndex</td>
     *			<td>0����Ӧģ���ļ��е����ģ��������</td>
     *			<td>��</td>
     *			<td>ֻ���ģ��ʶ����Ч��ģ���ļ��е�ģ��������</td>
     *			<td>ģ���ļ��п����ж��ģ�壬�������֤��Ʊ�ݵȣ�������ָ��ģ����ģ���ļ��е�������</td>
     *		</tr> 
     *		<tr>
     *			<td>templatePageIndex</td>
     *			<td>0����Ӧģ���е����ҳ������</td>
     *			<td>��</td>
     *			<td>ֻ���ģ��ʶ����Ч��ģ��ҳ������</td>
     *			<td>ģ���п��ܴ��ڶ��ģ��ҳ���������֤������ͷ��棬������ָ��ģ��ҳ��ģ���е�������</td>
     *		</tr> 
     *	</table>
     */ 
    
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_recog(
                                                  _MUST_ _IN_ const char * pszConfig,
                                                  _MUST_ _IN_ unsigned char * pImageData,
                                                  _MUST_ _IN_ unsigned int uiImageLen);
    
    /**  
     * @brief	OCR Capture����ʼ��
     * 
     *
     *
     * �ر�������ͷ�ocr�������Դ
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_release();
    
    /* @} */
    //////////////////////////////////////////////////////////////////////////
    
#ifdef __cplusplus
}
#endif

#endif
