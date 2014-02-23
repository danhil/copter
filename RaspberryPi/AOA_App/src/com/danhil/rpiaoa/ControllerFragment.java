package com.danhil.rpiaoa;

import android.os.Bundle;
import android.app.Activity;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class ControllerFragment extends Fragment {

	private SeekBar pBar;
	private SeekBar iBar;
	private SeekBar dBar;

	private TextView statusView;
	private TextView tiltView;
	private TextView controlView;
	private TextView pValue;
	private TextView iValue;
	private TextView dValue;

	private double currPValue;
	private double currIValue;
	private double currDValue;
	private final double P_SCALE = 100; // 1ms
	private final double I_SCALE = 100;
	private final double D_SCALE = 100;

	@Override
	public void onAttach(Activity activity)
	{
		super.onAttach(activity);

	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup view, Bundle bundle)
	{
		View viewRoot = inflater.inflate(R.layout.controller_fragment, null);

		statusView = (TextView) viewRoot.findViewById(R.id.statusIndication);
		tiltView = (TextView) viewRoot.findViewById(R.id.tiltValue);
		controlView = (TextView) viewRoot.findViewById(R.id.controlValue);

		pValue = (TextView) viewRoot.findViewById(R.id.pTextValue);
		pBar = (SeekBar) viewRoot.findViewById(R.id.pValue);
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

		iValue = (TextView) viewRoot.findViewById(R.id.iTextValue);
		iBar = (SeekBar) viewRoot.findViewById(R.id.iValue);
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

		dValue = (TextView) viewRoot.findViewById(R.id.dTextValue);
		dBar = (SeekBar) viewRoot.findViewById(R.id.dValue);
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
		return viewRoot;
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState)
	{
		super.onActivityCreated(savedInstanceState);

	}

	@Override
	public void onStart()
	{
		super.onStart();

	}

	@Override
	public void onResume()
	{
		super.onResume();

	}

	@Override
	public void onPause()
	{
		super.onPause();

	}
	
	public void  updateControlView(String updateValue)
	{
		this.controlView.setText(updateValue);
	}
	
	public void  updateTiltView(String updateValue)
	{
		this.tiltView.setText(updateValue);
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

}
