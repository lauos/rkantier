package com.rootkit.anti;

import android.content.pm.ApplicationInfo;

public class NativeFuncs {
	
	public NativeFuncs() {
		
	}
	
	static native boolean enableMod(ApplicationInfo appInfo);
	
	static native boolean disableMod();
	
	static native String startDetection(String config);
	
	static native boolean resetSystem();
	
	static{
		System.loadLibrary("native");
	}
}
