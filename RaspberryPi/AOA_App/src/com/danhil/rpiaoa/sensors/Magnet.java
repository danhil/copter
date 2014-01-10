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

import com.danhil.rpiaoa.observers.MagnetObserver;

public class Magnet implements SensorEventListener
{

	private static final String tag = Magnet.class.getSimpleName();

	// Keep track of observers.
	private ArrayList<MagnetObserver> observersMagnetic;
	private boolean landscapeMode = false;
	private Context context;
	private float[] magnetic = new float[3];
	private long timeStamp = 0;
	private Rotation yQuaternion;
	private Rotation xQuaternion;
	private Rotation rotationQuaternion;
	private SensorManager sensorManager;
	private Vector3D vIn;
	private Vector3D vOut;
	private FileOutputStream fOut;

	public Magnet(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		this.fOut = fOut;

		initQuaternionRotations();

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

			if (landscapeMode)
			{
				this.magnetic = quaternionToDeviceVehicleMode(this.magnetic);
			}

			notifyMagneticObserver();
		}
	}

	public void setLandscapeMode(boolean vehicleMode)
	{
		this.landscapeMode = vehicleMode;
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

	private void notifyMagneticObserver()
	{
		for (MagnetObserver a : observersMagnetic)
		{
			a.onMagneticSensorChanged(this.magnetic, this.timeStamp, fOut);
		}
	}

	private float[] quaternionToDeviceVehicleMode(float[] matrix)
	{
		vIn = new Vector3D(matrix[0], matrix[1], matrix[2]);
		vOut = rotationQuaternion.applyTo(vIn);

		float[] rotation =
			{ (float) vOut.getX(), (float) vOut.getY(), (float) vOut.getZ() };

		return rotation;
	}
}