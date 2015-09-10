package com.sinovoice.example.ocr;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.sinovoice.example.sys.AccountInfo;
import com.sinovoice.example.sys.HciCloudSysHelper;
import com.sinovoice.hcicloudsdk.common.HciErrorCode;

/**
 * ��ʾ����ʾ������Ocr�������õĻ�������, ����: ϵͳ��ʼ�� , ��ȡ��Ȩ/������Ȩ�ļ�, OCR�����ʼ��, OCR�ϳɵ���, OCR����ʼ��,
 * ϵͳ����ʼ��
 * 
 * @author sinovoice
 */
public class HciCloudOcrActivity extends Activity {
	private static final String TAG = "HciCloudOcrActivity";

	/**
	 * �����û���Ϣ������
	 */
	private AccountInfo mAccountInfo;

	/**
	 * HciCloud�����࣬���������ϵͳ��ʼ�����ͷŲ�����
	 */
	private HciCloudSysHelper mHciCloudSysHelper;

	/**
	 * չʾ���淵����Ϣ��TextView
	 */
	private TextView mLogView;

	/**
	 * Ocr�����࣬ �����Ocr�����ĳ�ʼ����ʹ�ã��ͷŲ�����
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
			// ������Ϣ�ɹ�����������
			mLogView.setText("���������˺ųɹ�\n");
		} else {
			// ������Ϣʧ�ܣ���ʾʧ�ܽ���
			mLogView.setText("���������˺�ʧ�ܣ�����assets/AccountInfo.txt�ļ�����д��ȷ�������˻���Ϣ���˻���Ҫ��www.hcicloud.com������������ע�����롣");
			return;
		}

		mHciCloudSysHelper = HciCloudSysHelper.getInstance();
		mHciCloudOcrHelper = HciCloudOcrHelper.getInstance();

		// �˷������߳������ģ����ҽ����н�����زŻ��������ִ�С�
		// �˴�ֻ����ʾ�ϳ������÷���û�жԺ�ʱ�������д�����Ҫ�����߷����̨�߳̽��г�ʼ������
		// �������ȵ���HciCloudSys�ĳ�ʼ������
		int sysInitResult = mHciCloudSysHelper.init(this);
		if (sysInitResult != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "hci init error, error code = " + sysInitResult);

			mLogView.append("hci init error, error code = " + sysInitResult);
			return;
		}
		mLogView.append(" ϵͳ��ʼ���ɹ���\n");

		// �˷������߳������ģ����ҽ����н�����زŻ��������ִ�С�
		// �˴�ֻ����ʾ�ϳ������÷���û�жԺ�ʱ�������д�����Ҫ�����߷����̨�߳̽��г�ʼ������
		// ֻ��HciCloudSys��ʼ���ɹ��󣬲��ܵ���ocr�ĳ�ʼ������
		int ocrInitResult = mHciCloudOcrHelper.init(this);
		if (ocrInitResult != HciErrorCode.HCI_ERR_NONE) {
			Log.e(TAG, "ocr init error " + ocrInitResult);

			mLogView.append("ocr init error " + ocrInitResult);
			return;
		}
		mLogView.append(" ocr��ʼ���ɹ���\n");	
		
		boolean nRet = true;
		String capkeyString = mAccountInfo.getCapKey();
		
		if (capkeyString.contains("bizcard")) {
			nRet = mHciCloudOcrHelper.bizcardRecog("BizCard.jpg", this); // ��Ƭʶ��
			if(nRet) {
				mLogView.append(" bizcardRecog success��\n");	
			} else {
				mLogView.append(" bizcardRecog failed��\n");
			}
		} else if (capkeyString.contains("template")) {
			// ����ģ��ʶ��
			if(capkeyString.contains("local")) {
				nRet = mHciCloudOcrHelper.templateRecog("IDCard.jpg", this);// ����ģ��ʶ��
				if(nRet) {
					mLogView.append("local templateRecog success��\n");	
				} else {
					mLogView.append("local templateRecog failed��\n");
				}				
			}
			
			// �ƶ�ģ��ʶ��
			if(capkeyString.contains("cloud")) {
				nRet = mHciCloudOcrHelper.autoRecog("IDCard.jpg", this);		//�ƶ�ģ��ʶ��
				if(nRet) {
					mLogView.append("cloud templateRecog success��\n");	
				} else {
					mLogView.append("cloud templateRecog failed��\n");
				}	
			}

		} else {
			nRet = mHciCloudOcrHelper.autoRecog("ocr.jpg", this); // �Զ������������бУ��
			if(nRet) {
				mLogView.append(" autoRecog success��\n");	
			} else {
				mLogView.append(" autoRecog failed��\n");
			}
			
			nRet = mHciCloudOcrHelper.advanceRecog("ocr.jpg", this); // �ֶ����������������бУ��
			if(nRet) {
				mLogView.append(" advanceRecog success��\n");	
			} else {
				mLogView.append(" advanceRecog failed��\n");
			}
		}
		
	}

	@Override
	protected void onDestroy() {
		// �ͷ�ocr�����������ͷ�ocr
		int nRet = mHciCloudOcrHelper.release();
		mLogView.append("ocr release! result = " + nRet);

		// �ͷ�HciCloudSys������������ȫ���ͷ���Ϻ󣬲��ܵ���HciCloudSys���ͷŷ���
		mHciCloudSysHelper.release();
		super.onDestroy();
	}
}
