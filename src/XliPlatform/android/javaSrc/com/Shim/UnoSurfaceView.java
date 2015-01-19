package com.Shim;

import android.app.NativeActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class UnoSurfaceView extends SurfaceView implements SurfaceHolder.Callback
{
    NativeActivity nativeActivity;
    int width = 0;
    int height = 0;
    
	public UnoSurfaceView(NativeActivity context) {
		super(context);
        nativeActivity = context;
        getHolder().addCallback(this);
        setZOrderOnTop(false);
	}

	public void updateDimensions(int w, int h) {
		if (width!=w || height!=h) {
			width=w;
			height=h;
			com.Shim.XliJ.RootSurfaceChangedDimensions(w, h);
		}
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.d("XliApp","surfaceCreated");
        com.Shim.XliJ.RootSurfaceChanged(holder.getSurface());
        updateDimensions(getWidth(), getHeight());
        //nativeActivity.surfaceCreated(holder);
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		Log.d("XliApp","surfaceChanged");
		updateDimensions(width, height);
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.d("XliApp","surfaceDestroyed");		
	}
}
