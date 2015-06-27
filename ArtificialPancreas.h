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
	int bolusReady;

	float sensorSampler;
	float sensorLogTimer;
	float sensorSamplerTimer;
	float bolusTimer;

	float pendingBolusAmount;
	float mealFactor;
	float mealTimeConst;

	float sensorLog[AP_SENSORLOG_LENGTH];
	float basalFilter[AP_SENSORLOG_LENGTH];
	float mealFilter[AP_SENSORLOG_LENGTH];

	void calculatePendingBolus();
public:
	void initialize();
	void update(float sensorReading,float dt);
	void release();

	float getSensorLog(int idx){return sensorLog[(idx)%AP_SENSORLOG_LENGTH];}
	float* getBasalFilter(){return basalFilter;}
	float* getMealFilter(){return mealFilter;}

	int bolusIsPending(){return bolusReady;}
	float bolusAmount(){return pendingBolusAmount;}
	void bolusSuccessful(){bolusReady = 0;}
	void mealFlag(){mealFactor = 1.0;}

	void copyFiltersFrom(ArtificialPancreas *other);
	void mutateFilters();
};

#endif