#include "BodyChem.h"

void StomachChem::initialize()
{
	stomachContents = 0.0; //grams
	indexOfContents = 0.0;
	gastricExponent = 1.0;
	gastricTimeConst = 10.0; //min

	carbToBloodSugarRatio = 5.0; //(mg/dL)/gram

	bloodSugarChange = 0.0; //mg/dL
}

void StomachChem::eatMeal(float carbs, float index)
{
	float fractionalChange = carbs/(stomachContents + carbs);

	indexOfContents = fractionalChange*index + (1.0 - fractionalChange)*indexOfContents;
	gastricExponent = expf(-indexOfContents/0.5);
	gastricTimeConst = (10.0-1.0)*expf(-indexOfContents/0.5) + 1.0;
	stomachContents += carbs;
}

void StomachChem::update(float dt)
{
	float stomachRate = 0.0;

	//Stomach Dynamics
	stomachRate += -powf(stomachContents,gastricExponent)/gastricTimeConst;

	stomachContents += stomachRate*dt;
	if(stomachContents < 0.0)
	{
		stomachContents = 0.0; 
		indexOfContents = 0.0;
		gastricExponent = 1.0;
		gastricTimeConst = 10.0;
	}
	bloodSugarChange = (-stomachRate*dt)*carbToBloodSugarRatio;
}

void InjectionSiteChem::initialize()
{
	latentInsulin = 0.0; //units

	latentInsulinTimeConst = 40; //min

	bloodInsulinChange = 0.0; //units
}

void InjectionSiteChem::update(float dt)
{
	float latentInsulinRate = 0.0;

	//Injection Site Dynamics
	latentInsulinRate = -latentInsulin/latentInsulinTimeConst;

	latentInsulin += latentInsulinRate*dt;
	bloodInsulinChange = (-latentInsulinRate*dt);
}

void BloodChem::initialize()
{
	bloodSugar = 80.0; //mg/dL
	bloodInsulin = 0.0; //units

	renalThreshold = 200; //mg/dL
	renalTimeConst = 360; //min
	liverThreshold = 500; //mg/dL
	liverToSugarRate = 0.75; //(mg/dL)/min
	insulinTimeConst = 5; //min
	sugarInsulinRateConst = 8; //(mg/dL)/(min*units)
}

void BloodChem::update(StomachChem *stomach, InjectionSiteChem *site, float dt)
{
	float bloodSugarRate = 0.0, bloodInsulinRate = 0.0;

	bloodSugar += stomach->getBloodSugarChange();
	bloodInsulin += site->getBloodInsulinChange();

	//Blood Sugar Dynamics
	if(bloodSugar > renalThreshold)
	{
		bloodSugarRate += -(bloodSugar - renalThreshold)/renalTimeConst;
	}
	if(bloodSugar < liverThreshold)
	{
		bloodSugarRate += liverToSugarRate;
	}
	bloodSugarRate += -bloodInsulin*sugarInsulinRateConst;

	//Insulin Dynamics
	bloodInsulinRate += -bloodInsulin/insulinTimeConst;

	bloodSugar += bloodSugarRate*dt;
	bloodInsulin += bloodInsulinRate*dt;
}

void SensorSiteChem::initialize()
{
	sensorSugar = 16.0; //AU

	sensorSugarTimeConst = 15; //min
	bloodToSensorRatio = 0.2; //AU/(mg/dL)
}

void SensorSiteChem::update(BloodChem *blood, float dt)
{
	float sensorSugarRate = 0.0;
	float currentBloodSugar = blood->getBloodSugar();

	//Sensor Sugar Dynamics
	sensorSugarRate += -(sensorSugar - bloodToSensorRatio*currentBloodSugar)/sensorSugarTimeConst;

	sensorSugar += sensorSugarRate*dt;
}

void BodyChem::initialize()
{
	stomachChem.initialize();
	injectionSiteChem.initialize();
	bloodChem.initialize();
	sensorSiteChem.initialize();
}

void BodyChem::update(float dt)
{
	stomachChem.update(dt);
	injectionSiteChem.update(dt);
	bloodChem.update(&stomachChem,&injectionSiteChem,dt);
	sensorSiteChem.update(&bloodChem,dt);
}

