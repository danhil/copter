package com.danhil.rpiaoa.sensors;


import java.io.FileOutputStream;
import java.util.ArrayList;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.danhil.rpiaoa.observers.RotationVectorObserver;

public class RotationVector implements SensorEventListener
{

	private static final String tag = RotationVector.class.getSimpleName();
	private ArrayList<RotationVectorObserver> observersRotationVector;
	private Context context;
	private SensorManager sensorManager;
	private float[] RotationVector = new float[5];
	// Keep track of the most recent event.
	private long timeStamp = 0;
	private FileOutputStream fOut;

	public RotationVector(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		observersRotationVector = new ArrayList<RotationVectorObserver>();

		sensorManager = (SensorManager) this.context
				.getSystemService(Context.SENSOR_SERVICE);

		this.fOut = fOut;
	}

	public void registerRotationVectorObserver(RotationVectorObserver observer)
	{
		if (observersRotationVector.size() == 0)
		{
			sensorManager.registerListener(this,
					sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR),
					SensorManager.SENSOR_DELAY_FASTEST);
		}

		int i = observersRotationVector.indexOf(observer);
		if (i == -1)
		{
			observersRotationVector.add(observer);
		}
	}

	public void removeRotationVectorObserver(RotationVectorObserver observer)
	{
		int i = observersRotationVector.indexOf(observer);
		if (i >= 0)
		{
			observersRotationVector.remove(i);
		}

		// If there are no observers, then don't listen for Sensor Events.
		if (observersRotationVector.size() == 0)
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
		if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR)
		{
			System.arraycopy(event.values, 0, this.RotationVector, 0,
					event.values.length);

			this.timeStamp = event.timestamp;


			notifyRotationVectorObserver();
		}
	}

	private void notifyRotationVectorObserver()
	{
		for (RotationVectorObserver a : observersRotationVector)
		{
			a.onRotationVectorSensorChanged(this.RotationVector, this.timeStamp, fOut);
		}
	}

}