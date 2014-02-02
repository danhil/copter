package com.danhil.rpiaoa.sensors;


import java.io.FileOutputStream;
import java.util.ArrayList;


import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.danhil.rpiaoa.observers.OrientationObserver;

public class Orientation implements SensorEventListener
{

	private static final String tag = Orientation.class.getSimpleName();
	private ArrayList<OrientationObserver> observersOrientation;
	private Context context;
	private SensorManager sensorManager;
	private float[] Orientation = new float[3];
	// Keep track of the moest recent event.
	private long timeStamp = 0;
	// ROtate from normal android orientation to actual orientation
	// ROtation vectors
	private FileOutputStream fOut;
	
	/**
	 * This sensor is depreciated as it drifts to much, this implementation is for 
	 * benchmarking purposes only. NOT to be used later.
	 * @param context
	 * @param fOut
	 */

	public Orientation(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		observersOrientation = new ArrayList<OrientationObserver>();

		sensorManager = (SensorManager) this.context
				.getSystemService(Context.SENSOR_SERVICE);

		this.fOut = fOut;
	}

	public void registerOrientationObserver(OrientationObserver observer)
	{
		if (observersOrientation.size() == 0)
		{
			sensorManager.registerListener(this,
					sensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION),
					SensorManager.SENSOR_DELAY_NORMAL);
		}

		int i = observersOrientation.indexOf(observer);
		if (i == -1)
		{
			observersOrientation.add(observer);
		}
	}

	public void removeOrientationObserver(OrientationObserver observer)
	{
		int i = observersOrientation.indexOf(observer);
		if (i >= 0)
		{
			observersOrientation.remove(i);
		}

		// If there are no observers, then don't listen for Sensor Events.
		if (observersOrientation.size() == 0)
		{
			sensorManager.unregisterListener(this);
		}
	}


	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy)
	{
		// Do nothing.
	}

	@Override
	public void onSensorChanged(SensorEvent event)
	{
		if (event.sensor.getType() == Sensor.TYPE_ORIENTATION)
		{
			System.arraycopy(event.values, 0, this.Orientation, 0,
					event.values.length);

			this.timeStamp = event.timestamp;

			notifyOrientationObserver();
		}
	}



	private void notifyOrientationObserver()
	{
		for (OrientationObserver a : observersOrientation)
		{
			a.onOrientationSensorChanged(this.Orientation, this.timeStamp, fOut);
		}
	}

}