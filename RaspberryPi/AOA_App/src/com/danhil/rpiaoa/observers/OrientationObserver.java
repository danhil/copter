package com.danhil.rpiaoa.observers;

import java.io.FileOutputStream;

public interface OrientationObserver {
	   public void onOrientationSensorChanged(float[] orientation, long timeStamp, FileOutputStream out);
}
