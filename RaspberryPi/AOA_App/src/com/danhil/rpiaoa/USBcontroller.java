package com.danhil.rpiaoa;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;

public class USBcontroller implements Runnable {

	private PendingIntent permissionIntent;
	private boolean requestPermissonPending;
	private UsbManager usbManagerMobile;
	private UsbAccessory accessoryForMobile;
	private ParcelFileDescriptor fileDescriptor;
	private FileInputStream inputStream;
	private FileOutputStream outputStream;
	private Context context;
	private Handler handler;
	private static final String ACTION_USB_PERMISSION = "com.danhil.rpiaoa.action.USB_PERMISSION";
	private static final String TAG = USBcontroller.class.getSimpleName();

	public USBcontroller(Context parentContext, Handler handler)
	{
		this.handler = handler;
		/* Init usbmanager and AOA COMMUNICATION */ 	 	
		usbManagerMobile = UsbManager.getInstance(parentContext);
		permissionIntent = PendingIntent.getBroadcast(parentContext, 0, new Intent(
				ACTION_USB_PERMISSION), 0);
		/*if (getLastNonConfigurationInstance() != null)
		{ //Use fragments instead.
			accessoryForMobile = (UsbAccessory) getLastNonConfigurationInstance();
			openAccessory(accessoryForMobile);
		}*/
		// Filter on intent charastaristics.
		IntentFilter filter = new IntentFilter();
		filter.addAction(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		parentContext.registerReceiver(usbRecieve, filter);

	}

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

	public void restartAccessory()
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

			sendToHandler(1);
		} else
		{
			Log.d(TAG, "accessory didnt open");
		}
	}

	public void closeAccessory()
	{
		sendToHandler(0);

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

	public void unregisterController()
	{
		context.unregisterReceiver(usbRecieve);
	}

	private void sendToHandler(int i)
	{
		Message m = Message.obtain(handler, i);
		m.arg1 = (int) i;
		handler.sendMessage(m);
	}

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
						sendToHandler((int) reciveBuffer[i + 1]);
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
				outputStream.write(sendBuffer, 0, 3);
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


}
