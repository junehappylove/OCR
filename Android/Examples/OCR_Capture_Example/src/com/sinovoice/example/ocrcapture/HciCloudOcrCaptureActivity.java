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
     * 加载用户信息工具类
     */
    private AccountInfo mAccountInfo;

    /**
     * HciCloud帮助类，可完成灵云系统初始化，释放操作。
     */
    private HciCloudSysHelper mHciCloudSysHelper;    
	
//	private HciCloudOcrCaptureActivity activity;
//	private Context context;
	private Handler hander;
	private String capKey;
	
	//显示摄像头预览视图的布局文件
	private FrameLayout cameraPreviewLayout;
	private ProgressDialog pDialog;
	
	//识别配置参数
	private String recogConfig;
	//是否需要加载模板文件
	private boolean isNeedLoadTemplate;
	//模板文件相关
	private OcrTemplateId currTemplateId;
	private int templateId;
	
	//拍照器模块
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
			
			//文档识别
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
		
		//设置屏幕显示方向
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);			
		
		//呈现默认布局
		showMainView();
	
		hander = new Handler();		
		
		pDialog = ProgressDialog.show(this, getText(R.string.dialog_title_tips), getText(R.string.dialog_msg_hcicloud_sysinit));
				
        mAccountInfo = AccountInfo.getInstance();
        boolean loadResult = mAccountInfo.loadAccountInfo(this);
        if (loadResult) {
            // 加载信息成功进入主界面
			Toast.makeText(getApplicationContext(), "加载灵云账号成功",
					Toast.LENGTH_SHORT).show();
        } else {
            // 加载信息失败，显示失败界面
			Toast.makeText(getApplicationContext(), "加载灵云账号失败！请在assets/AccountInfo.txt文件中填写正确的灵云账户信息，账户需要从www.hcicloud.com开发者社区上注册申请。",
					Toast.LENGTH_SHORT).show();
            return;
        }

        mHciCloudSysHelper = HciCloudSysHelper.getInstance();

        // 此方法是线程阻塞的，当且仅当有结果返回才会继续向下执行。
        // 此处只是演示合成能力用法，没有对耗时操作进行处理。需要开发者放入后台线程进行初始化操作
        // 必须首先调用HciCloudSys的初始化方法
        int sysInitResult = mHciCloudSysHelper.init(this);
        if (sysInitResult != HciErrorCode.HCI_ERR_NONE) {
            Log.e(TAG, "hci init error, error code = " + sysInitResult);
            return;
        }
        
		// 读取用户的调用的能力
		capKey = mAccountInfo.getCapKey();
				
		if(capKey.equalsIgnoreCase("ocr.cloud")
				|| capKey.equalsIgnoreCase("ocr.cloud.english")
				|| capKey.equalsIgnoreCase("ocr.local")){
			//文档识别
			recogConfig = "capKey=" + capKey;
		}else if(capKey.equalsIgnoreCase("ocr.local.bizcard.v6")){
			//名片识别
			recogConfig = "capkey=ocr.local.bizcard.v6,cutedge=yes";
		}else if(capKey.equalsIgnoreCase("ocr.local.template.v6")){
			//模板识别
			isNeedLoadTemplate = true;
		}else{
			Log.e(TAG, "未知的capKey。 capKey = " + capKey);
		}				
		
		ocrCapture = new OCRCapture();
		initOCRCapture();
		dismissDialog();											
	}

	/**
	 * 初始化OCR模块 
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
		
		//初始化成功，如果需要加载模板就启动新线程加载模板，加载成功后显示摄像头预览界面，否则直接显示摄像头预览界面
		if(captureErrorCode == CaptureErrCode.CAPTURE_ERR_NONE){
			Log.i(TAG, "hciOcrCaptureInit success.");
			if(isNeedLoadTemplate){
				new Thread(new Runnable() {
					
					@Override
					public void run() {
						//载入模板
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
		
		//释放拍照器资源
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
	 * 关闭对话框
	 */
	private void dismissDialog() {
		if(pDialog != null && pDialog.isShowing()){
			pDialog.dismiss();
			pDialog = null;
		}
	}
	
	/**
	 * 显示摄像头预览界面
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
		showResultView("初始状态");
	}
	
	/**
	 * 显示结果界面
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
