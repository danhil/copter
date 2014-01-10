package com.danhil.rpiaoa;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;


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
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends Activity implements Runnable, SensorsObserver {

	private static final String TAG = "RPIAOA";

	private static final String ACTION_USB_PERMISSION = "com.danhil.rpiaoa.action.USB_PERMISSION";

	private PendingIntent permissionIntent;
	private boolean requestPermissonPending;

	private UsbManager usbManagerMobile;
	private UsbAccessory accessoryForMobile;
	
	ParcelFileDescriptor fileDescriptor;

	FileInputStream inputStream;
	FileOutputStream outputStream;

	private TextView statusView;
	
	private Sensors sensors;
	private float[] gyro = {0, 0, 0};

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
	public static final String TAG1 = "SensingService";
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// Thinigymabobs
		sensors = new Sensors(this);
		sensors.registerSensorsObserver(this);
		
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
		statusView = (TextView) findViewById(R.id.statusIndication);

		enableControls(false);
	}

	@Override
	public void onResume() {
		super.onResume();
		sensors.onStart();
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
		sensors.onPause();
		//USB
		closeAccessory();
	}
	
	

	@Override
	public void onDestroy() {
		sensors.onExit();
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
	private static Handler handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MESSAGE_LED:
				if (msg.arg1 == 0) {
					//blink
				} else {
					//Poo
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
	
	public void sendCommand(byte[] command, byte[] value) {
		byte[] sendBuffer = concat(command,value);
		Log.d("sendCommand", "Sent the command: \n" + new String(sendBuffer));
		if (outputStream != null) {
			try {
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
    public void onSensorsChanged(float[] gyro, long timeStamp)
    {
			statusView.setText("Sensors Changed");
            // Get a local copy of the sensor values
            System.arraycopy(gyro, 0, this.gyro, 0,
                            gyro.length);
            byte[] command = {};
            float x = gyro[0];
            String xString = Float.toString(x);
            byte[] transmitX = xString.getBytes();
            sendCommand(command, transmitX);
            statusView.setText(Float.toString(x));
    }


}