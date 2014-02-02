/**
 * 
 */
package com.danhil.control.pid;

/**
 * 
 */
public class PIDController
{
    private float           currentOutput;
    private float           currentSetpoint;
    private boolean         automaticMode;
    private float           inputVal;
    private long            sampleTime;
    private long            previousTime;
    private float           previousInput;
    private float           calculatedKi, calculatedKp, calculatedKd;
    private float           minOutput,maxOutput;
    private ProcessType 	processType;
    private float           setpointKp, setpointKi, setpointKd;
    
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
        sampleTime = 50;
        
        setMode(OperationMode.AUTOMATIC);
        setControllerProcessType(ProcessType.DIRECT);
        setOutputLimits(0, 255); //Rpi pwm limits?
        reTune(Kp, Ki, Kd);
        previousTime = System.currentTimeMillis();
    }     
     
    public boolean computeControlSignal( float inputValue )
    {
        if (!automaticMode)
        {
            return false;
        }
        
        long currentTime = System.currentTimeMillis();
        long sampleTimeDelta = (currentTime - previousTime);
        
        if ( sampleTimeDelta >= sampleTime )
        {
            /* Compute controller setpoint error variables */
            float input = inputValue;
            float error = currentSetpoint - input;
            inputVal += ( calculatedKi * error );

            if ( inputVal > maxOutput )
                inputVal = maxOutput;
            else if ( inputVal < minOutput )
                inputVal = minOutput;
            float dInput = ( input - previousInput );

            /* Compute Control Output */
            float output = calculatedKp * error + inputVal - calculatedKd * dInput;

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
            return true;
        } else
        {
            return false;
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

    public float getKp()
    {
        return setpointKp;
    }

    public float getKi()
    {
        return setpointKi;
    }

    public float getKd()
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

    public float getOutput()
    {
        return currentOutput;
    }

    public void setSetpointValue(float setpoint)
    {
        currentSetpoint = setpoint;
    }
}
