package com.danhil.rpiaoa.sensors;
import java.io.FileOutputStream;
import java.util.ArrayList;
import org.apache.commons.math3.geometry.euclidean.threed.Rotation;
import org.apache.commons.math3.geometry.euclidean.threed.Vector3D;

import com.danhil.rpiaoa.observers.GravityObserver;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;
public class Gravity implements SensorEventListener
{
	private static final String tag = Gravity.class.getSimpleName();

	private ArrayList<GravityObserver> observersGravity;
	private boolean landscapeMode = false;
	// We need the Context to register for Sensor Events.
	private Context context;
	private float[] gravity = new float[3];
	private long timeStamp = 0;
	private Rotation yQuaternion;
	private Rotation xQuaternion;
	private Rotation rotationQuaternion;
	private SensorManager sensorManager;
	private Vector3D vIn;
	private Vector3D vOut;
	private FileOutputStream fOut;

	public Gravity(Context context, FileOutputStream fOut)
	{
		super();

		this.context = context;

		this.fOut = fOut;

		initQuaternionRotations();

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

			if (landscapeMode)
			{
				gravity = quaternionToDeviceVehicleMode(gravity);
			}

			notifyGravityObserver();
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

	private void notifyGravityObserver()
	{
		for (GravityObserver a : observersGravity)
		{
			a.onGravitySensorChanged(this.gravity, this.timeStamp, fOut);
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