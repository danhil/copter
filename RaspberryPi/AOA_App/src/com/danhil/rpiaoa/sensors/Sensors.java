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
import com.danhil.rpiaoa.observers.SensorsObserver;
/**
 * Uses the observer pattern to register obervers to sensors.
 * @author fladnag
 *
 */
public class Sensors implements GyroscopeObserver,
AccelerationObserver, MagnetObserver, GravityObserver
{
	private static final boolean DEBUG = false;

	private static final String tag = Sensors.class
			.getSimpleName();
	private static final int MIN_SAMPLE_COUNT = 30;
	protected static File FILEPATH;
	private static String fileGyro,fileAcc,fileMag, fileGrav;
	private static FileOutputStream foutGyro, foutAcc, foutMag, foutGrav;

	// Keep track of observers.
	private ArrayList<SensorsObserver> sensorObservors;

	private boolean hasInitialOrientation = false;

	private Context context;

	private long timestampOld = 0;

	private float[] gyroscope = new float[]
			{0,0,0};

	// Raw accelerometer data
	private float[] acceleration = new float[]
			{ 0, 0, 0 };

	private float[] gravity = new float[]
			{ 0, 0, 0 };

	// Raw accelerometer data
	private float[] magnetic = new float[]
			{ 0, 0, 0 };

	private int gravitySampleCount = 0;
	private int magneticSampleCount = 0;
	private float[] initialRotationMatrix;

	private Gravity gravitySensor;
	private Gyroscope gyroscopeSensor;
	private Magnet magneticSensor;
	private Accelorometer accelorometerSensor;
	private String TAG = Sensors.class.getSimpleName();



	public Sensors(Context context)
	{
		super();

		this.context = context;
		sensorObservors = new ArrayList<SensorsObserver>();
		FILEPATH = new File(Environment.getExternalStorageDirectory().getPath() + "/SensorSensing/");
		FILEPATH.mkdirs(); 	
		Log.d(TAG, "Path for gyrologs is :" + FILEPATH);
		prepareFiles();
		initSensors();
		reset();
		restart();
	}

	public void prepareFiles(){
		Log.d("prepareFiles", "initialized");

		//create a new file for gyro
		try {
			Log.e("Log","Write");
			fileAcc = FILEPATH+"/"+getCurrentTimeStamp()+"-accl.txt";
			foutAcc = new FileOutputStream(fileAcc,true);
		} catch (IOException e) {
			Log.e("IOError",e.toString());
		}
		//create a new file for accelorometer
		try {
			Log.e("Log","Write");
			fileGyro = FILEPATH+"/"+getCurrentTimeStamp()+"-gyro.txt";
			foutGyro = new FileOutputStream(fileGyro,true);
		} catch (IOException e) {
			Log.e("IOError",e.toString());
		}
		//create a new file for mag
		try {
			fileMag = FILEPATH+"/"+getCurrentTimeStamp()+"-mag.txt";
			foutMag = new FileOutputStream(fileMag,true);
		} catch (IOException e) {
			Log.e("IOError",e.toString());
		}

		try {
			fileGrav = FILEPATH+"/"+getCurrentTimeStamp()+"-mag.txt";
			foutGrav = new FileOutputStream(fileGrav, true);
		} catch (Exception e) {
			// TODO: handle exception
		}

		Log.d("prepareFiles", "excecuted");
	}

	public static String getCurrentTimeStamp() {
		Locale loc = Locale.getDefault();
		SimpleDateFormat sdfDate = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss", loc);
		Date now = new Date();
		String strDate = sdfDate.format(now);
		return strDate;
	}


	public void onStart()
	{
		restart();
	}

	public void onPause()
	{
		reset();

	}

	public void onExit(){
		finishFiles();

	}

	@Override
	public void onAccelerationSensorChanged(float[] acceleration, long timeStamp, FileOutputStream out)
	{
		// Get a local copy of the raw magnetic values from the device sensor.
		System.arraycopy(acceleration, 0, this.acceleration, 0,
				acceleration.length);
		if(DEBUG)
			writeToFile(out, acceleration);
	}

	@Override
	public void onGravitySensorChanged(float[] gravity, long timeStamp, FileOutputStream out)
	{
		// Get a local copy of the raw magnetic values from the device sensor.
		System.arraycopy(gravity, 0, this.gravity, 0,
				gravity.length);

		gravitySampleCount++;

		if(DEBUG)
			writeToFile(out, gravity);

		if (gravitySampleCount > MIN_SAMPLE_COUNT
				&& magneticSampleCount > MIN_SAMPLE_COUNT
				&& !hasInitialOrientation)
		{
			calculateInitialOrientation();
		}

	}

	@Override
	public void onGyroscopeSensorChanged(float[] gyroscope, long timestamp, FileOutputStream out)
	{
		// don't start until first accelerometer/magnetometer orientation has
		// been acquired
		Log.e(TAG, "Gyro changed");
		if (!hasInitialOrientation)
		{
			return;
		}
		timestampOld = timestamp;
		System.arraycopy(gyroscope, 0, this.gyroscope, 0, gyroscope.length);
		if(DEBUG)
			writeToFile(out, gyroscope);

		notifySensorsObserver();
	}

	@Override
	public void onMagneticSensorChanged(float[] magnetic, long timeStamp, FileOutputStream out)
	{
		magneticSampleCount++;
		System.arraycopy(magnetic, 0, this.magnetic, 0, magnetic.length);
		if(DEBUG)
			writeToFile(out, magnetic);
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
		{
			a.onSensorsChanged(this.gyroscope,
					this.timestampOld);
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
	public void removeAccelerationObserver(
			SensorsObserver observer)
	{
		int i = sensorObservors.indexOf(observer);
		if (i >= 0)
		{
			sensorObservors.remove(i);
		}
	}

	private void calculateInitialOrientation()
	{
		hasInitialOrientation = SensorManager.getRotationMatrix(
				initialRotationMatrix, null, gravity, magnetic);

		if (hasInitialOrientation)
		{
			gravitySensor.removeGravityObserver(this);
		}               magneticSensor.removeMagneticObserver(this);
	}

	private void initSensors()
	{
		gravitySensor = new Gravity(context, foutGrav);
		magneticSensor = new Magnet(context, foutMag);
		gyroscopeSensor = new Gyroscope(context, foutGyro);
		accelorometerSensor = new Accelorometer(context, foutAcc);
	}

	private void restart()
	{
		gravitySensor.registerGravityObserver(this);
		magneticSensor.registerMagneticObserver(this);
		gyroscopeSensor.registerGyroscopeObserver(this);
		accelorometerSensor.registerAccelerationObserver(this);
	}

	private void reset()
	{
		gravitySensor.removeGravityObserver(this);
		magneticSensor.removeMagneticObserver(this);
		gyroscopeSensor.removeGyroscopeObserver(this);
		accelorometerSensor.removeAccelerationObserver(this);

		acceleration = new float[3];
		magnetic = new float[3];
		initialRotationMatrix = new float[9];
		gravitySampleCount = 0;
		magneticSampleCount = 0;

		hasInitialOrientation = false;

		finishFiles();
	}

	public void finishFiles(){
		try{
			//	foutAcc.flush();
			foutAcc.close();
			//	foutGyro.flush();
			foutGyro.close();
			//	foutMag.flush();
			foutMag.close();
		}catch(IOException e){
			Log.e("IOException","Error on closing files");
		}
	}
}