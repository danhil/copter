package com.danhil.rpiaoa.sensors;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

import android.content.Context;
import android.hardware.SensorManager;
import android.os.Environment;
import android.util.Log;

import com.danhil.rpiaoa.observers.AccelerationObserver;
import com.danhil.rpiaoa.observers.GravityObserver;
import com.danhil.rpiaoa.observers.GyroscopeObserver;
import com.danhil.rpiaoa.observers.MagnetObserver;
import com.danhil.rpiaoa.observers.OrientationObserver;
import com.danhil.rpiaoa.observers.RotationVectorObserver;
import com.danhil.rpiaoa.observers.SensorsObserver;
/**
 * Uses the observer pattern to register obervers to sensors.
 * @author fladnag
 *
 */
public class Sensors implements GyroscopeObserver,
AccelerationObserver, MagnetObserver, GravityObserver, RotationVectorObserver, OrientationObserver
{
	private static final boolean DEBUG = false;

	private static final String tag = Sensors.class
			.getSimpleName();
	private static final int MIN_SAMPLE_COUNT = 30;

	// Keep track of observers.
	private ArrayList<SensorsObserver> sensorObservors;

	private Context context;

	private long timestampOld = 0;

	private float[] gyroscope = new float[]
			{0,0,0};

	// Raw accelerometer data
	private float[] acceleration = new float[]
			{ 0, 0, 0 };
	private float[] filteredAccelerationVals = new float[]
			{ 0, 0, 0 };

	private float[] gravity = new float[]
			{ 0, 0, 0 };

	// Raw accelerometer data
	private float[] magnetic = new float[]
			{ 0, 0, 0 };
	private float[] filteredMagneticVals = new float[]
			{ 0, 0, 0 };

	// Raw accelerometer data
	private float[] rotation = new float[]
			{ 0, 0, 0, 0, 0 };

	// Raw orientation data
	private float[] orientation = new float[]
			{ 0, 0, 0 };

	private float[] orientationVals = new float[3];

	private float [] inRotation = new float[16];
	private float [] inInclination = new float[16];

	private int gravitySampleCount = 0;
	private int magneticSampleCount = 0;
	private float[] initialRotationMatrix;

	private Gravity gravitySensor;
	private Gyroscope gyroscopeSensor;
	private Magnet magneticSensor;
	private Accelorometer accelorometerSensor;
	private RotationVector rotationVectorSensor;
	private Orientation orientationSensor;
	private FileOutputStream foutSensor;
	private SensorManager sensorManager;
	private String TAG = Sensors.class.getSimpleName();
	static final float ALPHA = 0.25f;

	public Sensors(Context context, FileOutputStream foutSensor)
	{
		super();
		this.context = context;
		sensorManager = (SensorManager) this.context
				.getSystemService(Context.SENSOR_SERVICE);
		this.foutSensor = foutSensor;
		sensorObservors = new ArrayList<SensorsObserver>();
		initSensors();
		reset();
		restart();
	}

	public void onStart()
	{
		restart();
	}

	public void onPause()
	{
		reset();

	}


	@Override
	public void onAccelerationSensorChanged(float[] acceleration, long timeStamp, FileOutputStream out)
	{
		// Get a local copy of the raw magnetic values from the device sensor.
		System.arraycopy(acceleration, 0, this.acceleration, 0,
				acceleration.length);
		filteredAccelerationVals = lowPass(this.acceleration, filteredAccelerationVals);
		if(DEBUG)
			writeToFile(out, acceleration);
		notifySensorsObserver();
	}

	@Override
	public void onOrientationSensorChanged(float[] orientation, long timestamp, FileOutputStream out)
	{

		timestampOld = timestamp;
		System.arraycopy(orientation, 0, this.orientation, 0, orientation.length);
		if(DEBUG)
			writeToFile(out, rotation);

		//notifySensorsObserver();
	}

	@Override
	public void onGravitySensorChanged(float[] gravity, long timeStamp, FileOutputStream out)
	{
		// Get a local copy of the raw magnetic values from the device sensor.
		System.arraycopy(gravity, 0, this.gravity, 0,
				gravity.length);

		if(DEBUG)
			writeToFile(out, gravity);

	}

	@Override
	public void onGyroscopeSensorChanged(float[] gyroscope, long timestamp, FileOutputStream out)
	{
		// don't start until first accelerometer/magnetometer orientation has
		// been acquired
		Log.e(TAG, "Gyro changed");
		timestampOld = timestamp;
		System.arraycopy(gyroscope, 0, this.gyroscope, 0, gyroscope.length);
		if(DEBUG)
			writeToFile(out, gyroscope);
	}

	@Override
	public void onMagneticSensorChanged(float[] magnetic, long timeStamp, FileOutputStream out)
	{
		Log.e(TAG, "Magnet changed");
		magneticSampleCount++;
		System.arraycopy(magnetic, 0, this.magnetic, 0, magnetic.length);
		filteredMagneticVals = lowPass(this.magnetic, filteredMagneticVals);
		if(DEBUG)
			writeToFile(out, magnetic);
	}

	@Override
	public void onRotationVectorSensorChanged(float[] rotation, long timestamp, FileOutputStream out)
	{
		// don't start until first accelerometer/magnetometer orientation has
		// been acquired
		Log.e(TAG, "Rotation changed");
		timestampOld = timestamp;
		System.arraycopy(rotation, 0, this.rotation, 0, rotation.length);
		if(DEBUG)
			writeToFile(out, rotation);
	}

	private void writeToFile(FileOutputStream out, float[] values){
		String logString;
		float x = values[0];
		float y = values[1];
		float z = values[2];
		try {
			logString = "" + " " + x + " " + y + " " + z + "\n";
			out.write(logString.getBytes());
			out.flush();
		} catch (IOException e) {
			Log.e("IOError",e.toString());
		}
	}

	/**
	 * Notify observers with new measurements.
	 */
	private void notifySensorsObserver()
	{
		for (SensorsObserver a : sensorObservors)
		{	/* TODO: this is specific as of now to return the device orientation matrix to 
			 * be able to measure the offset from y axis. Needs to be made more general */
			if( filteredAccelerationVals!= null && filteredMagneticVals != null)
			{
				boolean success = sensorManager.getRotationMatrix(inRotation, inInclination, filteredAccelerationVals, filteredMagneticVals);
				if (success){
					sensorManager.getOrientation(inRotation, this.orientationVals);
					a.onSensorsChanged(this.orientationVals,
							this.timestampOld);
				}
			}
		}
	}

	public void registerSensorsObserver(
			SensorsObserver observer)
	{
		// Only register the observer if it is not already registered.
		int i = sensorObservors.indexOf(observer);
		if (i == -1)
		{
			sensorObservors.add(observer);
		}

	}
	public void removeSensorsObserver(
			SensorsObserver observer)
	{
		int i = sensorObservors.indexOf(observer);
		if (i >= 0)
		{
			sensorObservors.remove(i);
		}
	}


	private void initSensors()
	{
		//gravitySensor = new Gravity(context, foutSensor);
		magneticSensor = new Magnet(context, foutSensor);
		accelorometerSensor = new Accelorometer(context, foutSensor);
		//gyroscopeSensor = new Gyroscope(context, foutSensor);
		//rotationVectorSensor = new RotationVector(context, foutSensor);
		//orientationSensor = new Orientation(context, foutSensor);

	}

	private void restart()
	{
		//gravitySensor.registerGravityObserver(this);
		magneticSensor.registerMagneticObserver(this);
		accelorometerSensor.registerAccelerationObserver(this);
		//gyroscopeSensor.registerGyroscopeObserver(this);
		//rotationVectorSensor.registerRotationVectorObserver(this);
		//orientationSensor.registerOrientationObserver(this);
	}

	private void reset()
	{
		//gravitySensor.removeGravityObserver(this);
		magneticSensor.removeMagneticObserver(this);
		accelorometerSensor.removeAccelerationObserver(this);
		//gyroscopeSensor.removeGyroscopeObserver(this);
		//rotationVectorSensor.removeRotationVectorObserver(this);
		//orientationSensor.registerOrientationObserver(this);

		acceleration = new float[3];
		magnetic = new float[3];
		initialRotationMatrix = new float[9];
		gravitySampleCount = 0;
		magneticSampleCount = 0;
	}

	protected float[] lowPass( float[] input, float[] output ) {
		if ( output == null ) return input;

		for ( int i=0; i<input.length; i++ ) {
			output[i] = output[i] + ALPHA * (input[i] - output[i]);
		}
		return output;
	}


}
