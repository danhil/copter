package com.danhil.rpiaoa.observers;

import java.io.FileOutputStream;

public interface GyroscopeObserver
{
		// Notify all observers.
        public void onGyroscopeSensorChanged(float[] gyroscope, long timeStamp, FileOutputStream out);
}