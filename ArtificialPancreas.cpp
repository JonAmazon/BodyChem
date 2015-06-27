#include "ArtificialPancreas.h"

void ArtificialPancreas::initialize()
{
	sensorLogIdx = 0;
	sensorSamplerNum = 0;
	sensorSampler = 0.0;
	sensorLogTimer = 10.0;
	sensorSamplerTimer = 0.5;

	bolusTimer = 10.0;
	bolusReady = 0;
	pendingBolusAmount = 0.0;

	mealFactor = 0.0;
	mealTimeConst = 30.0;

	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		sensorLog[L] = 16.0;
		basalFilter[L] = 0.0;
		mealFilter[L] = 0.0;
	}
}

void ArtificialPancreas::calculatePendingBolus()
{
	pendingBolusAmount = 0.0;

	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		pendingBolusAmount += ((1.0-mealFactor)*basalFilter[L] + mealFactor*mealFilter[L])*(sensorLog[(sensorLogIdx + L)%AP_SENSORLOG_LENGTH]);
	}
	pendingBolusAmount /= AP_SENSORLOG_LENGTH;
	pendingBolusAmount += 0.183;
	if(pendingBolusAmount < 0.0){pendingBolusAmount = 0.0;}
}

void ArtificialPancreas::update(float sensorReading,float dt)
{
	sensorSamplerTimer -= dt;
	if(sensorSamplerTimer < 0.0)
	{
		sensorSampler += sensorReading;
		sensorSamplerNum++;
		sensorSamplerTimer = 0.5;
	}

	sensorLogTimer -= dt;
	if(sensorLogTimer < 0.0)
	{
		sensorLog[sensorLogIdx] = (sensorSampler/sensorSamplerNum);
		sensorLogIdx = (sensorLogIdx + 1)%AP_SENSORLOG_LENGTH;
		sensorSampler = 0.0;
		sensorSamplerNum = 0;

		sensorLogTimer = 10.0;
	}

	bolusTimer -= dt;
	if(bolusTimer < 0.0)
	{
		calculatePendingBolus();
		bolusReady = 1;

		bolusTimer = 10.0;
	}

	//Meal Factor Dynamics
	mealFactor -= (mealFactor/mealTimeConst)*dt;
}

void ArtificialPancreas::copyFiltersFrom(ArtificialPancreas *other)
{
	float *basalFilterO = other->getBasalFilter();
	float *mealFilterO = other->getMealFilter();
	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		basalFilter[L] = basalFilterO[L];
		mealFilter[L] = mealFilterO[L];
	}
}