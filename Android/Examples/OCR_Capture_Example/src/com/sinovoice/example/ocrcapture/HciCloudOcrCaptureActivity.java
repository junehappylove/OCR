package com.sinovoice.example.ocrcapture;

import java.io.File;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.sinovoice.example.sys.AccountInfo;
import com.sinovoice.example.sys.HciCloudSysHelper;
import com.sinovoice.hcicloudsdk.android.ocr.capture.CaptureErrCode;
import com.sinovoice.hcicloudsdk.android.ocr.capture.CaptureEvent;
import com.sinovoice.hcicloudsdk.android.ocr.capture.OCRCapture;
import com.sinovoice.hcicloudsdk.android.ocr.capture.OCRCaptureListener;
import com.sinovoice.hcicloudsdk.common.HciErrorCode;
import com.sinovoice.hcicloudsdk.common.ocr.OcrCornersResult;
import com.sinovoice.hcicloudsdk.common.ocr.OcrInitParam;
import com.sinovoice.hcicloudsdk.common.ocr.OcrRecogResult;
import com.sinovoice.hcicloudsdk.common.ocr.OcrTemplateId;


public class HciCloudOcrCaptureActivity extends Activity {
	private static final String TAG = "HciCloudOcrCaptureActivity";
	
    /**
     * �����û���Ϣ������
     */
    private AccountInfo mAccountInfo;

    /**
     * HciCloud�����࣬���������ϵͳ��ʼ�����ͷŲ�����
     */
    private HciCloudSysHelper mHciCloudSysHelper;    
	
//	private HciCloudOcrCaptureActivity activity;
//	private Context context;
	private Handler hander;
	private String capKey;
	
	//��ʾ����ͷԤ����ͼ�Ĳ����ļ�
	private FrameLayout cameraPreviewLayout;
	private ProgressDialog pDialog;
	
	//ʶ�����ò���
	private String recogConfig;
	//�Ƿ���Ҫ����ģ���ļ�
	private boolean isNeedLoadTemplate;
	//ģ���ļ����
	private OcrTemplateId currTemplateId;
	private int templateId;
	
