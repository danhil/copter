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
import android.app.*;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.ToggleButton;

public class MainActivity extends Activity implements Runnable {

	private static final String TAG = "RPIAOA";

	private static final String ACTION_USB_PERMISSION = "com.danhil.rpiaoa.action.USB_PERMISSION";

	private PendingIntent permissionIntent;
	private boolean requestPermissonPending;

	private UsbManager usbManagerMobile;
	private UsbAccessory accessoryForMobile;

	ParcelFileDescriptor fileDescriptor;

	FileInputStream inputStream;
	FileOutputStream outputStream;

	private ToggleButton button;
	private static ToggleButton statusButton;
	private TextView statusView;
	private SeekBar seekBar;

	private final BroadcastReceiver usbRecieve = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();

			if (ACTION_USB_PERMISSION.equals(action)) {
				synchronized (this) {
					UsbAccessory usbAccessory = UsbManager.getAccessory(intent);

					if (intent.getBooleanExtra(
							UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
						openAccessory(usbAccessory);
					} else {
						Log.d(TAG, "Accesory permission denied " + usbAccessory);
					}
					requestPermissonPending = false;
				}
			} else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
				UsbAccessory usbAccessory = UsbManager.getAccessory(intent);
				if (usbAccessory != null
						&& usbAccessory.equals(accessoryForMobile)) {
					closeAccessory();
				}
			}
		}
	};
	
	//GyroVariables
	private SensorManager mSensorManager;
	private Sensor mGyroSensor, mAccSensor, mMagSensor;
	public static final String TAG1 = "SensingService";
	private static String fileGyro,fileAcc,fileMag;
	private static FileOutputStream foutGyro, foutAcc, foutMag;
	public static File FILEPATH;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		// GYRO INIT
		Log.e(TAG, "onCreateGyro");
		FILEPATH = new File(Environment.getExternalStorageDirectory().getPath() + "/SensorSensing/");
		FILEPATH.mkdirs();
		Log.e(TAG, "Path for gyrologs is :" + FILEPATH);

		prepareFiles();

		mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);

		mGyroSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
		mAccSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		mMagSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
		
		
		// AOA COMMUNICATION
		usbManagerMobile = UsbManager.getInstance(this);
		permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(
				ACTION_USB_PERMISSION), 0);
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(usbRecieve, filter);
		
		if (getLastNonConfigurationInstance() != null) { //Use fragments instead.
			accessoryForMobile = (UsbAccessory) getLastNonConfigurationInstance();
			openAccessory(accessoryForMobile);
		}
		
		setContentView(R.layout.activity_main);
		button = (ToggleButton) findViewById(R.id.toggle);
		statusView = (TextView) findViewById(R.id.statusIndication);
		statusButton = (ToggleButton) findViewById(R.id.statusButton);
		seekBar = (SeekBar) findViewById(R.id.seekBar);

		button.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(CompoundButton buttonView,
					boolean isChecked) {
				byte command = 0x1;
				byte value = (byte) (isChecked ? 0x1 : 0x0);
				sendCommand(command, value);
			}
		});
		
		

		seekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				// Dragging knob
				byte value = (byte) (progress * 255 / 100);
				byte command = 0x2;
				Log.d(TAG, "Current Value:" + progress + "," + value);
				sendCommand(command, value);
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// Touch knob
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				// Release knob
				// mSeekBar.setProgress(50);
			}
		});

		enableControls(false);
	}
	
	private SensorEventListener mGyroListener = new SensorEventListener() {

		private static final float MIN_TIME_STEP = (1f / 40f);
		private long mLastTime = System.currentTimeMillis();
		private float mRotationX, mRotationY, mRotationZ;

		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
		}

		@Override
		public void onSensorChanged(SensorEvent event) {
			float[] values = event.values;
			float x = values[0];
			float y = values[1];
			float z = values[2];
			String logString;

			float angularVelocity = z * 0.96f; // Minor adjustment to avoid drift on Nexus

			// Calculate time diff
			long now = System.currentTimeMillis();
			float timeDiff = (now - mLastTime) / 1000f;
			mLastTime = now;
			if (timeDiff > 1) {
				// Make sure we don't go bananas after pause/resume
				timeDiff = MIN_TIME_STEP;
			}

			mRotationX += x * timeDiff;
			if (mRotationX > 0.5f)
				mRotationX = 0.5f;
			else if (mRotationX < -0.5f)
				mRotationX = -0.5f;

			mRotationY += y * timeDiff;
			if (mRotationY > 0.5f)
				mRotationY = 0.5f;
			else if (mRotationY < -0.5f)
				mRotationY = -0.5f;

			mRotationZ += angularVelocity * timeDiff;

			try {
				logString = ""+ timeDiff + " " + mRotationX + " " + mRotationY + " " + mRotationZ + "\n";
				foutGyro.write(logString.getBytes());
				foutGyro.flush();
			} catch (IOException e) {
				Log.e("IOError",e.toString());
			}
		}
	};

	private SensorEventListener mAccListener = new SensorEventListener() {
		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
		}

		@Override
		public void onSensorChanged(SensorEvent event) {
			float[] values = event.values;
			float x = values[0];
			float y = values[1];
			// float z = values[2];
			String logString;

			// Ignoring orientation since the activity is using screenOrientation "nosensor"
			try {
				logString = " " + -x + " " + y + " " + "\n";
				foutAcc.write(logString.getBytes());
				foutAcc.flush();
			} catch (IOException e) {
				Log.e("IOError",e.toString());
			}
		}
	};

	private SensorEventListener mMagListener = new SensorEventListener() {
		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
		}

		@Override
		public void onSensorChanged(SensorEvent event) {
			float[] values = event.values;
			float x = values[0];
			float y = values[1];
			// float z = values[2];
			String logString;
			try {
				logString = " " + x + " " + -y + " " + "\n";
				foutMag.write(logString.getBytes());
				foutMag.flush();
			} catch (IOException e) {
				Log.e("IOError",e.toString());
			}
		}
	};

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

		Log.d("prepareFiles", "excecuted");
	}

	public static String getCurrentTimeStamp() {
		Locale loc = Locale.getDefault();
		SimpleDateFormat sdfDate = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss", loc);
		Date now = new Date();
		String strDate = sdfDate.format(now);
		return strDate;
	}

	@Override
	public void onResume() {
		super.onResume();
		//GYRO
		mSensorManager.registerListener(mGyroListener, mGyroSensor, SensorManager.SENSOR_DELAY_UI);
		mSensorManager.registerListener(mAccListener, mAccSensor, SensorManager.SENSOR_DELAY_UI);
		mSensorManager.registerListener(mMagListener, mMagSensor, SensorManager.SENSOR_DELAY_UI);
		prepareFiles();
		
		// AOA USB communication
		if (inputStream != null && outputStream != null) {
			return;
		}

		UsbAccessory[] accessories = usbManagerMobile.getAccessoryList();
		UsbAccessory accessory = (accessories == null ? null : accessories[0]);
		if (accessory != null) {
			if (usbManagerMobile.hasPermission(accessory)) {
				openAccessory(accessory);
			} else {
				synchronized (usbRecieve) {
					if (!requestPermissonPending) {
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
	public void onPause() {
		super.onPause();
		// GYRO
		mSensorManager.unregisterListener(mGyroListener, mGyroSensor);
		mSensorManager.unregisterListener(mAccListener, mGyroSensor);
		mSensorManager.unregisterListener(mMagListener, mMagSensor);
		finishFiles();
		//USB
		closeAccessory();
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

	@Override
	public void onDestroy() {
		// GYRO
		finishFiles();
		//USB
		unregisterReceiver(usbRecieve);
		super.onDestroy();
	}

	private void openAccessory(UsbAccessory accessory) {
		fileDescriptor = usbManagerMobile.openAccessory(accessory);

		if (fileDescriptor != null) {
			accessoryForMobile = accessory;
			FileDescriptor fd = fileDescriptor.getFileDescriptor();

			inputStream = new FileInputStream(fd);
			outputStream = new FileOutputStream(fd);

			// communication thread start
			Thread thread = new Thread(null, this, "CommThread");
			thread.start();
			Log.d(TAG, "Opened accessory");

			enableControls(true);
		} else {
			Log.d(TAG, "accessory didnt open");
		}
	}

	private void closeAccessory() {
		enableControls(false);

		try {
			if (fileDescriptor != null) {
				fileDescriptor.close();
			}
		} catch (IOException e) {
		} finally {
			fileDescriptor = null;
			accessoryForMobile = null;
		}
	}

	private void enableControls(boolean enable) {
		if (enable) {
			statusView.setText("Connected.");
		} else {
			statusView.setText("Not connected.");
		}
		button.setEnabled(enable);
	}

	private static final int MESSAGE_LED = 1;

	@Override
	public void run() {
		int ret = 0;
		byte[] reciveBuffer = new byte[16384];
		int i;

		// Accessory -> Android, ReadThread
		while (ret >= 0) {
			try {
				ret = inputStream.read(reciveBuffer);
			} catch (IOException e) {
				e.printStackTrace();
				break;
			}

			if (ret > 0) {
				Log.d(TAG, ret + " bytes message received.");
			}
			i = 0;
			while (i < ret) {
				int len = ret - i;

				switch (reciveBuffer[i]) {
				case 0x1:
					if (len >= 2) {
						Message m = Message.obtain(mHandler, MESSAGE_LED);
						m.arg1 = (int) reciveBuffer[i + 1];
						mHandler.sendMessage(m);
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
	private static Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_LED:
				if (msg.arg1 == 0) {
					statusButton.setChecked(false);
				} else {
					statusButton.setChecked(true);
				}
				break;
			}
		}
	};

	// Android -> Accessory
	public void sendCommand(byte command, byte value) {
		byte[] sendBuffer = new byte[2];
		sendBuffer[0] = command;
		sendBuffer[1] = value;
		if (outputStream != null) {
			try {
				outputStream.write(sendBuffer);
			} catch (IOException e) {
				Log.e(TAG, "Failed Write Android->Acc", e);
			}
		}
	}
}