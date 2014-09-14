package com.rootkit.anti;

import java.util.HashMap;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Toast;

public class DetectingActivity extends Activity {
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_detecting);
		
		InitApplication app = (InitApplication) getApplication();
		HashMap<String, Integer> config = app.getConfig();
		
		OnClickListener ok = new OnClickListener() {
			
			@Override
			public void onClick(DialogInterface arg0, int arg1) {
				// TODO Auto-generated method stub
				
				if(NativeFuncs.resetSystem())
				{
					Toast.makeText(getApplicationContext(), "系统恢复成功", Toast.LENGTH_LONG).show();
				}else {
					Toast.makeText(getApplicationContext(), "系统恢复失败", Toast.LENGTH_LONG).show();
				}
			}
		};
		String result = NativeFuncs.startDetection(config.toString());
		
		if(result != null)
		{
			Log.d("ANTI-RK", result);
			new AlertDialog.Builder(this)
			.setTitle("警告")
			.setMessage("发现内核级Rootkit威胁，是否尝试对系统进行恢复？")
			.setPositiveButton("确定", ok)
			.setNegativeButton("取消", null)
			.show();
		}
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
