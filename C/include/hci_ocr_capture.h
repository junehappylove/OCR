/**
 * @file    hci_ocr_capture.h
 * @brief   HCI_ocr_capture SDK 头文件
 */

#ifndef __HCI_OCR_CAPTURE_HEADER__
#define __HCI_OCR_CAPTURE_HEADER__

#include "hci_sys.h"
#include "hci_ocr.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
    /** @defgroup HCI_ocr_capture_API 灵云OCR拍照器API */
    /* @{ */
    
    /**
     *CAPTURE_EVENT  扩展接口回调事件定义
     */
    typedef enum
    {
        CAPTURE_EVENT_CAPTURE_BEGIN,		///< 0: 拍照器开始进行图片捕获
        CAPTURE_EVENT_CAPTURE_FINISH,		///< 1: 扫描完成
        CAPTURE_EVENT_RECOGNIZE_BEGIN,		///< 2: 开始识别
        CAPTURE_EVENT_RECOGNIZE_MORE,		///< 3: 还有图片没有识别
        CAPTURE_EVENT_RECOGNIZE_FINISH,		///< 4: 单张图片识别结束
        CAPTURE_EVENT_RECOGNIZE_COMPLETE,	///< 5: 所有识别结束
        CAPTURE_EVENT_ENGINE_ERROR,			///< 6: 引擎出错，一般为HCI_OCR模块出错
        CAPTURE_EVENT_DEVICE_ERROR,			///< 7: 设备出错,目前windows接口都为同步实现，因此都通过返回值返回，不再进行该种错误回调
        CAPTURE_EVENT_MALLOC_ERROR,			///< 8: 分配空间失败
        CAPTURE_EVENT_PREVIEW_BEGIN,        ///< 9: 开始预览图像
        CAPTURE_EVENT_PREVIEW_DETECT_CORNER_POINTS_SUCCESS,  ///< 10: 检测到预览图片帧的切边信息
        CAPTURE_EVENT_PREVIEW_DETECT_CAN_BE_RECOGNIZE_SUCCESS,  ///< 11: 检测到预览图片帧可识别
        CAPTURE_EVENT_FOCUS_BEGIN,          ///< 12: 开始对焦
        CAPTURE_EVENT_FOCUS_SUCCESS,        ///< 13: 对焦成功
        CAPTURE_EVENT_FOCUS_FAIL,           ///< 14: 对焦失败
        CAPTURE_EVENT_CAPTURE_DETECT_CORNER_POINTS_SUCCESS,    ///< 15: 拍照获得的图片检测切边信息成功  暂未使用
        CAPTURE_EVENT_CAPTURE_DETECT_CORNER_POINTS_FAIL,       ///< 16: 拍照获得的图片检测切边信息失败  暂未使用
    } CAPTURE_EVENT;
    
    /**
     *CAPTURE_ERR_CODE  扩展接口错误码定义
     */
    typedef enum
    {
        CAPTURE_ERR_UNKNOWN = -1,					///< -1: 未知错误，不会出现
        CAPTURE_ERR_NONE = 0,						///< 0: 成功
        CAPTURE_ERR_ALREADY_INITED,                 ///< 1: 已经初始化
        CAPTURE_ERR_NOT_INITED,                     ///< 2: 拍图器没有初始化
        CAPTURE_ERR_ENGINE_INIT,					///< 3: 引擎初始化失败
        CAPTURE_ERR_DEVICE_INIT,					///< 4: 设备初始化错误
        CAPTURE_ERR_CAPTURE,                        ///< 5: 扫描错误
        CAPTURE_ERR_LOAD_TEMPLATE,                  ///< 6: 加载模板错误
        CAPTURE_ERR_UNLOAD_TEMPLATE,                ///< 7: 卸载模板错误
        CAPTURE_ERR_SESSION_START,                  ///< 8: 启动任务失败
        CAPTURE_ERR_RECOGNIZE,                      ///< 9: 识别失败
        CAPTURE_ERR_LOAD_IMAGE,                     ///< 10: 加载图片失败
        CAPTURE_ERR_UNSUPPORT,                      ///< 11: 接口暂不支持
        CAPTURE_ERR_RELEASE,                        ///< 12: 释放失败
        CAPTURE_ERR_BUSY,                           ///< 13: 状态忙碌，正在拍照或识别
        CAPTURE_ERR_CAPTUREONE,                     ///< 14: 拍照失败
        CAPTURE_ERR_PARAM_INVALID,                  ///< 15: 参数非法
        CAPTURE_ERR_OUT_OF_MEMORY,                  ///< 16: 内存不足
    } CAPTURE_ERR_CODE;
    
    //////////////////////////////////////////////////////////////////////////
    // 回调函数定义
    
    /**
     * @brief	 事件变化回调
     * @note
     * CAPTURE_EVENT_RECOGNIZE_FINISH 事件不会在该回调中触发，它只会在 Callback_CaptureEventRecogFinish 中触发
     * CAPTURE_EVENT_ENGINE_ERROR、CAPTURE_EVENT_DEVICE_ERROR 事件不会在该回调中触发，只会在 Callback_CaptureEventError 中触发
     * @param	eCaptureEvent		回调时的通知事件
     * @param	pUsrParam			用户自定义参数
     */
    typedef void (HCIAPI * Callback_CaptureEventStateChange)(
                                                             _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                                                             _OPT_ _IN_ void * pUsrParam );
    
    /**
     * @brief	图片抓取回调
     * @note
     * @param	pszImageData		图像数据
     * @param	uiImageLen			图像数据长度
     * @param	bHasConers			是否切边,扫描仪返回true
     * @param	pCornersResult	    图像坐标,扫描仪返回NULL
     * @param	pUsrParam	        用户自定义参数
     */
    typedef void (HCIAPI * Callback_CaptureCapturing)(
                                                      _MUST_ _IN_ void * pszImageData,
                                                      _MUST_ _IN_ unsigned int uiImageLen,
                                                      _MUST_ _IN_ bool bHasConers,
                                                      _OPT_ _IN_ OCR_CORNERS_RESULT *pCornersResult,
                                                      _OPT_ _IN_ void * pUsrParam);
    
    /**
     * @brief	拍照器识别完成回调
     * @note
     * 识别成功后回调
     * @param	eCaptureEvent		识别完成事件
     * @param	pRecogResult		识别结果
     * @param	pUsrParam			用户自定义参数
     */
    typedef void (HCIAPI * Callback_CaptureEventRecogFinish)(
                                                             _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                                                             _MUST_ _IN_ OCR_RECOG_RESULT *pRecogResult,
                                                             _OPT_ _IN_ void * pUsrParam );
    
    /**
     * @brief	OCR SDK出错回调
     * @note
     * 发生事件 CAPTURE_EVENT_ENGINE_ERROR 或 CAPTURE_EVENT_DEVICE_ERROR 时回调
     * @param	eCaptureEvent		回调时的通知事件
     * @param	nErrorCode			错误码
     * @param	pUsrParam			用户自定义参数
     */
    typedef void (HCIAPI * Callback_CaptureEventError)(
                                                       _MUST_ _IN_ CAPTURE_EVENT eCaptureEvent,
                                                       _MUST_ _IN_ int eErrorCode,
                                                       _OPT_ _IN_ void * pUsrParam );
    
    /**
     * @brief	拍照器回调函数的结构体
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
    // 接口API函数定义
    
    /**
     * @brief	拍照器SDK初始化
     * @param	pszOCRSdkConfig		拍照器初始化配置串
     * @param	psCallbackParam		回调函数的集合
     * @par 附加配置串定义：
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
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_init(
                                                 _MUST_ _IN_ const char * pszOCRSdkConfig,
                                                 _MUST_ _IN_ CAPTURE_CALLBACK_PARAM *psCallbackParam);
    
    /**
     * @brief	OCR Capture加载模板文件
     * @param    pszTemplateFile    模板路径
     * @param    pnTemplateId    模板标识ID
     */
    
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_load_template(
                                                          _MUST_ _IN_ const char * pszTemplateFile,
                                                          _MUST_ _OUT_ int * pnTemplateId
                                                          );
    
    /**
     * @brief	OCR Capture卸载模板文件
     * @param    nTemplateId    模板标识ID
     */
    
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_unload_template(
                                                            _MUST_ _IN_  int nTemplateId
                                                            );
    
    /**
     * @brief	OCR Capture开启扫描并识别
     * @param	pszConfig     识别配置项
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
     *           <td rowspan="3">识别通用</td>
     *			<td>deviceName</td>
     *			<td>S225</td>
     *			<td>空</td>
     *			<td>设备名称（Windows平台有效）</td>
     *			<td>该项不填写则使用默认设备（Windows平台有效）</td>
     *		</tr>
     *		<tr>
     *			<td>showSettingUi</td>
     *			<td>yes,no</td>
     *			<td>yes</td>
     *			<td>是否在start后显示设置界面（Windows平台有效）</td>
     *			<td>是否在start后显示设置界面（Windows平台有效）</td>
     *		</tr>
     *       <tr>
     *			<td>capKey</td>
     *			<td>ocr.cloud</td>
     *			<td>无</td>
     *			<td>OCR识别能力key</td>
     *			<td>参见 @ref hci_ocr_page 。每个session只能定义一种能力，并且过程中不能改变。</td>
     *		</tr>
     *		<tr>
     *			<td rowspan="9">文本识别</td>
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
     *			<td>是否自动倾斜校正，自动识别时才支持</td>
     *			<td></td>
     *		</tr>
     *		<tr>
     *			<td>autoLayout</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>是否进行自动版面分析，自动识别时才支持</td>
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
     *			<td>设置输出结果简繁体转换</td>
     *			<td>nochange: 简繁体不做变化<br/>tosimplified: 繁体转换为简体</td>
     *		</tr>
     *		<tr>
     *			<td>detailResult</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>是否返回详细结果</td>
     *			<td>缺省只返回识别结果的文本串，如果设为yes，则会返回详细的节点信息 @ref OCR_RECOG_NODE </td>
     *		</tr>
     *		<tr>
     *			<td>binarize</td>
     *			<td>adaptive, global</td>
     *			<td>adaptive</td>
     *			<td>二值化方法，仅对灰度或者彩色图像有效</td>
     *			<td>global: 全局二值化算法，主要针对通过扫描、截屏等获取的图像<br/>
     *              adaptive: 局部自适应二值化算法，主要针对通过拍照、摄像头等获取的受光照影响较大图像
     *			</td>
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
     *			<td>localProcess</td>
     *			<td>yes, no</td>
     *			<td>yes</td>
     *			<td>云端能力，设置图像时
     *				是否先做本地预处理，现在主要是指二值化</td>
     *			<td>yes: 二值化<br/>
     *              no: 不进行二值化
     *			</td>
     *		</tr>
     *		<tr>
     *			<td rowspan="3">模板识别</td>
     *			<td>templateID</td>
     *			<td>1</td>
     *			<td>无</td>
     *			<td>模板文件的ID</td>
     *			<td>本地和云端模板识别支持，即capKey为ocr.local.template和ocr.cloud.template时必选，其他情况忽略。<br/>
     *				本地识别时，本参数是由 hci_ocr_capture_load_template()返回的ID；<br/>
     *				云端识别时，请咨询捷通华声或从灵云开发者社区了解</td>
     *		</tr>
     *		<tr>
     *			<td>templateIndex</td>
     *			<td>0到对应模板文件中的最大模板索引号</td>
     *			<td>无</td>
     *			<td>只针对模板识别生效，模板文件中的模板索引号</td>
     *			<td>模板文件中可能有多个模板，比如身份证、票据等，本参数指定模板在模板文件中的索引号</td>
     *		</tr>
     *		<tr>
     *			<td>templatePageIndex</td>
     *			<td>0到对应模板中的最大页索引号</td>
     *			<td>无</td>
     *			<td>只针对模板识别生效，模板页索引号</td>
     *			<td>模板中可能存在多个模板页，比如身份证的正面和反面，本参数指定模板页在模板中的索引号</td>
     *		</tr>
     *	</table>
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_start(
                                                  _MUST_ _IN_ const char * pszConfig);
    
    /**
     * @brief	获取预览界面，该接口Windows不支持(iOS专用)
     * @param	preview		用户传入的UIView，预览内容将展示在此view上
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_get_preview(void * preview);
    
    /**
     * @brief	停止相机并识别
     *
     *
     * 当识别名片和模板时返回图像数据与识别结果，否则仅返回图像数据，该接口Windows不支持
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_stop_and_recog();
    
    /**
     * @brief	停止相机（不识别）
     *
     *
     * 该接口Windows不支持
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_stop();
    
    /**
     * @brief	OCR Capture   支持识别单张图片识别，通过回调返回处理结果
     * @param	pszConfig     识别配置项
     * @param	pImageData    图像数据，可以通过isFile字段来指定使用文件路径还是图像数据内存
     * @param    uiImageLen    设置图像数据的长度，如果接口传入的是路径该设置项无效
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
     *			<td rowspan="3">识别通用</td>
     *			<td>capKey</td>
     *			<td>ocr.cloud</td>
     *			<td>无</td>
     *			<td>OCR识别能力key</td>
     *			<td>参见 @ref hci_ocr_page 。每个session只能定义一种能力，并且过程中不能改变。</td>
     *		</tr>
     *		<tr>
     *			<td>cutEdge</td>
     *			<td>yes, no </td>
     *			<td>no</td>
     *			<td>原始图片需要裁边，该配置只适用于本地识别</td>
     *			<td>yes,需要裁边,对于拍照照片适用，原始图像为未裁边图像<br/>
     *				no, 不需要裁边，非拍照图片使用，原始图像为裁边图像<br/>
     *			</td>
     *		</tr>
     *		<tr>
     *			<td>isFile</td>
     *			<td>yes,no</td>
     *			<td>no</td>
     *			<td>是否文件路径输入</td>
     *			<td>是否是文件路径输入</td>
     *		</tr> 
     *		<tr>
     *			<td rowspan="9">文本识别</td>
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
     *			<td>是否自动倾斜校正，自动识别时才支持</td>
     *			<td></td>
     *		</tr>
     *		<tr>
     *			<td>autoLayout</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>是否进行自动版面分析，自动识别时才支持</td>
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
     *			<td>设置输出结果简繁体转换</td>
     *			<td>nochange: 简繁体不做变化<br/>tosimplified: 繁体转换为简体</td>
     *		</tr> 
     *		<tr>
     *			<td>detailResult</td>
     *			<td>no, yes</td>
     *			<td>no</td>
     *			<td>是否返回详细结果</td>
     *			<td>缺省只返回识别结果的文本串，如果设为yes，则会返回详细的节点信息 @ref OCR_RECOG_NODE </td>
     *		</tr>
     *		<tr>
     *			<td>binarize</td>
     *			<td>adaptive, global</td>
     *			<td>adaptive</td>
     *			<td>二值化方法，仅对灰度或者彩色图像有效</td>
     *			<td>global: 全局二值化算法，主要针对通过扫描、截屏等获取的图像<br/>
     *              adaptive: 局部自适应二值化算法，主要针对通过拍照、摄像头等获取的受光照影响较大图像
     *			</td>
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
     *			<td>localProcess</td>
     *			<td>yes, no</td>
     *			<td>yes</td>
     *			<td>云端能力，设置图像时
     *				是否先做本地预处理，现在主要是指二值化</td>
     *			<td>yes: 二值化<br/>
     *              no: 不进行二值化
     *			</td>
     *		</tr> 
     *		<tr>
     *			<td rowspan="3">模板识别</td>
     *			<td>templateID</td>
     *			<td>1</td>
     *			<td>无</td>
     *			<td>模板文件的ID</td>
     *			<td>本地和云端模板识别支持，即capKey为ocr.local.template和ocr.cloud.template时必选，其他情况忽略。<br/>
     *				本地识别时，本参数是由 hci_ocr_capture_load_template()返回的ID；<br/>
     *				云端识别时，请咨询捷通华声或从灵云开发者社区了解</td>
     *		</tr>
     *		<tr>
     *			<td>templateIndex</td>
     *			<td>0到对应模板文件中的最大模板索引号</td>
     *			<td>无</td>
     *			<td>只针对模板识别生效，模板文件中的模板索引号</td>
     *			<td>模板文件中可能有多个模板，比如身份证、票据等，本参数指定模板在模板文件中的索引号</td>
     *		</tr> 
     *		<tr>
     *			<td>templatePageIndex</td>
     *			<td>0到对应模板中的最大页索引号</td>
     *			<td>无</td>
     *			<td>只针对模板识别生效，模板页索引号</td>
     *			<td>模板中可能存在多个模板页，比如身份证的正面和反面，本参数指定模板页在模板中的索引号</td>
     *		</tr> 
     *	</table>
     */ 
    
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_recog(
                                                  _MUST_ _IN_ const char * pszConfig,
                                                  _MUST_ _IN_ unsigned char * pImageData,
                                                  _MUST_ _IN_ unsigned int uiImageLen);
    
    /**  
     * @brief	OCR Capture反初始化
     * 
     *
     *
     * 关闭相机，释放ocr和相机资源
     */
    CAPTURE_ERR_CODE HCIAPI hci_ocr_capture_release();
    
    /* @} */
    //////////////////////////////////////////////////////////////////////////
    
#ifdef __cplusplus
}
#endif

#endif
