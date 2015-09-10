package com.sinovoice.example.ocr;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

import android.content.Context;
import android.os.Environment;
import android.util.Log;

import com.sinovoice.example.sys.AccountInfo;
import com.sinovoice.hcicloudsdk.api.ocr.HciCloudOcr;
import com.sinovoice.hcicloudsdk.common.HciErrorCode;
import com.sinovoice.hcicloudsdk.common.Session;
import com.sinovoice.hcicloudsdk.common.ocr.OcrInitParam;
import com.sinovoice.hcicloudsdk.common.ocr.OcrRecogDeskewResult;
import com.sinovoice.hcicloudsdk.common.ocr.OcrRecogLayoutAnalysisResult;
import com.sinovoice.hcicloudsdk.common.ocr.OcrRecogNode;
import com.sinovoice.hcicloudsdk.common.ocr.OcrRecogRegion;
import com.sinovoice.hcicloudsdk.common.ocr.OcrRecogResult;
import com.sinovoice.hcicloudsdk.common.ocr.OcrTemplateId;
import com.sinovoice.hcicloudsdk.common.tts.TtsInitParam;

public class HciCloudOcrHelper {
	private static final String TAG = HciCloudOcrHelper.class.getSimpleName();
	private static final String[] mRegionType = { "HORZ", "VERT", "TABLE",
			"GRAPH", "AUTOTEXT" };
	private static final String[] mNodeType = { "PAGE", "RGN ", "CELL", "LINE",
			"CHAR" };
	private static final String[] mLangType = { "default", "zh-cn", "zh-hk",
			"zh-tw", "english" };
	
	private static HciCloudOcrHelper mInstance;
	private String mCapKey;
	private Context mContext;
	
    private HciCloudOcrHelper() {
    }
    
	public static HciCloudOcrHelper getInstance() {
		if (mInstance == null) {
			mInstance = new HciCloudOcrHelper();
		}
		return mInstance;
	}

	/**
     *  ocr的初始化方法
     */
	public int init(Context context) {
		this.mCapKey = AccountInfo.getInstance().getCapKey();
		this.mContext = context;
		// 构造Ocr初始化的帮助类的实例
		OcrInitParam ocrInitParam = new OcrInitParam();
		
	    // 获取App应用中的lib的路径,放置能力所需资源文件。如果使用/data/data/packagename/lib目录,需要添加android_so的标记
		// 注意：使用bizcard 或者template能力时候 不支持使用"android_so",只能将资源库放入SD卡,并将路径绝对目录赋值给PARAM_KEY_DATA_PATH

		String sdcardState = Environment.getExternalStorageState();
		if(Environment.MEDIA_MOUNTED.equals(sdcardState)){
			String sdPath = Environment.getExternalStorageDirectory().getAbsolutePath();
			String packageName = mContext.getPackageName();
			String dataPath = sdPath + File.separator + "sinovoice"
                    + File.separator + packageName + File.separator + "data" + File.separator;

			ocrInitParam.addParam(OcrInitParam.PARAM_KEY_DATA_PATH, dataPath); 
			ocrInitParam.addParam(OcrInitParam.PARAM_KEY_FILE_FLAG, "none");
		}
		
        // 此处演示初始化的能力为ocr.local, 用户可以根据自己可用的能力进行设置, 另外,此处可以传入多个能力值,并用;隔开
        // 如 "ocr.local;ocr.local.bizcard;ocr.local.template"
		ocrInitParam.addParam(OcrInitParam.PARAM_KEY_INIT_CAP_KEYS, mCapKey);
		Log.i(TAG, "init config :" + ocrInitParam.getStringConfig());
        // 调用初始化方法
		int initResult = HciCloudOcr.hciOcrInit(ocrInitParam.getStringConfig());
		if (initResult == HciErrorCode.HCI_ERR_NONE) {
			Log.i(TAG, "OCR init success");
		} else {
			Log.e(TAG, "OCR init failed:" + initResult);

			return initResult;
		}

		return HciErrorCode.HCI_ERR_NONE;
	}

	/**
     *  Ocr反初始化
     */
	public int release() {
		int nRet = HciCloudOcr.hciOcrRelease();

		Log.i(TAG, "ocr release, result: " + nRet);
		return nRet;
	}

