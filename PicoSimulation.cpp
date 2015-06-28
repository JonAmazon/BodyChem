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
	timeStep = 0.1;
	timeMultiplier = 1000000;
	_timeAccum = 0.0;
	_mouseSensitivity = 0.5;

	mealCount = 0;
	mealTimerReset = 1440*10;
	mutateTimerReset = 400000*10;
	mealTimer = mealTimerReset;
	mutateTimer = mutateTimerReset;

	fitnessMetric = 0.0;
	bestFitnessMetric = 1000000.0;

	random = new stdRan(0);
	dstreams.initialize();
	jon.initialize();
	pancreas.initialize();
	pancreas.readFromFile("pancreasParams.txt");
	bestPancreas.initialize();

	//TEMPORARY FOR TESTING
	_cam._position.setToZero();

	_cam._distance = 15.0;
	_cam._azimuth = 0.0;
	_cam._elevation = 25.0;

	_cam._FoV = 75.0;
	/////////////////////
}

float square(float a)
{
	return a*a;
}

float PicoSimulation::fitnessMetricFunction(float s)
{
	if(s < 40.0)
	{
		return 50.0*square(s-80.0);
	}
	else if(s < 60.0)
	{
		return 10.0*square(s-80.0);
	}
	if(s < 80.0)
	{
		return 2.0*square(s-80.0);
	}
	else{return square(s-80.0);}
}

void PicoSimulation::mutateFilters()
{
	int filter = 4*(random->doub());
	float mutationRate = 0.01;
	float *basal = pancreas.getBasalFilter();
	float *meal = pancreas.getMealFilter();
	float *insulinCoeff = pancreas.getActiveInsulinCoeff();
	float *mealDose = pancreas.getMealDose();

	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		switch(filter)
		{
		case 0:
			{
				basal[L] += mutationRate*(2.0*random->doub()-1.0);
				break;
			}
		case 1:
			{
				meal[L] += mutationRate*(2.0*random->doub()-1.0);
				break;
			}
		case 2:
			{
				if(L == 0)
				{
					//insulinCoeff[L] += mutationRate*(2.0*random->doub()-1.0);
				}
				break;
			}
		case 3:
			{
				if(L == 0)
				{
					mealDose[L] += mutationRate*(2.0*random->doub()-1.0);
				}
				break;
			}
		}
	}
}

void PicoSimulation::update(PicoInput* xinput,float dtin)
{
	float mealCarbs = 60.0,mealIndex = 1.5*(random->doub());
	float pTry;

	_timeAccum += dtin;
	if(_timeAccum > 0.1){_timeAccum = 0.1;}
	
	for(int n = 0; n < timeMultiplier; ++n)
	{
		jon.update(timeStep);
		pancreas.update(jon.getSensorSugar(),timeStep);
		fitnessMetric += fitnessMetricFunction(jon.getBloodSugar())*timeStep;
		updateCounter++;
		mealTimer--;
		mutateTimer--;

		if(mealTimer == 0)
		{
			jon.eatMeal(mealCarbs,mealIndex);
			pancreas.prepareForMeal();
			mealCount++;
			mealTimer = mealTimerReset;
		}

		if(pancreas.bolusIsPending())
		{
			jon.bolusInsulin(pancreas.bolusAmount());
			pancreas.bolusSuccessful();
		}

		if(mutateTimer == 0)
		{
			fitnessMetric /= mutateTimerReset*timeStep;
			pTry = random->doub();

			printf("Fitness Metric: %f %d\n",fitnessMetric,mealCount);

			if(pTry < expf(-(fitnessMetric-bestFitnessMetric)/10.0))
			{
				bestFitnessMetric = fitnessMetric;
				bestPancreas.copyFiltersFrom(&pancreas);
				bestPancreas.printFiltersToFile("pancreasParams.txt");
				printf("fitness metric: %f - New Best Found\n",fitnessMetric,bestFitnessMetric);
			}

			pancreas.initialize();
			pancreas.copyFiltersFrom(&bestPancreas);
			mutateFilters();
			jon.initialize(); //resets to normal values

			fitnessMetric = 0.0;
			mealCount = 0;
			mutateTimer = mutateTimerReset;
		}
	}
	dstreams.pushDataToStreams(&jon);

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