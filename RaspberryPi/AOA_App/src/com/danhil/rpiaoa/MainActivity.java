package com.danhil.rpiaoa;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;


import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;

import com.danhil.rpiaoa.R;
import com.danhil.rpiaoa.observers.SensorsObserver;
import com.danhil.rpiaoa.sensors.Sensors;

import android.app.*;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

public class MainActivity extends Activity implements Runnable, SensorsObserver {

	private static final String TAG = MainActivity.class.getSimpleName();
	private static final String ACTION_USB_PERMISSION = "com.danhil.rpiaoa.action.USB_PERMISSION";
	protected static File FILEPATH;
	private static String fileSensors;
	private static FileOutputStream foutSensors;
	private PendingIntent permissionIntent;
	private boolean requestPermissonPending;
	private UsbManager usbManagerMobile;
	private UsbAccessory accessoryForMobile;
	private ParcelFileDescriptor fileDescriptor;
	private FileInputStream inputStream;
	private FileOutputStream outputStream;
	private SeekBar pBar;
	private SeekBar iBar;
	private SeekBar dBar;

	private TextView statusView;
	private TextView tiltView;
	private TextView pValue;
	private TextView iValue;
	private TextView dValue;
	
	private double currPValue;
	private double currIValue;
	private double currDValue;
	private final double P_SCALE = 100;
	private final double I_SCALE = 100;
	private final double D_SCALE = 100;
	
	

	private Sensors sensors;
	//TODO Why is this vector 5 long?
	private float[] inputSensorValues = {0, 0, 0};

