package org.libsdl.nanovg;
import java.io.File;

import org.libsdl.app.SDLActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

public class nanovgActivity extends SDLActivity {
	String TAG = "nanovg";
	@Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "luajit",
            "main",
        };
    }	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	Log.w(TAG,String.format("getFilesDir : %s",	getFilesDir().getPath()));
    	Log.w(TAG,String.format("getCacheDir : %s",getCacheDir().getPath()));
    	Log.w(TAG,String.format("getExternalCacheDir : %s",getExternalCacheDir().getPath()));
    	
    	Log.w(TAG,String.format("getDataDirectory : %s",Environment.getDataDirectory().getPath()));
    	Log.w(TAG,String.format("getDownloadCacheDirectory : %s",Environment.getDownloadCacheDirectory().getPath()));
    	Log.w(TAG,String.format("getExternalStorageDirectory : %s",Environment.getExternalStorageDirectory().getPath()));
    	Log.w(TAG,String.format("getExternalStoragePublicDirectory(DIRECTORY_PICTURES) : %s",
    			Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).getPath()));
    	Log.w(TAG,String.format("getExternalStoragePublicDirectory(DIRECTORY_DOWNLOADS) : %s",
    			Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).getPath()));    	
    	Log.w(TAG,String.format("getRootDirectory : %s",Environment.getRootDirectory().getPath()));
    	Log.w(TAG,String.format("getExternalStorageState : %s",Environment.getExternalStorageState()));
    	Log.w(TAG,String.format("isExternalStorageEmulated : %s",Environment.isExternalStorageEmulated()?"true":"false"));
    	Log.w(TAG,String.format("isExternalStorageRemovable : %s",Environment.isExternalStorageRemovable()?"true":"false"));
    /*
     * ��װһ��apk	
    	Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setDataAndType(Uri.fromFile(new File
                (Environment.getExternalStorageDirectory()  + "/ljdata/share/download/http/edengine_16_0110.apk")), "application/vnd.android.package-archive");
        startActivity(intent);
    */     	
    	super.onCreate(savedInstanceState);
    }
}