	//������ģ��
	private OCRCapture ocrCapture;
	private OCRCaptureListener ocrCaptureListener = new OCRCaptureListener() {
		
		@Override
		public void onCaptureEventStateChange(CaptureEvent captureEvent) {
			// TODO Auto-generated method stub		
		}
		
		@Override
		public void onCaptureEventRecogFinish(CaptureEvent captureEvent,
				OcrRecogResult ocrRecogResult) {
			switch (captureEvent) {
			case CAPTURE_EVENT_RECOGNIZE_FINISH:
				if(ocrRecogResult != null){
					Log.v(TAG, "recog result = " + ocrRecogResult.getResultText());
					showResultView(ocrRecogResult.getResultText());
				}
				break;

			default:
				break;
			}
		}
		
		@Override
		public void onCaptureEventError(CaptureEvent captureEvent, int errorCode) {
			// TODO Auto-generated method stub		
		}
		
		@Override
		public void onCaptureEventCapturing(CaptureEvent captureEvent,
				final byte[] imageData, OcrCornersResult ocrCornersResult) {
			Log.i(TAG, "onCaptureEventCapturing. imageData len = " + imageData.length);
			
			//�ĵ�ʶ��
			if(capKey.equalsIgnoreCase("ocr.cloud")
					|| capKey.equalsIgnoreCase("ocr.cloud.english")
					|| capKey.equalsIgnoreCase("ocr.local")){
				
				hander.post(new Runnable() {
					
					@Override
					public void run() {
						ocrCapture.hciOcrCaptureRecog(imageData, recogConfig, null);
					}
				});
			}
		}
	};	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		//������Ļ��ʾ����
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);			
		
		//����Ĭ�ϲ���
		showMainView();
	
		hander = new Handler();		
		
		pDialog = ProgressDialog.show(this, getText(R.string.dialog_title_tips), getText(R.string.dialog_msg_hcicloud_sysinit));
				
        mAccountInfo = AccountInfo.getInstance();
        boolean loadResult = mAccountInfo.loadAccountInfo(this);
        if (loadResult) {
            // ������Ϣ�ɹ�����������
			Toast.makeText(getApplicationContext(), "���������˺ųɹ�",
					Toast.LENGTH_SHORT).show();
        } else {
            // ������Ϣʧ�ܣ���ʾʧ�ܽ���
			Toast.makeText(getApplicationContext(), "���������˺�ʧ�ܣ�����assets/AccountInfo.txt�ļ�����д��ȷ�������˻���Ϣ���˻���Ҫ��www.hcicloud.com������������ע�����롣",
					Toast.LENGTH_SHORT).show();
            return;
        }

        mHciCloudSysHelper = HciCloudSysHelper.getInstance();

        // �˷������߳������ģ����ҽ����н�����زŻ��������ִ�С�
        // �˴�ֻ����ʾ�ϳ������÷���û�жԺ�ʱ�������д�����Ҫ�����߷����̨�߳̽��г�ʼ������
        // �������ȵ���HciCloudSys�ĳ�ʼ������
        int sysInitResult = mHciCloudSysHelper.init(this);
        if (sysInitResult != HciErrorCode.HCI_ERR_NONE) {
            Log.e(TAG, "hci init error, error code = " + sysInitResult);
            return;
        }
        
		// ��ȡ�û��ĵ��õ�����
		capKey = mAccountInfo.getCapKey();
				
		if(capKey.equalsIgnoreCase("ocr.cloud")
				|| capKey.equalsIgnoreCase("ocr.cloud.english")
				|| capKey.equalsIgnoreCase("ocr.local")){
			//�ĵ�ʶ��
			recogConfig = "capKey=" + capKey;
		}else if(capKey.equalsIgnoreCase("ocr.local.bizcard.v6")){
			//��Ƭʶ��
			recogConfig = "capkey=ocr.local.bizcard.v6,cutedge=yes";
		}else if(capKey.equalsIgnoreCase("ocr.local.template.v6")){
			//ģ��ʶ��
			isNeedLoadTemplate = true;
		}else{
			Log.e(TAG, "δ֪��capKey�� capKey = " + capKey);
		}				
		
		ocrCapture = new OCRCapture();
		initOCRCapture();
		dismissDialog();											
	}

	/**
	 * ��ʼ��OCRģ�� 
	 */
	private void initOCRCapture() {
		OcrInitParam ocrInitParam = new OcrInitParam();
        String sdPath = Environment.getExternalStorageDirectory()
                .getAbsolutePath();
		String packageName = this.getPackageName();
        String dataPath = sdPath + File.separator + "sinovoice"
                + File.separator + packageName + File.separator + "data"
                + File.separator;
        
		if (capKey.contains("local")) {
			ocrInitParam.addParam(OcrInitParam.PARAM_KEY_DATA_PATH, dataPath);
			ocrInitParam.addParam(OcrInitParam.PARAM_KEY_INIT_CAP_KEYS, capKey);
			ocrInitParam.addParam(OcrInitParam.PARAM_KEY_FILE_FLAG, "none");
		}
		
		String initParam = ocrInitParam.getStringConfig();
		int captureErrorCode = ocrCapture.hciOcrCaptureInit(getApplicationContext(), initParam, ocrCaptureListener);
		dismissDialog();
		
		//��ʼ���ɹ��������Ҫ����ģ����������̼߳���ģ�壬���سɹ�����ʾ����ͷԤ�����棬����ֱ����ʾ����ͷԤ������
		if(captureErrorCode == CaptureErrCode.CAPTURE_ERR_NONE){
			Log.i(TAG, "hciOcrCaptureInit success.");
			if(isNeedLoadTemplate){
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						//����ģ��
				        String sdPath = Environment.getExternalStorageDirectory().getAbsolutePath();
				        String dataPath = sdPath + File.separator + "sinovoice"
				                + File.separator + "com.sinovoice.example.ocrcapture" + File.separator + "data"
				                + File.separator;
						String templatePath = dataPath + "/templates/IDCard_EN.xml";
						currTemplateId = new OcrTemplateId();
						int errorCode = ocrCapture.hciOcrCaptureLoadTemplate(templatePath, currTemplateId);
						if (errorCode != CaptureErrCode.CAPTURE_ERR_NONE) {
							Log.e(TAG, "hciOcrLoadTemplate() error. errorcode = " + errorCode);
						}else{
							templateId = currTemplateId.getTemplateId();
							recogConfig = "capkey="+ capKey + ",cutedge=no,templateid="+ templateId +",templateIndex=0,templatePageIndex=0";
							hander.post(new Runnable() {
								
								@Override
								public void run() {
									showCaptureView();
								}
							});
						}
					}
				}).start();
			}else{
				showCaptureView();				
			}
		}else{
			Log.e(TAG, "hciOcrCaptureInit fail. captureErrorCode = " + captureErrorCode);
		}
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		
		//�ͷ���������Դ
		if(ocrCapture != null){
			if(isNeedLoadTemplate){
				int captureErrorCode = ocrCapture.hciOcrCaptureUnloadTemplate(currTemplateId);
				Log.v(TAG, "hciOcrCaptureUnloadTemplate(), captureErrorCode = " + captureErrorCode);
			}
			
			int captureErrorCode = ocrCapture.hciOcrCaptureRelease();
			Log.v(TAG, "hciOcrCaptureRelease(), captureErrorCode = " + captureErrorCode);
			
			ocrCapture = null;
		}
		
		mHciCloudSysHelper.release();
	}

	/**
	 * �رնԻ���
	 */
	private void dismissDialog() {
		if(pDialog != null && pDialog.isShowing()){
			pDialog.dismiss();
			pDialog = null;
		}
	}
	
	/**
	 * ��ʾ����ͷԤ������
	 */
	private void showCaptureView(){
		setContentView(R.layout.ocr_capture_camera_preview);
		
		if(cameraPreviewLayout != null){
			cameraPreviewLayout.removeAllViews();
			cameraPreviewLayout = null;
		}
		
		cameraPreviewLayout = (FrameLayout) findViewById(R.id.layout_camera_preview);
		cameraPreviewLayout.addView(ocrCapture.getCameraPreview());
		
		Button btnTakePicture = (Button) findViewById(R.id.btn_take_picture);
		btnTakePicture.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View v) {
				ocrCapture.hciOcrCaptureStopAndRecog();
			}
		});
		
		ocrCapture.hciOcrCaptureStart(recogConfig);
	}
	
	private void showMainView(){
		showResultView("��ʼ״̬");
	}
	
	/**
	 * ��ʾ�������
	 * @param text
	 */
	private void showResultView(String text){
		if(cameraPreviewLayout != null){
			cameraPreviewLayout.removeAllViews();
			cameraPreviewLayout = null;
		}
		
		setContentView(R.layout.ocr_capture_result);
		
		TextView tvResult = (TextView) findViewById(R.id.tv_result);
		tvResult.setText(text);
	}
	
}
