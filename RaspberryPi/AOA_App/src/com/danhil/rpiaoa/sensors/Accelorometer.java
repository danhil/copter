package com.danhil.rpiaoa.sensors;

import java.io.FileOutputStream;
import java.util.ArrayList;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.danhil.rpiaoa.observers.AccelerationObserver;

public class Accelorometer implements SensorEventListener
{ 
	private static final String tag = Accelorometer.class.getSimpleName();

	// Keep track of observers.
	private ArrayList<AccelerationObserver> observersAcceleration;


	private Context context;
	private float[] acceleration = new float[3];
	private long timeStamp = 0;
	private SensorManager sensorManager;
	private FileOutputStream fOut;

	public Accelorometer(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;
		this.fOut = fOut;

		observersAcceleration = new ArrayList<AccelerationObserver>();

		sensorManager = (SensorManager) this.context
				.getSystemService(Context.SENSOR_SERVICE);
	}

	public void registerAccelerationObserver(AccelerationObserver observer)
	{
		// If there are currently no observers, but one has just requested to be
		// registered, register to listen for sensor events from the device.
		if (observersAcceleration.size() == 0)
		{
			sensorManager.registerListener(this,
					sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
					SensorManager.SENSOR_DELAY_FASTEST);
		}

		// Only register the observer if it is not already registered.
		int i = observersAcceleration.indexOf(observer);
		if (i == -1)
		{
			observersAcceleration.add(observer);
		}

	}

	public void removeAccelerationObserver(AccelerationObserver observer)
	{
		int i = observersAcceleration.indexOf(observer);
		if (i >= 0)
		{
			observersAcceleration.remove(i);
		}

		// If there are no observers, then don't listen for Sensor Events.
		if (observersAcceleration.size() == 0)
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
		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER)
		{
			System.arraycopy(event.values, 0, acceleration, 0,
					event.values.length);

			timeStamp = event.timestamp;

			notifyAccelerationObserver();
		}
	}

	private void notifyAccelerationObserver()
	{
		for (AccelerationObserver a : observersAcceleration)
		{
			a.onAccelerationSensorChanged(this.acceleration, this.timeStamp, fOut);
		}
	}
}