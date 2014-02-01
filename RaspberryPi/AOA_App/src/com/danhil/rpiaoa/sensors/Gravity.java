package com.danhil.rpiaoa.sensors;
import java.io.FileOutputStream;
import java.util.ArrayList;

import com.danhil.rpiaoa.observers.GravityObserver;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
public class Gravity implements SensorEventListener
{
	private static final String tag = Gravity.class.getSimpleName();

	private ArrayList<GravityObserver> observersGravity;
	// We need the Context to register for Sensor Events.
	private Context context;
	private float[] gravity = new float[3];
	private long timeStamp = 0;
	private SensorManager sensorManager;
	private FileOutputStream fOut;

	public Gravity(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		this.fOut = fOut;

		observersGravity = new ArrayList<GravityObserver>();

		sensorManager = (SensorManager) this.context
				.getSystemService(Context.SENSOR_SERVICE);

	}

	public void registerGravityObserver(GravityObserver observer)
	{
		// If there are currently no observers, but one has just requested to be
		// registered, register to listen for sensor events from the device.
		if (observersGravity.size() == 0)
		{
			sensorManager.registerListener(this,
					sensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY),
					SensorManager.SENSOR_DELAY_FASTEST);
		}

		int i = observersGravity.indexOf(observer);
		if (i == -1)
		{	
			observersGravity.add(observer);
		}
	}

	public void removeGravityObserver(GravityObserver observer)
	{
		int i = observersGravity.indexOf(observer);
		if (i >= 0)
		{
			observersGravity.remove(i);
		}

		// If there are no observers, then don't listen for Sensor Events.
		if (observersGravity.size() == 0)
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
		if (event.sensor.getType() == Sensor.TYPE_GRAVITY)
		{
			System.arraycopy(event.values, 0, gravity, 0,
					event.values.length);

			timeStamp = event.timestamp;


			notifyGravityObserver();
		}
	}

	private void notifyGravityObserver()
	{
		for (GravityObserver a : observersGravity)
		{
			a.onGravitySensorChanged(this.gravity, this.timeStamp, fOut);
		}
	}


}