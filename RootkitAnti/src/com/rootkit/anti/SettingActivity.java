package com.rootkit.anti;

import java.util.HashMap;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Toast;

public class SettingActivity extends Activity {
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_setting);
		
		Button button = (Button)findViewById(R.id.startbutton);
		button.setOnClickListener(new View.OnClickListener() {
			
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				
				EditText edittext = (EditText) findViewById(R.id.editText1); 
				CharSequence time = edittext.getText();
			
				CheckBox checkBox1 = (CheckBox)findViewById(R.id.checkBox1);
				CheckBox checkBox2 = (CheckBox)findViewById(R.id.checkBox2);
				CheckBox checkBox3 = (CheckBox)findViewById(R.id.checkBox3);
				CheckBox checkBox4 = (CheckBox)findViewById(R.id.checkBox4);
				CheckBox checkBox5 = (CheckBox)findViewById(R.id.checkBox5);

				
				HashMap<String, Integer> config = new HashMap<String, Integer>(TRIM_MEMORY_COMPLETE);
				if(checkBox1.isChecked())
				{
					config.put("isAutoDetect", Integer.valueOf(time.toString()));
				}else {
					config.put("isAutoDetect", 0);
				}
				if(checkBox2.isChecked())
				{
					config.put("isException", 1);
				}else {
					config.put("isException", 0);
				}
				if(checkBox3.isChecked())
				{
					config.put("isSysCall", 1);
				}else {
					config.put("isSysCall", 0);
				}
				if(checkBox4.isChecked())
				{
					config.put("isModule", 1);
				}else {
					config.put("isModule", 0);
				}
				if(checkBox5.isChecked())
				{
					config.put("isVFS", 1);
				}else {
					config.put("isVFS", 0);
				}
				
				InitApplication app = (InitApplication) getApplication();
				app.setConfig(config);
				
				Toast.makeText(getApplicationContext(), config.toString(), Toast.LENGTH_LONG).show();
				
				Intent intent = new Intent(getApplicationContext(), DetectingActivity.class);
				startActivity(intent);
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
