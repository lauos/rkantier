package com.rootkit.anti;

import com.rootkit.anti.ShellUtils.CommandResult;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {
	private String tag = "ANTI-RK";
	private String MOD = "antirk.ko";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		Button button = (Button)findViewById(R.id.enableButton);
		button.setOnClickListener(new View.OnClickListener() {
			
		Util util = new Util(getApplicationContext());
		String dataDir = getApplication().getApplicationInfo().dataDir;
			
			@Override
			public void onClick(View arg0) {
				util.writeBinary(util.getFromAssets(MOD), dataDir + "/" + MOD);
				
				CommandResult result = ShellUtils.execCommand("ls -l " + dataDir, false);
				
				if(result.result == 0){
					Log.d(tag, result.successMsg);
				}else{
					Log.d(tag, result.errorMsg);
				}
				
				if(Util.checkStart()){
					Toast.makeText(getApplicationContext(), "已经开启内核级Rootkit检测功能,进入设置界面", Toast.LENGTH_SHORT).show();
					Intent intent = new Intent(getApplicationContext(), SettingActivity.class);
					startActivity(intent);
				}
				else {
					if(!ShellUtils.checkRootPermission())
					{
						Toast.makeText(getApplicationContext(), "未能获取Root权限，不能开启Rootkit检测功能", Toast.LENGTH_SHORT).show();
					}else{
						if(NativeFuncs.enableMod(getApplication().getApplicationInfo())){
							Toast.makeText(getApplicationContext(), "已经开启内核级Rootkit检测功能,进入设置界面", Toast.LENGTH_SHORT).show();
							
							Intent intent = new Intent(getApplicationContext(), SettingActivity.class);
							startActivity(intent);
						}
					}
/*				
					result = ShellUtils.execCommand("ls -l " + dataDir, false);
				
					if(result.result == 0){
						Log.d(tag, result.successMsg);
					}else{
						Log.d(tag, result.errorMsg);
					}*/
				}
			}
		});
		
		button = (Button)findViewById(R.id.disableButton);
		button.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				
				CommandResult result = ShellUtils.execCommand("rmmod " + "antirk", true);
				
				if(result.result == 0){
					Log.d(tag, result.successMsg);
				}else{
					Log.d(tag, result.errorMsg);
				}
				if(Util.checkStart()){
					Toast.makeText(getApplicationContext(), "卸载内核模块失败，请重启手机", Toast.LENGTH_SHORT).show();
				}else {
					Toast.makeText(getApplicationContext(), "卸载内核模块成功", Toast.LENGTH_SHORT).show();					
				}
				NativeFuncs.disableMod();	
				
			}
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
