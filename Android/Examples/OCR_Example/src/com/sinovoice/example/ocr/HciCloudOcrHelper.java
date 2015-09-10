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
     *  ocr�ĳ�ʼ������
     */
	public int init(Context context) {
		this.mCapKey = AccountInfo.getInstance().getCapKey();
		this.mContext = context;
		// ����Ocr��ʼ���İ������ʵ��
		OcrInitParam ocrInitParam = new OcrInitParam();
		
	    // ��ȡAppӦ���е�lib��·��,��������������Դ�ļ������ʹ��/data/data/packagename/libĿ¼,��Ҫ���android_so�ı��
		// ע�⣺ʹ��bizcard ����template����ʱ�� ��֧��ʹ��"android_so",ֻ�ܽ���Դ�����SD��,����·������Ŀ¼��ֵ��PARAM_KEY_DATA_PATH

		String sdcardState = Environment.getExternalStorageState();
		if(Environment.MEDIA_MOUNTED.equals(sdcardState)){
			String sdPath = Environment.getExternalStorageDirectory().getAbsolutePath();
			String packageName = mContext.getPackageName();
			String dataPath = sdPath + File.separator + "sinovoice"
                    + File.separator + packageName + File.separator + "data" + File.separator;

			ocrInitParam.addParam(OcrInitParam.PARAM_KEY_DATA_PATH, dataPath); 
			ocrInitParam.addParam(OcrInitParam.PARAM_KEY_FILE_FLAG, "none");
		}
		
        // �˴���ʾ��ʼ��������Ϊocr.local, �û����Ը����Լ����õ�������������, ����,�˴����Դ���������ֵ,����;����
        // �� "ocr.local;ocr.local.bizcard;ocr.local.template"
		ocrInitParam.addParam(OcrInitParam.PARAM_KEY_INIT_CAP_KEYS, mCapKey);
		Log.i(TAG, "init config :" + ocrInitParam.getStringConfig());
        // ���ó�ʼ������
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
     *  Ocr����ʼ��
     */
	public int release() {
		int nRet = HciCloudOcr.hciOcrRelease();

		Log.i(TAG, "ocr release, result: " + nRet);
		return nRet;
	}

	/**
	 * �ƶ˸߼�ʶ��,�û����е�����б�����Ͱ������
	 * @param data ͼ��Ķ���������
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
		// ���� OCR Session
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
		
		// ��бУ��
		Log.i(TAG, "deskew start...");
		
		// ��������ͼƬΪ����ͼƬ��������Ӳ���imageType=screen �� ����ʶ��Ч����������Ĭ�ϲ���ΪimageType=normal
		// ��ȡ��б��������Ķ���
		OcrRecogDeskewResult ocrDeskewResult = new OcrRecogDeskewResult();

		// ��ʼ����������Ĭ�ϲ���ȫ������ΪĬ��ֵ
		errCode = HciCloudOcr.hciOcrDeskew(nSessionId, null, ocrDeskewResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrDeskew failed:" + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrDeskew success");

		// �������
		Log.i(TAG, "analysis start...");
		OcrRecogLayoutAnalysisResult ocrLayoutResult = new OcrRecogLayoutAnalysisResult();

		// �������
		// ��ʼ�������,�������ò��������, ѡ��Ĭ��
		errCode = HciCloudOcr.hciOcrLayoutAnalysis(nSessionId, null, ocrLayoutResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrLayoutAnalysis failed:" + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrLayoutAnalysis success");


		Log.i(TAG, "recog start.");
		
		// ���ò���
		String strRecogConfig = "detailresult=yes,recogrange=all";
		
		// ��������Ľ��Ҫ��Ϊ�������ݸ�ʶ�𷽷�
		ArrayList<OcrRecogRegion> ocrlayoutResultList = ocrLayoutResult.getRecogRegionList();

		OcrRecogResult recogResult = new OcrRecogResult();
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, ocrlayoutResultList, recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed:" + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");

		// �����ϸ��Ϣ
		outPutRecogResult(recogResult);

		// �رջỰ
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
		
		// ���� OCR Session
		String sSessionConfig = "capkey=" + mCapKey;
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart config: " + sSessionConfig);
		Session nSessionId = new Session();
		
		int errCode = HciCloudOcr.hciOcrSessionStart(sSessionConfig, nSessionId);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.e(TAG, "HciCloudOcr hciOcrSessionStart return " + errCode);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart Success");

		// --------------------------���뱾�λỰ��ͼ���ڴ�-------------------------------------

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
		 * ��������ͼƬΪ����ͼƬ��������Ӳ���imageType=screen
		 * ����ʶ��Ч����������Ĭ�ϲ���ΪimageType=normal
		 */
		OcrRecogResult recogResult = new OcrRecogResult();
		// ��ʼʶ��, ���ڲ�ָ��ʶ������,��������������NULL
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, null, recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");
		
		Log.i(TAG, "AutoRecog result: " + recogResult.getResultText());
		Log.i(TAG, "\n");

		// �رջỰ
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
		
		// ���� OCR Session
		String sSessionConfig = "capkey=" + mCapKey;
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart config: " + sSessionConfig);
		Session nSessionId = new Session();
		int errCode = HciCloudOcr.hciOcrSessionStart(sSessionConfig, nSessionId);
		if (HciErrorCode.HCI_ERR_NONE != errCode) {
			Log.i(TAG, "HciCloudOcr hciOcrSessionStart return " + errCode);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSessionStart Success");
		
		// --------------------------���뱾�λỰ��ͼ���ڴ�-------------------------------------------------------------
		errCode = HciCloudOcr.hciOcrSetImageBuffer(nSessionId, data);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrSetImageBuffer failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrSetImageBuffer Success");
		
		// ʶ��������ע��cutEdge������,���ͼ���Ѿ����б�ͼ��������Ϊno��������Բ���Ĭ��ֵyes
		String strRecogConfig = "cutedge=yes";
		OcrRecogResult recogResult = new OcrRecogResult();
		// ��ʼʶ��, ���ڲ�ָ��ʶ������,��������������NULL
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, null, recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");
		
		Log.i(TAG, "bizcardRecog result: " + recogResult.getResultText());
		Log.i(TAG, "\n");

		// �رջỰ
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
			// ����ģ��ʶ����Ҫ�ֶ�����ģ���ļ�
			errCode = HciCloudOcr.hciOcrLoadTemplate(ocrTemplatePath
					+ "/templates/IDCard_EN.xml", templateId);
			if (errCode != HciErrorCode.HCI_ERR_NONE) {
				Log.e(TAG, "load template failed: " + errCode);
				return false;
			}
			Log.i(TAG, "load template success");
		} else {
			// �˴�ָ��ģ��IDΪ1����Ҫ����ʵ�ʷ����������ģ����������趨
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
		
		// ʶ��������ע��cutEdge������,���ͼ���Ѿ����б�ͼ��������Ϊno��������Բ���Ĭ��ֵyes
		// ģ����Ϣ���òο������ֲ��ж�ģ���ļ���˵��
		String strRecogConfig = "templateIndex=0,templatePageIndex=0,cutedge=no";
		OcrRecogResult recogResult = new OcrRecogResult();
		// ��ʼʶ��, ���ڲ�ָ��ʶ������,��������������NULL
		errCode = HciCloudOcr.hciOcrRecog(nSessionId, strRecogConfig, null,recogResult);
		if (errCode != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "HciCloudOcr hciOcrRecog failed: " + errCode);
			HciCloudOcr.hciOcrSessionStop(nSessionId);
			return false;
		}
		Log.i(TAG, "HciCloudOcr hciOcrRecog success");
		
		Log.i(TAG, "templateRecog result: " + recogResult.getResultText());
		Log.i(TAG, "\n");

		// �رջỰ
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
	 * ��ȡָ��Assert�ļ��е�����
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
	 * ���ʶ����
	 * 
	 * @param recogResult
	 *            ʶ��������
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
			// ʶ�����ڵ�����
			OcrRecogNode recogNode = arrayResult.get(i);
			int nodeType = recogNode.getNodeType();
			Log.i(TAG, mNodeType[nodeType] + ": ");
			if (HciCloudOcr.OCR_RECOG_NODE_PAGE == nodeType) {
				// ҳ����
				Log.i(TAG, "(" + recogNode.getPageNode().getWidth() + " x "
						+ recogNode.getPageNode().getHeight() + ") Skew: "
						+ recogNode.getPageNode().getDeskewAngle());
			} else if (HciCloudOcr.OCR_RECOG_NODE_REGION == nodeType) {
				// ��������
				Log.i(TAG, mRegionType[recogNode.getRegionNode()
						.getRegionType()]);

				// �������������ͼ���λ��
				Log.i(TAG, " ("
						+ recogNode.getRegionNode().getRegionRect().left + ", "
						+ recogNode.getRegionNode().getRegionRect().top + ", "
						+ recogNode.getRegionNode().getRegionRect().right
						+ ", "
						+ recogNode.getRegionNode().getRegionRect().bottom
						+ ")");
				// �����ʵ��ʶ������
				Log.i(TAG, " Lang: "
						+ mLangType[recogNode.getRegionNode().getRegionType()]);
			} else if (HciCloudOcr.OCR_RECOG_NODE_CELL == nodeType) {
				// �˱��Ԫ��λ��
				Log.i(TAG, "(" + recogNode.getCellNode().getCellRect().left
						+ ", " + recogNode.getCellNode().getCellRect().top
						+ ", " + recogNode.getCellNode().getCellRect().right
						+ ", " + recogNode.getCellNode().getCellRect().bottom
						+ ")");
			} else if (HciCloudOcr.OCR_RECOG_NODE_LINE == nodeType) {
				// ���еĻ���
				Log.i(TAG, "baseline: " + recogNode.getLineNode().getBaseLine()
						+ " height:" + recogNode.getLineNode().getLineSize()
						+ " avgCharSize: " + recogNode.getLineNode().getXChar()
						+ " x " + recogNode.getLineNode().getYChar());
			} else if (HciCloudOcr.OCR_RECOG_NODE_CHAR == nodeType) {
				// �Ƿ����
				Log.i(TAG, recogNode.getCharNode().isAccept() ? " " : "*");
				// ʶ��������
				if (recogNode.getCharNode().getChar() != null) {
					Log.i(TAG, recogNode.getCharNode().getChar());
				}

				// �����ѡ�ֵ�ʶ����
				Log.i(TAG, "[");
				final int CANDIDATE_NUM = 5;
				for (int j = 0; j < CANDIDATE_NUM; j++) {
					Log.i(TAG,
							recogNode.getCharNode().getCandidateWords().get(j));
				}
				Log.i(TAG, "]");

				// ���ַ���ͼ��������
				Log.i(TAG, " (" + recogNode.getCharNode().getImageRect().left
						+ ", " + recogNode.getCharNode().getImageRect().top
						+ ", " + recogNode.getCharNode().getImageRect().bottom
						+ ", " + recogNode.getCharNode().getImageRect().right
						+ ")");

				// �ַ�ǰ��Ŀո��ַ���
				if (recogNode.getCharNode().getSpaceBefore() != 0) {
					Log.i(TAG, "LeadingSpace: "
							+ recogNode.getCharNode().getSpaceBefore());
				}
			}
			Log.i(TAG, "");
		}
	}
	
}
