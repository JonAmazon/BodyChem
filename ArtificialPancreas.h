#ifndef _H_ARTIFICIAL_PANCREAS
#define _H_ARTIFICIAL_PANCREAS

#include <cstdlib>
#include <cstdio>
#include <cmath>

#define AP_SENSORLOG_LENGTH 16

class ArtificialPancreas
{
private:
	int sensorLogIdx;
	int sensorSamplerNum;
	int mealFlag;
	int bolusReady;

	float sensorSampler;
	float sensorLogTimer;
	float sensorSamplerTimer;
	float bolusTimer;

	float pendingBolusAmount;
	float mealDose;
	float mealFactor;
	float mealTimeConst;
	float activeInsulin;
	float activeInsulinTimeConst;

	float sensorLog[AP_SENSORLOG_LENGTH];
	float basalFilter[AP_SENSORLOG_LENGTH];
	float mealFilter[AP_SENSORLOG_LENGTH];
	float activeInsulinCoeff;

	void calculatePendingBolus();
public:
	void initialize();
	void update(float sensorReading,float dt);
	void release();

	float getSensorLog(int idx){return sensorLog[(idx)%AP_SENSORLOG_LENGTH];}
	float* getBasalFilter(){return basalFilter;}
	float* getMealFilter(){return mealFilter;}
	float* getActiveInsulinCoeff(){return &activeInsulinCoeff;}
	float* getMealDose(){return &mealDose;}

	int bolusIsPending(){return bolusReady;}
	float bolusAmount(){return pendingBolusAmount;}
	void bolusSuccessful(){bolusReady = 0; activeInsulin += pendingBolusAmount;}
	void prepareForMeal(){mealFactor = 1.0; mealFlag = 1;}

	void copyFiltersFrom(ArtificialPancreas *other);
	void printFiltersToFile(const char *fname);
	void readFromFile(const char *fname);
};

#endif