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

import com.danhil.rpiaoa.observers.RotationVectorObserver;

public class RotationVector implements SensorEventListener
{

	private static final String tag = RotationVector.class.getSimpleName();
	private ArrayList<RotationVectorObserver> observersRotationVector;
	private boolean landscapeOrientation = false;
	private Context context;
	private SensorManager sensorManager;
	private float[] RotationVector = new float[5];
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

	public RotationVector(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		initQuaternionRotations();

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

			if (landscapeOrientation)
			{
				this.RotationVector = compensateToLandscape(this.RotationVector);
			}

			notifyRotationVectorObserver();
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

	private void notifyRotationVectorObserver()
	{
		for (RotationVectorObserver a : observersRotationVector)
		{
			a.onRotationVectorSensorChanged(this.RotationVector, this.timeStamp, fOut);
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