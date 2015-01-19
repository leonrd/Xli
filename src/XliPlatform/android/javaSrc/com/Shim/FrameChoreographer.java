package com.Shim;

import android.annotation.TargetApi;
import android.os.Build;
import android.view.Choreographer;

@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
public class FrameChoreographer implements Choreographer.FrameCallback {

	@Override
	public void doFrame(long frameTimeNanos) {
		com.Shim.XliJ.XliJ_FrameTick();
	}
}
