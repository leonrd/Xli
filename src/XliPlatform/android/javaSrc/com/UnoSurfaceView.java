package com.Untitled;

import android.app.NativeActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class UnoSurfaceView extends SurfaceView implements SurfaceHolder.Callback
{
    NativeActivity nativeActivity;
    
	public UnoSurfaceView(NativeActivity context) {
		super(context);
        nativeActivity = context;
        getHolder().addCallback(this);
        setZOrderOnTop(false);
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.d("XliApp","surfaceCreated");
        com.Shim.XliJ.RootSurfaceChanged(holder.getSurface());
        //nativeActivity.surfaceCreated(holder);
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		Log.d("XliApp","surfaceChanged");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.d("XliApp","surfaceDestroyed");		
	}
}
