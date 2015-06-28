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

	mealFlag = 0;
	mealDose = 0.0;
	mealFactor = 0.0;
	mealTimeConst = 30.0;

	activeInsulin = 0.0;
	activeInsulinTimeConst = 60.0;
	activeInsulinCoeff = 1.0;

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
		pendingBolusAmount += (basalFilter[L] + mealFactor*mealFilter[L])*(sensorLog[(sensorLogIdx-1-L+AP_SENSORLOG_LENGTH)%AP_SENSORLOG_LENGTH]);
	}
	pendingBolusAmount /= AP_SENSORLOG_LENGTH;
	
	if(mealFlag)
	{
		pendingBolusAmount += mealDose;
		mealFlag = 0;
	}
	pendingBolusAmount -= activeInsulinCoeff*activeInsulin;
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

	//activeInsulinDynamics
	activeInsulin -= (activeInsulin/activeInsulinTimeConst)*dt;
}

void ArtificialPancreas::copyFiltersFrom(ArtificialPancreas *other)
{
	float *basalFilterO = other->getBasalFilter();
	float *mealFilterO = other->getMealFilter();
	float *insulinCoeffO = other->getActiveInsulinCoeff();
	float *mealDoseO = other->getMealDose();

	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		basalFilter[L] = basalFilterO[L];
		mealFilter[L] = mealFilterO[L];
		
	}
	activeInsulinCoeff = insulinCoeffO[0];
	mealDose = mealDoseO[0];
}

void ArtificialPancreas::printFiltersToFile(const char *fname)
{
	FILE *fout = fopen(fname,"w");
	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		fprintf(fout,"%f\n",basalFilter[L]);
	}
	fprintf(fout,"\n");
	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		fprintf(fout,"%f\n",mealFilter[L]);
	}
	fprintf(fout,"\n");
	fprintf(fout,"%f\n",activeInsulinCoeff);
	fprintf(fout,"\n");
	fprintf(fout,"%f\n",mealDose);

	fclose(fout);
}

void ArtificialPancreas::readFromFile(const char *fname)
{
	FILE *fin = fopen(fname,"r");
	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		fscanf(fin,"%f\n",&basalFilter[L]);
	}
	for(int L = 0; L < AP_SENSORLOG_LENGTH; ++L)
	{
		fscanf(fin,"%f\n",&mealFilter[L]);
	}

	fscanf(fin,"%f\n",&activeInsulinCoeff);
	fscanf(fin,"%f\n",&mealDose);
}