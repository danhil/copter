package com.danhil.rpiaoa.sensors;

import java.io.FileOutputStream;
import java.util.ArrayList;


import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.danhil.rpiaoa.observers.MagnetObserver;

public class Magnet implements SensorEventListener
{

	private static final String tag = Magnet.class.getSimpleName();

	// Keep track of observers.
	private ArrayList<MagnetObserver> observersMagnetic;
	private Context context;
	private float[] magnetic = new float[3];
	private long timeStamp = 0;
	private SensorManager sensorManager;
	private FileOutputStream fOut;

	public Magnet(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		this.fOut = fOut;

		observersMagnetic = new ArrayList<MagnetObserver>();

		sensorManager = (SensorManager) this.context
				.getSystemService(Context.SENSOR_SERVICE);
	}

	public void registerMagneticObserver(MagnetObserver observer)
	{
		if (observersMagnetic.size() == 0)
		{
			sensorManager.registerListener(this,
					sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD),
					SensorManager.SENSOR_DELAY_FASTEST);
		}

		// Only register the observer if it is not already registered.
		int i = observersMagnetic.indexOf(observer);
		if (i == -1)
		{
			observersMagnetic.add(observer);
		}
	}

	public void removeMagneticObserver(MagnetObserver observer)
	{
		int i = observersMagnetic.indexOf(observer);
		if (i >= 0)
		{
			observersMagnetic.remove(i);
		}

		// If there are no observers, then don't listen for Sensor Events.
		if (observersMagnetic.size() == 0)
		{
			sensorManager.unregisterListener(this);
		}
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy)
	{

	}

	@Override
	public void onSensorChanged(SensorEvent event)
	{
		if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
		{
			System.arraycopy(event.values, 0, magnetic, 0, event.values.length);

			timeStamp = event.timestamp;

			notifyMagneticObserver();
		}
	}



	private void notifyMagneticObserver()
	{
		for (MagnetObserver a : observersMagnetic)
		{
			a.onMagneticSensorChanged(this.magnetic, this.timeStamp, fOut);
		}
	}

}