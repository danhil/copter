package com.danhil.rpiaoa.observers;

import java.io.FileOutputStream;

public interface RotationVectorObserver {
	
	 public void onRotationVectorSensorChanged(float[] rotation,
             long timeStamp, FileOutputStream out);
}