	private final BroadcastReceiver usbRecieve = new BroadcastReceiver(){

		@Override
		public void onReceive(Context context, Intent intent) {

			String action = intent.getAction();
			if (ACTION_USB_PERMISSION.equals(action))
			{
				synchronized (this)
				{
					UsbAccessory usbAccessory = UsbManager.getAccessory(intent);

					if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false))
					{
						openAccessory(usbAccessory);
					} else
					{
						Log.d(TAG, "Accesory permission denied " + usbAccessory);
					}

					requestPermissonPending = false;
				}
			} else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action))
			{
				UsbAccessory usbAccessory = UsbManager.getAccessory(intent);
				if (usbAccessory != null && usbAccessory.equals(accessoryForMobile))
				{
					closeAccessory();
				}
			}
		}
	};
	

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		/* init the sensors */
		FILEPATH = new File(Environment.getExternalStorageDirectory().getPath() + "/SensorSensing/");
		FILEPATH.mkdirs(); 	
		Log.d(TAG, "Path for gyrologs is :" + FILEPATH);
		prepareFileStreams();
		sensors = new Sensors(this, foutSensors);
		sensors.registerSensorsObserver(this);

		/* Init usbmanager and AOA COMMUNICATION */ 	 	
		usbManagerMobile = UsbManager.getInstance(this);
		permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(
				ACTION_USB_PERMISSION), 0);
		// Filter on intent charastaristics.
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(usbRecieve, filter);

		if (getLastNonConfigurationInstance() != null)
		{ //Use fragments instead.
			accessoryForMobile = (UsbAccessory) getLastNonConfigurationInstance();
			openAccessory(accessoryForMobile);
		}

		setContentView(R.layout.activity_main);
		statusView = (TextView) findViewById(R.id.statusIndication);
		tiltView = (TextView) findViewById(R.id.tiltValue);
		pBar = (SeekBar) findViewById(R.id.pValue);
		
		pValue = (TextView) findViewById(R.id.pTextValue);
		pBar.setOnSeekBarChangeListener(
				new OnSeekBarChangeListener() {
					
					@Override
					public void onStopTrackingTouch(SeekBar seekBar) {
						pValue.setText(String.valueOf(currPValue));
						/*TODO Set the resulting value to p in the controller*/
					}
					
					@Override
					public void onStartTrackingTouch(SeekBar seekBar) {}
					
					@Override
					public void onProgressChanged(SeekBar seekBar, int progress,
							boolean fromUser) {
						currPValue = ((double)progress)/P_SCALE;
					}
				});
		
		iValue = (TextView) findViewById(R.id.iTextValue);
		iBar = (SeekBar) findViewById(R.id.iValue);
		iBar.setOnSeekBarChangeListener(
				new OnSeekBarChangeListener() {
					@Override
					public void onStopTrackingTouch(SeekBar seekBar) {
						iValue.setText(String.valueOf(currIValue));
						
					}
					
					@Override
					public void onStartTrackingTouch(SeekBar seekBar) {}
					
					@Override
					public void onProgressChanged(SeekBar seekBar, int progress,
							boolean fromUser) {
						currIValue = ((double)progress)/I_SCALE;
						
					}
				});
		
		dValue = (TextView) findViewById(R.id.dTextValue);
		dBar = (SeekBar) findViewById(R.id.dValue);
		dBar.setOnSeekBarChangeListener(
				new OnSeekBarChangeListener() {
					
					@Override
					public void onStopTrackingTouch(SeekBar seekBar) {
						dValue.setText(String.valueOf(currDValue));
					}
					
					@Override
					public void onStartTrackingTouch(SeekBar seekBar) {}
					
					@Override
					public void onProgressChanged(SeekBar seekBar, int progress,
							boolean fromUser) {
						currDValue = ((double)progress)/D_SCALE;
					}
				});
		
		pValue.setText(String.valueOf(pBar.getProgress()));
		iValue.setText(String.valueOf(iBar.getProgress()));
		dValue.setText(String.valueOf(dBar.getProgress()));
		setConnectedStatus(false);
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
		sensors.onStart();
		restartAccessory();

	}

	private void restartAccessory()
	{
		if (inputStream != null && outputStream != null)
		{
			return;
		}
		UsbAccessory[] accessories = usbManagerMobile.getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
		if (accessory != null)
		{
			if (usbManagerMobile.hasPermission(accessory))
			{
				openAccessory(accessory);
			} else {
				synchronized (usbRecieve)
				{
					if (!requestPermissonPending)
					{
						usbManagerMobile.requestPermission(accessory,
								permissionIntent);
						requestPermissonPending = true;
					}
				}
			}
		} else {
			Log.d(TAG, "mAccessory is null");
		}

	}

	@Override
	public void onPause()
	{
		super.onPause();
		sensors.onPause();
		//USB
		closeAccessory();
	}



	@Override
	public void onDestroy()
	{
		// Unregister the usB
		unregisterReceiver(usbRecieve);
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

	private void openAccessory(UsbAccessory accessory)
	{
		fileDescriptor = usbManagerMobile.openAccessory(accessory);

		if (fileDescriptor != null)
		{
			accessoryForMobile = accessory;
			FileDescriptor fd = fileDescriptor.getFileDescriptor();

			inputStream = new FileInputStream(fd);
			outputStream = new FileOutputStream(fd);

			// communication thread start
			Thread thread = new Thread(null, this, "CommThread");
			thread.start();
			Log.d(TAG, "Opened accessory");

			setConnectedStatus(true);
		} else
		{
			Log.d(TAG, "accessory didnt open");
		}
	}

	private void closeAccessory()
	{
		setConnectedStatus(false);

		try
		{
			if (fileDescriptor != null)
			{
				fileDescriptor.close();
			}
		} catch (IOException e)
		{
		} finally 
		{
			fileDescriptor = null;
			accessoryForMobile = null;
		}
	}

	private void setConnectedStatus(boolean enable)
	{
		if (enable)
		{
			statusView.setText("Connected.");
		} else
		{
			statusView.setText("Not connected.");
		}
	}

	private static final int MESSAGE_LED = 1;

	@Override
	public void run()
	{
		int ret = 0;
		byte[] reciveBuffer = new byte[16384];
		int i;

		// Accessory -> Android, ReadThread
		while (ret >= 0)
		{
			try
			{
				ret = inputStream.read(reciveBuffer);
			} catch (IOException e)
			{
				e.printStackTrace();
				break;
			}

			if (ret > 0)
			{
				Log.d(TAG, ret + " bytes message received.");
			}
			i = 0;
			while (i < ret)
			{
				int len = ret - i;

				switch (reciveBuffer[i])
				{
				case 0x1:
					if (len >= 2)
					{
						Message m = Message.obtain(handler, MESSAGE_LED);
						m.arg1 = (int) reciveBuffer[i + 1];
						handler.sendMessage(m);
						i += 2;
					}
					break;

				default:
					Log.d(TAG, "unknown msg: " + reciveBuffer[i]);
					i = len;
					break;
				}
			}

		}
	}

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

	// Android -> Accessory
	public void sendCommand(byte command, byte value)
	{
		byte[] sendBuffer = new byte[2];
		sendBuffer[0] = command;
		sendBuffer[1] = value;
		if (outputStream != null)
		{
			try 
			{
				outputStream.write(sendBuffer);
			} catch (IOException e) {
				Log.e(TAG, "Failed Write Android->Acc", e);
			}
		}
	}

	public void sendCommand(byte[] command, byte[] value)
	{
		byte[] sendBuffer = concat(command,value);
		Log.d("sendCommand", "Sent the command: \n" + new String(sendBuffer));
		if (outputStream != null)
		{
			try 
			{
				outputStream.write(sendBuffer, 0, 8);
			} catch (IOException e) {
				Log.e(TAG, "Failed Write Android->Acc", e);
			}
		}
	}

	byte[] concat(byte[] A, byte[] B) {
		int aLen = A.length;
		int bLen = B.length;
		byte[] res= new byte[aLen+bLen];
		System.arraycopy(A, 0, res, 0, aLen);
		System.arraycopy(B, 0, res, aLen, bLen);
		return res;
	}

	@Override
	public void onSensorsChanged(float[] inputSensorValues, long timeStamp)
	{
		// Get a local copy of the sensor values
		System.arraycopy(inputSensorValues, 0, this.inputSensorValues, 0,
				inputSensorValues.length);
		byte[] command = {};
		float z = inputSensorValues[1]; //This is the pitch in radians
		double deg = Math.toDegrees(z);
		String xString = Float.toString(z);
		byte[] transmitX = xString.getBytes();
		sendCommand(command, transmitX);
		statusView.setText(Double.toString(z*(180/Math.PI)));
	}


}