	/**
	 * 云端高级识别,用户自行调用倾斜矫正和版面分析
	 * @param data 图像的二进制数据
	 */
	public boolean advanceRecog(String fileNameString, Context context) {
		Log.i(TAG, "advanceRecog start...");
		
		this.mContext = context;		
		byte[] data = getAssetFileData(fileNameString, mContext);
		if(null == data) {
			Log.e(TAG, "Open input voice file" + fileNameString + "error!");
			return false;
		}
		
		this.mCapKey = AccountInfo.getInstance().getCapKey();
		// 启动 OCR Session
		String sSessionConfig = "capkey=" + mCapKey;
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart config=" + sSessionConfig);

		Session nSessionId = new Session();
		int errCode = HciCloudOcr.hciOcrSessionStart(sSessionConfig, nSessionId);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.e(TAG, "HciCloudOcr hciOcrSessionStart failed: " + errCode);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart Success");

		errCode = HciCloudOcr.hciOcrSetImageBuffer(nSessionId, data);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.e(TAG, "HciCloudOcr hciOcrSetImageBuffer failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSetImageBuffer Success");
		
		// 倾斜校正
		Log.i(TAG, "deskew start...");
		
		// 如果传入的图片为截屏图片，可以添加参数imageType=screen ， 会有识别效果的提升，默认参数为imageType=normal
		// 获取倾斜矫正结果的对象
		OcrRecogDeskewResult ocrDeskewResult = new OcrRecogDeskewResult();

		// 开始矫正，传入默认参数全部设置为默认值
		errCode = HciCloudOcr.hciOcrDeskew(nSessionId, null, ocrDeskewResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrDeskew failed:" + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrDeskew success");

		// 版面分析
		Log.i(TAG, "analysis start...");
		OcrRecogLayoutAnalysisResult ocrLayoutResult = new OcrRecogLayoutAnalysisResult();

		// 添加数据
		// 开始版面分析,分析配置参数传入空, 选择默认
		errCode = HciCloudOcr.hciOcrLayoutAnalysis(nSessionId, null, ocrLayoutResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrLayoutAnalysis failed:" + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrLayoutAnalysis success");


		Log.i(TAG, "recog start.");
		
		// 配置参数
		String strRecogConfig = "detailresult=yes,recogrange=all";
		
		// 版面分析的结果要作为参数传递给识别方法
		ArrayList<OcrRecogRegion> ocrlayoutResultList = ocrLayoutResult.getRecogRegionList();

		OcrRecogResult recogResult = new OcrRecogResult();
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, ocrlayoutResultList, recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed:" + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");

		// 输出详细信息
		outPutRecogResult(recogResult);

