#include<stdlib.h>
#include<stdio.h>
#include<math.h>

class StomachChem
{
private:
	float stomachContents;
	float indexOfContents;
	float gastricExponent;
	float gastricTimeConst;

	float carbToBloodSugarRatio;

	float bloodSugarChange;
public:
	void initialize();
	void update(float dt);
	void release();
	void eatMeal(float carbs, float index);

	float getStomachContents(){return stomachContents;}
	float getBloodSugarChange(){return bloodSugarChange;}
};

class InjectionSiteChem
{
private:
	float latentInsulin;

	float latentInsulinTimeConst;

	float bloodInsulinChange;
public:
	void initialize();
	void update(float dt);
	void release();
	void bolus(float units){latentInsulin += units;}

	float getLatentInsulin(){return latentInsulin;}
	float getBloodInsulinChange(){return bloodInsulinChange;}
};

class BloodChem
{
private:
	float bloodSugar;
	float bloodInsulin;

	float renalThreshold;
	float renalTimeConst;
	float insulinTimeConst;
	float sugarInsulinRateConst;
public:
	void initialize();
	void update(StomachChem *stomach, InjectionSiteChem *site, float dt);
	void release();

	float getBloodSugar(){return bloodSugar;}
	float getBloodInsulin(){return bloodInsulin;}
};

class SensorSiteChem
{
private:
	float sensorSugar;

	float sensorSugarTimeConst;
	float bloodToSensorRatio;
public:
	void initialize();
	void update(BloodChem *blood, float dt);
	void release();

	float getSensorSugar(){return sensorSugar*bloodToSensorRatio;}
};

class BodyChem
{
private:
	BloodChem bloodChem;
	StomachChem stomachChem;
	InjectionSiteChem injectionSiteChem;
	SensorSiteChem sensorSiteChem;
public:
	void initialize();
	void update(float dt);
	void release();
	void eatMeal(float carbs, float index){stomachChem.eatMeal(carbs,index);}
	void bolusInsulin(float units){injectionSiteChem.bolus(units);}

	float getStomachContents(){return stomachChem.getStomachContents();}
	float getLatentInsulin(){return injectionSiteChem.getLatentInsulin();}
	float getBloodSugar(){return bloodChem.getBloodSugar();}
	float getBloodInsulin(){return bloodChem.getBloodInsulin();}
	float getSensorSugar(){return sensorSiteChem.getSensorSugar();}
};