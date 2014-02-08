/**
 * 
 */
package com.danhil.control.pid;

/**
 * 
 */
public class PIDController
{
    private double           currentOutput;
    private double          currentSetpoint;
    private boolean         automaticMode;
    private double          inputVal;
    private long            sampleTime;
    private long            previousTime;
    private double           previousInput;
    private double			currentInputValue;
    private double           calculatedKi, calculatedKp, calculatedKd;
    private double           minOutput,maxOutput;
    private ProcessType 	processType;
    private double          setpointKp, setpointKi, setpointKd;
    
    private enum OperationMode
    {
    	AUTOMATIC,
    	MANUAL 
    };

    private enum ProcessType //Keep track of whether it is a Direct or Inverted acting process
    { 
    	DIRECT, 
    	INVERTED
    };
    
    public PIDController(float setPoint, float Kp, float Ki, float Kd)
    {
        currentSetpoint = setPoint;
        automaticMode = false;
        currentInputValue = 90;
        sampleTime = 50;
        
        setMode(OperationMode.AUTOMATIC);
        setControllerProcessType(ProcessType.DIRECT);
        setOutputLimits(0, 255); //Rpi pwm limits?
        reTune(Kp, Ki, Kd);
        previousTime = System.currentTimeMillis();
    }
     
    public void computeControlSignal(double currentInputValue)
    {
        if (!automaticMode)
        {
            return;
        }
        
        long currentTime = System.currentTimeMillis();
        long sampleTimeDelta = (currentTime - previousTime);
        
        if ( sampleTimeDelta >= sampleTime )
        {
            /* Compute controller setpoint error variables */
        	double input = currentInputValue;
            double error = currentSetpoint - input;
            inputVal += ( calculatedKi * error );

            if ( inputVal > maxOutput )
                inputVal = maxOutput;
            else if ( inputVal < minOutput )
                inputVal = minOutput;
            double dInput = ( input - previousInput );

            /* Compute Control Output, maybe needs to be a float value */
            double output = calculatedKp * error + inputVal - calculatedKd * dInput;

            if ( output > maxOutput )
            {
                output = maxOutput;
            } else if ( output < minOutput )
            {
                output = minOutput;
            }
            currentOutput = output;
            previousInput = input;
            previousTime = currentTime;
        }
    }

    public void reTune( float Kp, float Ki, float Kd )
    {
        if ( Kp < 0 || Ki < 0 || Kd < 0 )
        {
            return;
        }

        setpointKp = Kp;
        setpointKi = Ki;
        setpointKd = Kd;

        float sampleTimeInSec = ( (float) sampleTime ) / 1000;
        calculatedKp = Kp;
        calculatedKi = Ki * sampleTimeInSec;
        calculatedKd = Kd / sampleTimeInSec;

        if ( processType == ProcessType.INVERTED )
        {
            calculatedKp = ( 0 - calculatedKp );
            calculatedKi = ( 0 - calculatedKi );
            calculatedKd = ( 0 - calculatedKd );
        }
    }

    public void setSampleTime_ms( int newSampleTime )
    {
        if ( newSampleTime > 0 )
        {
            float ratio = (float) newSampleTime / (float) sampleTime;
            calculatedKi *= ratio;
            calculatedKd /= ratio;
            sampleTime = newSampleTime;
        }
    }
     
    public void setOutputLimits(float min, float max)
    {
        if(min >= max)
            return;
        minOutput = min;
        maxOutput = max;

        if(automaticMode)
        {
            if(currentOutput > maxOutput)
            {
                currentOutput = maxOutput;
            } else if(currentOutput < minOutput)
            {
                currentOutput = minOutput;
            }

            if(inputVal > maxOutput)
            {
                inputVal = maxOutput;
            } else if(inputVal < minOutput)
            {
                inputVal = minOutput;
            }
        }
    }

    public void setMode( OperationMode mode )
    {
        boolean newMode = ( mode == OperationMode.AUTOMATIC );
        if(newMode == !automaticMode)
        {
            init();
        }
        automaticMode = newMode;
    }
     
   /* Initialize the controller, bumpless transfer */
    private void init()
    {
        inputVal = currentOutput;
        previousInput = 0;
        if (inputVal > maxOutput)
        {
            inputVal = maxOutput;
        } else if(inputVal < minOutput)
        {
            inputVal = minOutput;
        }
    }
    
    public void setControllerProcessType(ProcessType type)
    {
        if (automaticMode&&type != processType)
        {
            calculatedKp = (0 - calculatedKp);
            calculatedKi = (0 - calculatedKi);
            calculatedKd = (0 - calculatedKd);
        }
        processType = type;
    }

    public double getKp()
    {
        return setpointKp;
    }

    public double getKi()
    {
        return setpointKi;
    }

    public double getKd()
    {
        return setpointKd;
    }

    public OperationMode getMode()
    {
        return automaticMode ? OperationMode.AUTOMATIC : OperationMode.MANUAL;
    }

    public ProcessType getProcessType()
    {
        return processType;
    }

    public double getOutput()
    {
        return currentOutput;
    }

    public void setSetpointValue(float setpoint)
    {
        currentSetpoint = setpoint;
    }

}
