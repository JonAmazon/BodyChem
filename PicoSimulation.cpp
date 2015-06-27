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

	updateCounter = 0;
	timeStep = 0.01;
	timeMultiplier = 10000;
	_timeAccum = 0.0;
	_mouseSensitivity = 0.5;

	mealTimer = 360.0;
	mutateTimer = 14400.0;

	fitnessMetric = 0.0;
	bestFitnessMetric = 10000000.0;

	random = new stdRan(0);
	dstreams.initialize();
	jon.initialize();
	pancreas.initialize();

	//TEMPORARY FOR TESTING
	_cam._position.setToZero();

	_cam._distance = 15.0;
	_cam._azimuth = 0.0;
	_cam._elevation = 25.0;

	_cam._FoV = 75.0;
	/////////////////////
}

float PicoSimulation::fitnessMetricFunction(float s)
{
	if(s < 30.0)
	{
		return 100000000.0;
	}
	else if(s < 80.0)
	{
		return 2.0*(s-80.0);
	}
	else if(s > 120.0)
	{
		return s-120.0;
	}
	else{return 0.0;}
}

void PicoSimulation::mutateFilters()
{
	float *basal = pancreas.getBasalFilter();
	float *meal = pancreas.getMealFilter();

	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		basal[L] += 0.005*(2.0*random->doub()-1.0);
		meal[L] += 0.005*(2.0*random->doub()-1.0);
	}
}

void PicoSimulation::update(PicoInput* xinput,float dtin)
{
	float mealCarbs = 100.0*(random->doub()),mealIndex = 0.5*(random->doub());

	_timeAccum += dtin;
	if(_timeAccum > 0.1){_timeAccum = 0.1;}
	
	while(_timeAccum > 0.0)
	{
		jon.update(timeStep);
		pancreas.update(jon.getSensorSugar(),timeStep);
		fitnessMetric += fitnessMetricFunction(jon.getBloodSugar())*timeStep;
		updateCounter++;
		mealTimer -= timeStep;
		mutateTimer -= timeStep;
		_timeAccum -= timeStep/timeMultiplier;
	}
	dstreams.pushDataToStreams(&jon);
	//printf("Simulation Time: %f - sensor sugar: %f\n",updateCounter*timeStep,jon.getBloodSugar());

	if(mealTimer < 0.0)
	{
		jon.eatMeal(mealCarbs,mealIndex);
		pancreas.mealFlag();
		mealTimer = 360.0;
	}

	if(pancreas.bolusIsPending())
	{
		jon.bolusInsulin(pancreas.bolusAmount());
		pancreas.bolusSuccessful();
	}

	if(mutateTimer < 0.0)
	{
		printf("fitness metric: %f\n",fitnessMetric);
		if(fitnessMetric < bestFitnessMetric)
		{
			bestFitnessMetric = fitnessMetric;
			bestPancreas.copyFiltersFrom(&pancreas);
		}

		pancreas.initialize();
		pancreas.copyFiltersFrom(&bestPancreas);
		mutateFilters();
		jon.initialize(); //resets to normal values

		fitnessMetric = 0.0;
		mutateTimer = 14400.0;
	}

	if(xinput->getKeyWasPressed(VK_SPACE))
	{
	}

	if(xinput->getKeyWasPressed(VK_CONTROL))
	{
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