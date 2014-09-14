package com.rootkit.anti;

import java.util.HashMap;

import android.app.Application;
import android.content.res.Configuration;

public class InitApplication extends Application {
	
	HashMap<String, Integer> config;
	
	
	public void setConfig(HashMap<String, Integer> cfg){
		this.config = cfg;
	}
	
	public HashMap<String, Integer> getConfig(){
		return this.config;
	}
	
	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
	}
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		super.onConfigurationChanged(newConfig);
	}
	
	@Override
	public void onTerminate() {
		// TODO Auto-generated method stub
		super.onTerminate();
	}

	@Override
	public void onLowMemory() {
		// TODO Auto-generated method stub
		super.onLowMemory();
	}
	
	@Override
	public void onTrimMemory(int level) {
		// TODO Auto-generated method stub
		super.onTrimMemory(level);
	}
}
