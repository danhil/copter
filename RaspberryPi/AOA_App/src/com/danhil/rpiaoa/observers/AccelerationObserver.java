package com.danhil.rpiaoa.observers;

import java.io.FileOutputStream;

public interface AccelerationObserver
{
        public void onAccelerationSensorChanged(float[] acceleration,
                        long timeStamp, FileOutputStream out);
}