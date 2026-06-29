#include "ni6343.h"
#include "NIDAQmx.h"
#include <QDebug>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

TaskHandle  taskHandle1=0;
TaskHandle  taskHandle2=0;
TaskHandle  taskHandle3=0;
TaskHandle  taskHandle4=0;

float cal_voltage[69] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.01, 1.02, 1.03, 1.04, 1.05, 1.06, 1.07, 1.08, 1.09, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4, 4.01, 4.02, 4.03, 4.04, 4.05, 4.06, 4.07, 4.08, 4.09, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 5 };

float cal_speed[69] = { -1265, -1265, -1270, -1265, -1269, -1263, -1263, -1277, -1267, -1146, -489, -292, -206, -163, -133, -112, -96, -86, -76, -69, -36, -24, -19, -27, -25, -20, -19, -16, -15, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 9, 10, 11, 13, 17, 20, 29, 46, 123, 150, 186, 253, 410, 874, 1277, 1280, 1281, 1266, 1266, 1288, 1281, 1263, 1270, 1261, 1266, 1289, 1282, 1264 };


NI6343::NI6343()
{
    stopped = false;
    update_channel = 0;
}


void NI6343::run()
{
    int         error=0;

    float64     data = 0;
    char        errBuff[2048]={'\0'};
    voltage1 = voltage2 = voltage3 = voltage4 = 0;

    DAQmxErrChk (DAQmxCreateTask("v1",&taskHandle1));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandle1,"Dev1/ao0","",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxStartTask(taskHandle1));
    DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle1,1,10.0,data,NULL));


    DAQmxErrChk (DAQmxCreateTask("v2",&taskHandle2));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandle2,"Dev1/ao1","",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxStartTask(taskHandle2));
    DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle2,1,10.0,data,NULL));


    DAQmxErrChk (DAQmxCreateTask("v3",&taskHandle3));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandle3,"Dev1/ao2","",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxStartTask(taskHandle3));
    DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle3,1,10.0,data,NULL));


    DAQmxErrChk (DAQmxCreateTask("v4",&taskHandle4));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandle4,"Dev1/ao3","",-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxStartTask(taskHandle4));
    DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle4,1,10.0,data,NULL));


    while(!stopped)
    {
        if (update_channel != 0)
        {
            DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle1,1,10.0,voltage1,NULL));
            DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle2,1,10.0,voltage2,NULL));
            DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle3,1,10.0,voltage3,NULL));
            DAQmxErrChk (DAQmxWriteAnalogScalarF64(taskHandle4,1,10.0,voltage4,NULL));
            update_channel = 0;
        }
        QThread::msleep(1);
    }


    Error:
    if( DAQmxFailed(error) )
        DAQmxGetExtendedErrorInfo(errBuff,2048);
    if( taskHandle1!=0 )
    {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle1);
        DAQmxClearTask(taskHandle1);
    }

    if( taskHandle2!=0 )
    {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle2);
        DAQmxClearTask(taskHandle2);
    }

    if( taskHandle3!=0 )
    {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle3);
        DAQmxClearTask(taskHandle3);
    }

    if( taskHandle4!=0 )
    {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle4);
        DAQmxClearTask(taskHandle4);
    }

    if( DAQmxFailed(error) )
        qDebug() << "DAQmx Error: %s\n" << errBuff;
}


void NI6343::set_voltage(uint8_t channel, float voltage)
{

    update_channel = channel;
    switch (channel)
    {
        case 1: voltage1=voltage; break;
        case 2: voltage2=voltage; break;
        case 3: voltage3=voltage; break;
        case 4: voltage4=voltage; break;
        default: qDebug() << "NI6343 volatage out no commond"; break;
    }
}

int NI6343::find_closest(float arr[], int n, float target)
{
    int left = 0, right = n - 1;
    while (left < right) {
        if (abs(arr[left] - target)
            <= abs(arr[right] - target)) {
            right--;
        }
        else {
            left++;
        }
    }
    return left; //arr[left];
}

void NI6343::set_mp285_speed(int s1, int s2, int s3, bool enable)
{

    int s1_index = find_closest(cal_speed,69,s1);
    int s2_index = find_closest(cal_speed,69,s2);
    int s3_index = find_closest(cal_speed,69,s3);

    voltage1 = cal_voltage[s1_index];
    voltage2 = cal_voltage[s2_index];
    voltage3 = cal_voltage[s3_index];

    if (enable==false)
        voltage4 = 0.0f;
    else
        voltage4 = 5.0f;

    qDebug() << "s123 to v123: "<<s1<<s2<<s3<<voltage1<<voltage2<<voltage3<<voltage4;

    update_channel = 5;

}

void NI6343::stop()
{
    set_mp285_speed(0, 0, 0, false);
    stopped = true;
}
