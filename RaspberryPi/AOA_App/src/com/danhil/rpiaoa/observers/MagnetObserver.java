package com.danhil.rpiaoa.observers;

import java.io.FileOutputStream;

public interface MagnetObserver
{
        public void onMagneticSensorChanged(float[] magnetic, long timeStamp, FileOutputStream out);
}