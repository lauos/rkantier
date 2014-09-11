package com.rootkit.anti;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import com.rootkit.anti.ShellUtils.CommandResult;

import android.R.bool;
import android.content.Context;
import android.util.Log;

public class Util {
	
	private static String tag = "ANTI-RK";	
	private Context ctx;
	
	public Util(Context context) {
		this.ctx = context;
	}
	
	public byte[] getFromAssets(String filename){
		byte[] buffer = null;
		try {
			InputStream in = this.ctx.getResources().getAssets().open(filename);
			
			int len = in.available();
			buffer = new byte[len];
			in.read(buffer);
			in.close();
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return buffer;
	}
	
	public boolean writeBinary(byte[] buffer, String path){
		File binaFile = new File(path);
		
		if(!binaFile.exists()){
			try {
				binaFile.createNewFile();
				FileOutputStream out = new FileOutputStream(binaFile);
				out.write(buffer);
				out.close();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}else {
			return false;
		}
		
		return true;
	}
	
	public static boolean checkStart(){
		CommandResult result = ShellUtils.execCommand("lsmod", false);
		
		if(result.result == 0){
			Log.d(tag, result.successMsg);
			if(result.successMsg.contains("rootkit"))
			{
				return true;
			}
			else {
				return false;
			}
		}else{
			Log.d(tag, result.errorMsg);
			return false;
		}
	}
}
