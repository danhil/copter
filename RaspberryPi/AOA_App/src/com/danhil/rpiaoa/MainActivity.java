package com.danhil.rpiaoa;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import com.danhil.control.pid.PIDController;
import com.danhil.rpiaoa.R;
import com.danhil.rpiaoa.observers.SensorsObserver;
import com.danhil.rpiaoa.sensors.Sensors;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends FragmentActivity implements SensorsObserver {

	private static final String TAG = MainActivity.class.getSimpleName();
	protected static File FILEPATH;
	private static String fileSensors;
	private static FileOutputStream foutSensors;
	private USBcontroller usbController;
	private PIDController PID;
	private ControllerFragment fragment_obj;

	private Sensors sensors;
	//TODO Why is this vector 5 long?
	private float[] inputSensorValues = {0, 0, 0};

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		if (savedInstanceState == null) {
            Fragment newFragment = new ControllerFragment();
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            ft.add(R.id.fragment_container, newFragment).commit();
        }
		/* init the sensors */
		FILEPATH = new File(Environment.getExternalStorageDirectory().getPath() + "/SensorSensing/");
		FILEPATH.mkdirs(); 	
		Log.d(TAG, "Path for gyrologs is :" + FILEPATH);
		prepareFileStreams();
		sensors = new Sensors(this, foutSensors);
		sensors.registerSensorsObserver(this);
		usbController = new USBcontroller(this, handler);
		PID = new PIDController(90, 1, 1, 1);
		setContentView(R.layout.activity_main);
		
	}

	public void prepareFileStreams()
	{
		Log.d("prepareFiles", "initialized");
		try 
		{
			Log.e("Log","Write");
			fileSensors = FILEPATH+"/"+getCurrentTimeStamp()+"-sensors.txt";
			foutSensors= new FileOutputStream(fileSensors, true);
		} catch (IOException e) {
			e.printStackTrace();
			Log.e(TAG, "Could not open fileIO");
		}

		Log.d("prepareFiles", "excecuted");
	}

	
	public static String getCurrentTimeStamp()
	{
		Locale loc = Locale.getDefault();
		SimpleDateFormat sdfDate = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss", loc);
		Date now = new Date();
		String strDate = sdfDate.format(now);
		return strDate;
	}

	@Override
	public void onResume()
	{
		super.onResume();
		fragment_obj = (ControllerFragment)getSupportFragmentManager().
                findFragmentById(R.id.fragment_container);
		sensors.onStart();
		usbController.restartAccessory();
	}

	@Override
	public void onPause()
	{
		super.onPause();
		sensors.onPause();
		//USB
		usbController.closeAccessory();
	}


	@Override
	public void onDestroy()
	{
		// Unregister the usB
		usbController.unregisterController();
		finishFiles();
		super.onDestroy();
	}

	
	public void finishFiles()
	{
		try{
			//	foutAcc.flush();
			foutSensors.close();
		}catch(IOException e){
			Log.e("IOException","Error on closing files");
		}
	}

	
	private static final int MESSAGE_LED = 1;
	// Change the view in the UI thread, handler is static so no leaks occur
	private static Handler handler = new Handler()
	{
		@Override
		public void handleMessage(Message msg) 
		{
			switch (msg.what)
			{
			case MESSAGE_LED:
				if (msg.arg1 == 0)
				{
					//blink
				} else
				{
					//Poo
				}
				break;
			}
		}
	};

	
	@Override
	public void onSensorsChanged(float[] inputSensorValues, long timeStamp)
	{
		// Get a local copy of the sensor values
		System.arraycopy(inputSensorValues, 0, this.inputSensorValues, 0,
				inputSensorValues.length);
		byte[] command = {};
		float z = Math.abs(inputSensorValues[1]); //This is the pitch in radians
		PID.computeControlSignal(z*(180/Math.PI));
		double controlOutput = PID.getOutput();
		String controlOutputString = Double.toString(controlOutput);
		byte[] transmitX = controlOutputString.getBytes();
		usbController.sendCommand(command, transmitX);
		fragment_obj.updateTiltView(Double.toString(z*(180/Math.PI)));
		fragment_obj.updateControlView(controlOutputString);
		
	}


}