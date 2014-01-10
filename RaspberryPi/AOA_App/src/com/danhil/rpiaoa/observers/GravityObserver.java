package com.danhil.rpiaoa.observers;

import java.io.FileOutputStream;

public interface GravityObserver
{
        public void onGravitySensorChanged(float[] gravity,
                        long timeStamp, FileOutputStream out);
}