		// 关闭会话
		errCode = HciCloudOcr.hciOcrSessionStop(nSessionId);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrSessionStop failed:" + errCode);
			return false;
		}
		
		Log.i(TAG, "advanceRecog leave...");
		return true;
	}

	public boolean autoRecog(String fileNameString, Context context) {
		Log.i(TAG, "autoRecog start...");
		
		this.mContext = context;
		byte[] data = getAssetFileData(fileNameString, mContext);
		if(null == data) {
			Log.e(TAG, "Open input voice file" + fileNameString + "error!");
			return false;
		}
		
		this.mCapKey = AccountInfo.getInstance().getCapKey();
		
		// 启动 OCR Session
		String sSessionConfig = "capkey=" + mCapKey;
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart config: " + sSessionConfig);
		Session nSessionId = new Session();
		
		int errCode = HciCloudOcr.hciOcrSessionStart(sSessionConfig, nSessionId);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.e(TAG, "HciCloudOcr hciOcrSessionStart return " + errCode);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart Success");

		// --------------------------载入本次会话的图像到内存-------------------------------------

		errCode = HciCloudOcr.hciOcrSetImageBuffer(nSessionId, data);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.e(TAG, "HciCloudOcr hciOcrSetImageBuffer failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSetImageBuffer Success");

		String strRecogConfig = "";
		if(mCapKey.contains("cloud.template")) {
			strRecogConfig = "cutedge=no,domain=idcard,templateIndex=0,templatePageIndex=0";
		} else {
			strRecogConfig = "autodeskew=yes,autolayout=yes,detailresult=yes,recogrange=all";
		}
		
		/*
		 * 如果传入的图片为截屏图片，可以添加参数imageType=screen
		 * 会有识别效果的提升，默认参数为imageType=normal
		 */
		OcrRecogResult recogResult = new OcrRecogResult();
		// 开始识别, 由于不指定识别区域,第三个参数传入NULL
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, null, recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");
		
		Log.i(TAG, "AutoRecog result: " + recogResult.getResultText());
		Log.i(TAG, "\n");

		// 关闭会话
		errCode = HciCloudOcr.hciOcrSessionStop(nSessionId);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "hciOcrSessionStop failed: " + errCode);
			return false;
		}

		Log.i(TAG, "autoRecog leave...");
		return true;
	}
	
	public boolean bizcardRecog(String fileNameString, Context context) {
		Log.i(TAG, "bizcardRecog start...");

		this.mContext = context;
		byte[] data = getAssetFileData(fileNameString, mContext);
		if(null == data) {
			Log.e(TAG, "Open input voice file" + fileNameString + "error!");
			return false;
		}
		this.mCapKey = AccountInfo.getInstance().getCapKey();
		
		// 启动 OCR Session
		String sSessionConfig = "capkey=" + mCapKey;
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart config: " + sSessionConfig);
		Session nSessionId = new Session();
		int errCode = HciCloudOcr.hciOcrSessionStart(sSessionConfig, nSessionId);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.i(TAG, "HciCloudOcr hciOcrSessionStart return " + errCode);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart Success");
		
		// --------------------------载入本次会话的图像到内存-------------------------------------------------------------
		errCode = HciCloudOcr.hciOcrSetImageBuffer(nSessionId, data);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrSetImageBuffer failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSetImageBuffer Success");
		
		// 识别配置中注意cutEdge的配置,如果图像已经是切边图像则设置为no，否则可以采用默认值yes
		String strRecogConfig = "cutedge=yes";
		OcrRecogResult recogResult = new OcrRecogResult();
		// 开始识别, 由于不指定识别区域,第三个参数传入NULL
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, null, recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");
		
		Log.i(TAG, "bizcardRecog result: " + recogResult.getResultText());
		Log.i(TAG, "\n");

		// 关闭会话
		errCode = HciCloudOcr.hciOcrSessionStop(nSessionId);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrSessionStop failed: " + errCode);
			return false;
		}

		Log.i(TAG, "bizcardRecog leave...");
		return true;
	}
	
	public boolean templateRecog(String fileNameString, Context context) {	
		Log.i(TAG, "templateRecog start...");
		
		this.mContext = context;
		byte[] data = getAssetFileData(fileNameString, mContext);
		if(null == data) {
			Log.e(TAG, "Open input voice file" + fileNameString + "error!");
			return false;
		}
		this.mCapKey = AccountInfo.getInstance().getCapKey();
		
		int errCode = 0;
		OcrTemplateId templateId = new OcrTemplateId();		
		
		String sdPath = Environment.getExternalStorageDirectory().getAbsolutePath();
		String packageName = mContext.getPackageName();
		String ocrTemplatePath = sdPath + File.separator + "sinovoice"
                + File.separator + packageName + File.separator + "data" + File.separator;

		if (mCapKey.contains("ocr.local.template")) {
			// 本地模板识别需要手动加载模板文件
			errCode = HciCloudOcr.hciOcrLoadTemplate(ocrTemplatePath
					+ "/templates/IDCard_EN.xml", templateId);
			if (errCode != HciErrorCode.HCI_ERR_NONE) {
				Log.e(TAG, "load template failed: " + errCode);
				return false;
			}
			Log.i(TAG, "load template success");
		} else {
			// 此处指定模板ID为1，需要根据实际服务器部署的模板情况进行设定
			templateId.setTemplateId(1);
		}

		String sSessionConfig = "capkey=" + mCapKey;
		sSessionConfig += ",templateid=" + templateId.getTemplateId();
		Session nSessionId = new Session();
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart config: " + sSessionConfig);

		errCode = HciCloudOcr.hciOcrSessionStart(sSessionConfig, nSessionId);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.i(TAG, "HciCloudOcr hciOcrSessionStart failed: " + errCode);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart Success");

		errCode = HciCloudOcr.hciOcrSetImageBuffer(nSessionId, data);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrSetImageBuffer failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSetImageBuffer Success");
		
		// 识别配置中注意cutEdge的配置,如果图像已经是切边图像则设置为no，否则可以采用默认值yes
		// 模板信息配置参考开发手册中对模板文件的说明
		String strRecogConfig = "templateIndex=0,templatePageIndex=0,cutedge=no";
		OcrRecogResult recogResult = new OcrRecogResult();
		// 开始识别, 由于不指定识别区域,第三个参数传入NULL
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, null,recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");
		
		Log.i(TAG, "templateRecog result: " + recogResult.getResultText());
		Log.i(TAG, "\n");

		// 关闭会话
		errCode = HciCloudOcr.hciOcrSessionStop(nSessionId);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "hciOcrSessionStop failed: " + errCode);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSessionStop success");
		
		if (mCapKey.contains("ocr.local.template")) {
			errCode = HciCloudOcr.hciOcrUnloadTemplate(templateId);
			if (errCode != HciErrorCode.HCI_ERR_NONE) {
				Log.e(TAG, "unLoad Template failed: " + errCode);
				return false;
			}
			Log.i(TAG, "HciCloudOcr hciOcrUnloadTemplate success");
		}
		
		Log.i(TAG, "templateRecog leave...");
		return true;
	}
	
	/**
	 * 获取指定Assert文件中的数据
	 * 
	 * @param fileName
	 * @return
	 */
	private byte[] getAssetFileData(String fileName, Context context) {
		InputStream in = null;
		int size = 0;
		try {
			in = context.getResources().getAssets().open(fileName);
			size = in.available();
			byte[] data = new byte[size];
			in.read(data, 0, size);

			return data;
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}
	
	
	/**
	 * 输出识别结果
	 * 
	 * @param recogResult
	 *            识别结果对象
	 */
	private void outPutRecogResult(OcrRecogResult recogResult) {
		if (recogResult == null) {
			Log.e(TAG, "recogResult is null.");
			return;
		}

		Log.i(TAG, "RecogResultText=" + recogResult.getResultText());
		

		ArrayList<OcrRecogNode> arrayResult = recogResult.getNodeList();
		if (arrayResult == null) {
			Log.e(TAG, "OcrRecogNodeList is null.");
			return;
		}

		for (int i = 0; i < arrayResult.size(); i++) {
			// 识别结果节点类型
			OcrRecogNode recogNode = arrayResult.get(i);
			int nodeType = recogNode.getNodeType();
			Log.i(TAG, mNodeType[nodeType] + ": ");
			if (HciCloudOcr.OCR_RECOG_NODE_PAGE == nodeType) {
				// 页面宽带
				Log.i(TAG, "(" + recogNode.getPageNode().getWidth() + " x "
						+ recogNode.getPageNode().getHeight() + ") Skew: "
						+ recogNode.getPageNode().getDeskewAngle());
			} else if (HciCloudOcr.OCR_RECOG_NODE_REGION == nodeType) {
				// 区域类型
				Log.i(TAG, mRegionType[recogNode.getRegionNode()
						.getRegionType()]);

				// 区域相对于整个图像的位置
				Log.i(TAG, " ("
						+ recogNode.getRegionNode().getRegionRect().left + ", "
						+ recogNode.getRegionNode().getRegionRect().top + ", "
						+ recogNode.getRegionNode().getRegionRect().right
						+ ", "
						+ recogNode.getRegionNode().getRegionRect().bottom
						+ ")");
				// 区域的实际识别语言
				Log.i(TAG, " Lang: "
						+ mLangType[recogNode.getRegionNode().getRegionType()]);
			} else if (HciCloudOcr.OCR_RECOG_NODE_CELL == nodeType) {
				// 此表格单元的位置
				Log.i(TAG, "(" + recogNode.getCellNode().getCellRect().left
						+ ", " + recogNode.getCellNode().getCellRect().top
						+ ", " + recogNode.getCellNode().getCellRect().right
						+ ", " + recogNode.getCellNode().getCellRect().bottom
						+ ")");
			} else if (HciCloudOcr.OCR_RECOG_NODE_LINE == nodeType) {
				// 此行的基线
				Log.i(TAG, "baseline: " + recogNode.getLineNode().getBaseLine()
						+ " height:" + recogNode.getLineNode().getLineSize()
						+ " avgCharSize: " + recogNode.getLineNode().getXChar()
						+ " x " + recogNode.getLineNode().getYChar());
			} else if (HciCloudOcr.OCR_RECOG_NODE_CHAR == nodeType) {
				// 是否可信
				Log.i(TAG, recogNode.getCharNode().isAccept() ? " " : "*");
				// 识别结果单字
				if (recogNode.getCharNode().getChar() != null) {
					Log.i(TAG, recogNode.getCharNode().getChar());
				}

				// 多个候选字的识别结果
				Log.i(TAG, "[");
				final int CANDIDATE_NUM = 5;
				for (int j = 0; j < CANDIDATE_NUM; j++) {
					Log.i(TAG,
							recogNode.getCharNode().getCandidateWords().get(j));
				}
				Log.i(TAG, "]");

				// 此字符在图像中区域
				Log.i(TAG, " (" + recogNode.getCharNode().getImageRect().left
						+ ", " + recogNode.getCharNode().getImageRect().top
						+ ", " + recogNode.getCharNode().getImageRect().bottom
						+ ", " + recogNode.getCharNode().getImageRect().right
						+ ")");

				// 字符前面的空格字符数
				if (recogNode.getCharNode().getSpaceBefore() != 0) {
					Log.i(TAG, "LeadingSpace: "
							+ recogNode.getCharNode().getSpaceBefore());
				}
			}
			Log.i(TAG, "");
		}
	}
	
}
