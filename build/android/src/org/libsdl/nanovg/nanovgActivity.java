package org.libsdl.nanovg;
import org.libsdl.app.SDLActivity;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

public class nanovgActivity extends SDLActivity {
	String TAG = "nanovg";
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
    	super.onCreate(savedInstanceState);
    }
}
