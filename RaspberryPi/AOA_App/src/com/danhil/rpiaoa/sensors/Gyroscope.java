package com.danhil.rpiaoa.sensors;


import java.io.FileOutputStream;
import java.util.ArrayList;


import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.danhil.rpiaoa.observers.GyroscopeObserver;

public class Gyroscope implements SensorEventListener
{

	private static final String tag = Gyroscope.class.getSimpleName();
	private ArrayList<GyroscopeObserver> observersGyroscope;
	private Context context;
	private SensorManager sensorManager;
	private float[] gyroscope = new float[3];
	// Keep track of the moest recent event.
	private long timeStamp = 0;
	// ROtate from normal android orientation to actual orientation
	// ROtation vectors
	private FileOutputStream fOut;

	public Gyroscope(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		observersGyroscope = new ArrayList<GyroscopeObserver>();

		sensorManager = (SensorManager) this.context
				.getSystemService(Context.SENSOR_SERVICE);

		this.fOut = fOut;
	}

	public void registerGyroscopeObserver(GyroscopeObserver observer)
	{
		if (observersGyroscope.size() == 0)
		{
			sensorManager.registerListener(this,
					sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE),
					SensorManager.SENSOR_DELAY_NORMAL);
		}

		int i = observersGyroscope.indexOf(observer);
		if (i == -1)
		{
			observersGyroscope.add(observer);
		}
	}

	public void removeGyroscopeObserver(GyroscopeObserver observer)
	{
		int i = observersGyroscope.indexOf(observer);
		if (i >= 0)
		{
			observersGyroscope.remove(i);
		}

		// If there are no observers, then don't listen for Sensor Events.
		if (observersGyroscope.size() == 0)
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
		if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE)
		{
			System.arraycopy(event.values, 0, this.gyroscope, 0,
					event.values.length);

			this.timeStamp = event.timestamp;

			notifyGyroscopeObserver();
		}
	}



	private void notifyGyroscopeObserver()
	{
		for (GyroscopeObserver a : observersGyroscope)
		{
			a.onGyroscopeSensorChanged(this.gyroscope, this.timeStamp, fOut);
		}
	}

}