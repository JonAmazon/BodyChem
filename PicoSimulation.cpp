#include "PicoSimulation.h"

void DataStreams::initialize()
{
	stomachContentsIdx = 0;
	latentInsulinIdx = 0;
	bloodSugarIdx = 0;
	bloodInsulinIdx = 0;
	sensorSugarIdx = 0;

	for(int i = 0; i < DATA_STREAM_SIZE; ++i)
	{
		stomachContentsStream[i] = 0;
		latentInsulinStream[i] = 0;
		bloodSugarStream[i] = 0;
		bloodInsulinStream[i] = 0;
		sensorSugarStream[i] = 0;
	}
}

void DataStreams::pushDataToStreams(BodyChem *bchem)
{
	stomachContentsStream[stomachContentsIdx] = bchem->getStomachContents();
	latentInsulinStream[latentInsulinIdx] = bchem->getLatentInsulin();
	bloodSugarStream[bloodSugarIdx] = bchem->getBloodSugar();
	bloodInsulinStream[bloodInsulinIdx] = bchem->getBloodInsulin();
	sensorSugarStream[sensorSugarIdx] = bchem->getSensorSugar();

	stomachContentsIdx = (stomachContentsIdx+1)%DATA_STREAM_SIZE;
	latentInsulinIdx = (latentInsulinIdx+1)%DATA_STREAM_SIZE;
	bloodSugarIdx = (bloodSugarIdx+1)%DATA_STREAM_SIZE;
	bloodInsulinIdx = (bloodInsulinIdx+1)%DATA_STREAM_SIZE;
	sensorSugarIdx = (sensorSugarIdx+1)%DATA_STREAM_SIZE;
}

void PicoSimulation::initialize()
{
    char bufferA[256];
    char bufferB[256];

	_timeAccum = 0.0;
	_mouseSensitivity = 0.5;

	dstreams.initialize();
	jon.initialize();

	//TEMPORARY FOR TESTING
	_cam._position.setToZero();

	_cam._distance = 15.0;
	_cam._azimuth = 0.0;
	_cam._elevation = 25.0;

	_cam._FoV = 75.0;
	/////////////////////
}

void PicoSimulation::update(PicoInput* xinput,float dtin)
{
	_timeAccum += dtin;
	if(_timeAccum > 0.1){_timeAccum = 0.1;}

	while(_timeAccum > 0.0)
	{
		jon.update(0.01);
		_timeAccum -= (0.01)/20.0;
	}
	dstreams.pushDataToStreams(&jon);

	if(xinput->getKeyWasPressed(VK_SPACE))
	{
		jon.eatMeal(100.0,0.3);
	}

	if(xinput->getKeyWasPressed(VK_CONTROL))
	{
		jon.bolusInsulin(1.0);
	}

    if(xinput->getKeyState(VK_RBUTTON))
    {
	    _cam._azimuth += _mouseSensitivity*(-xinput->getDeltaMposX());
	    _cam._elevation += _mouseSensitivity*(-xinput->getDeltaMposY());
	    _cam._distance += -0.5*xinput->getDeltaMwheel();
    }
}

void PicoSimulation::release()
{
}