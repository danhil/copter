package com.danhil.rpiaoa.observers;

	public interface SensorsObserver
	{
	        public void onSensorsChanged(float[] linearAcceleration,
	                        long timeStamp);
	}