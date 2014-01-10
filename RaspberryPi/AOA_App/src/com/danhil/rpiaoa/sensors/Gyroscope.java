package com.danhil.rpiaoa.sensors;


import java.io.FileOutputStream;
import java.util.ArrayList;

import org.apache.commons.math3.geometry.euclidean.threed.Rotation;
import org.apache.commons.math3.geometry.euclidean.threed.Vector3D;

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
	private boolean landscapeOrientation = false;
	private Context context;
	private SensorManager sensorManager;
	private float[] gyroscope = new float[3];
	// Keep track of the moest recent event.
	private long timeStamp = 0;
	// ROtate from normal android orientation to actual orientation
	private Rotation yQuaternion;
	private Rotation xQuaternion;
	private Rotation rotationQuaternion;
	// ROtation vectors
	private Vector3D vIn;
	private Vector3D vOut;
	private FileOutputStream fOut;

	public Gyroscope(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		initQuaternionRotations();

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
					SensorManager.SENSOR_DELAY_FASTEST);
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

			if (landscapeOrientation)
			{
				this.gyroscope = compensateToLandscape(this.gyroscope);
			}

			notifyGyroscopeObserver();
		}
	}

	public void setLandscapeMode(boolean landscapeMode)
	{
		this.landscapeOrientation = landscapeMode;
	}

	private void initQuaternionRotations()
	{
		// Rotate by 90 degrees or pi/2 radians.
		double rotation = Math.PI / 2;

		// Create the rotation around the x-axis
		Vector3D xV = new Vector3D(1, 0, 0);
		xQuaternion = new Rotation(xV, rotation);

		// Create the rotation around the y-axis
		Vector3D yV = new Vector3D(0, 1, 0);
		yQuaternion = new Rotation(yV, -rotation);

		// Create the composite rotation.
		rotationQuaternion = yQuaternion.applyTo(xQuaternion);
	}

	private void notifyGyroscopeObserver()
	{
		for (GyroscopeObserver a : observersGyroscope)
		{
			a.onGyroscopeSensorChanged(this.gyroscope, this.timeStamp, fOut);
		}
	}

	private float[] compensateToLandscape(float[] matrix)
	{
		vIn = new Vector3D(matrix[0], matrix[1], matrix[2]);
		vOut = rotationQuaternion.applyTo(vIn);
		float[] rotation =
			{ (float) vOut.getX(), (float) vOut.getY(), (float) vOut.getZ() };
		return rotation;
	}
}