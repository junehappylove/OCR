package com.sinovoice.example.ocr;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.sinovoice.example.sys.AccountInfo;
import com.sinovoice.example.sys.HciCloudSysHelper;
import com.sinovoice.hcicloudsdk.common.HciErrorCode;

/**
 * 本示例演示了灵云Ocr能力调用的基本步骤, 流程: 系统初始化 , 获取授权/更新授权文件, OCR引擎初始化, OCR合成调用, OCR反初始化,
 * 系统反初始化
 * 
 * @author sinovoice
 */
public class HciCloudOcrActivity extends Activity {
	private static final String TAG = "HciCloudOcrActivity";

	/**
	 * 加载用户信息工具类
	 */
	private AccountInfo mAccountInfo;

	/**
	 * HciCloud帮助类，可完成灵云系统初始化，释放操作。
	 */
	private HciCloudSysHelper mHciCloudSysHelper;

	/**
	 * 展示引擎返回信息的TextView
	 */
	private TextView mLogView;

	/**
	 * Ocr帮助类， 可完成Ocr能力的初始化，使用，释放操作。
	 */
	private HciCloudOcrHelper mHciCloudOcrHelper;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		mLogView = (TextView) findViewById(R.id.logview);
		
		mAccountInfo = AccountInfo.getInstance();
		boolean loadResult = mAccountInfo.loadAccountInfo(this);
		if (loadResult) {
			// 加载信息成功进入主界面
			mLogView.setText("加载灵云账号成功\n");
		} else {
			// 加载信息失败，显示失败界面
			mLogView.setText("加载灵云账号失败！请在assets/AccountInfo.txt文件中填写正确的灵云账户信息，账户需要从www.hcicloud.com开发者社区上注册申请。");
			return;
		}

		mHciCloudSysHelper = HciCloudSysHelper.getInstance();
		mHciCloudOcrHelper = HciCloudOcrHelper.getInstance();

		// 此方法是线程阻塞的，当且仅当有结果返回才会继续向下执行。
		// 此处只是演示合成能力用法，没有对耗时操作进行处理。需要开发者放入后台线程进行初始化操作
		// 必须首先调用HciCloudSys的初始化方法
		int sysInitResult = mHciCloudSysHelper.init(this);
		if (sysInitResult != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "hci init error, error code = " + sysInitResult);

			mLogView.append("hci init error, error code = " + sysInitResult);
			return;
		}
		mLogView.append(" 系统初始化成功！\n");

		// 此方法是线程阻塞的，当且仅当有结果返回才会继续向下执行。
		// 此处只是演示合成能力用法，没有对耗时操作进行处理。需要开发者放入后台线程进行初始化操作
		// 只有HciCloudSys初始化成功后，才能调用ocr的初始化方法
		int ocrInitResult = mHciCloudOcrHelper.init(this);
		if (ocrInitResult != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "ocr init error " + ocrInitResult);

			mLogView.append("ocr init error " + ocrInitResult);
			return;
		}
		mLogView.append(" ocr初始化成功！\n");	
		
		boolean nRet = true;
		String capkeyString = mAccountInfo.getCapKey();
		
		if (capkeyString.contains("bizcard")) {
			nRet = mHciCloudOcrHelper.bizcardRecog("BizCard.jpg", this); // 名片识别
			if(nRet) {
				mLogView.append(" bizcardRecog success！\n");	
			} else {
				mLogView.append(" bizcardRecog failed！\n");
			}
		} else if (capkeyString.contains("template")) {
			// 本地模板识别
			if(capkeyString.contains("local")) {
				nRet = mHciCloudOcrHelper.templateRecog("IDCard.jpg", this);// 本地模板识别
				if(nRet) {
					mLogView.append("local templateRecog success！\n");	
				} else {
					mLogView.append("local templateRecog failed！\n");
				}				
			}
			
			// 云端模板识别
			if(capkeyString.contains("cloud")) {
				nRet = mHciCloudOcrHelper.autoRecog("IDCard.jpg", this);		//云端模板识别
				if(nRet) {
					mLogView.append("cloud templateRecog success！\n");	
				} else {
					mLogView.append("cloud templateRecog failed！\n");
				}	
			}

		} else {
			nRet = mHciCloudOcrHelper.autoRecog("ocr.jpg", this); // 自动版面分析和倾斜校正
			if(nRet) {
				mLogView.append(" autoRecog success！\n");	
			} else {
				mLogView.append(" autoRecog failed！\n");
			}
			
			nRet = mHciCloudOcrHelper.advanceRecog("ocr.jpg", this); // 手动决定版面分析和倾斜校正
			if(nRet) {
				mLogView.append(" advanceRecog success！\n");	
			} else {
				mLogView.append(" advanceRecog failed！\n");
			}
		}
		
	}

	@Override
	protected void onDestroy() {
		// 释放ocr能力，优先释放ocr
		int nRet = mHciCloudOcrHelper.release();
		mLogView.append("ocr release! result = " + nRet);

		// 释放HciCloudSys，当其他能力全部释放完毕后，才能调用HciCloudSys的释放方法
		mHciCloudSysHelper.release();
		super.onDestroy();
	}
}
