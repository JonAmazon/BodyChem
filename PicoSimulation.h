#ifndef _H_PICOSIMULATION
#define _H_PICOSIMULATION

#include "PicoGlobal.h"
#include "PicoInput.h"
#include "PicoVec.h"
#include "StatRand.h"
#include "BodyChem.h"
#include "ArtificialPancreas.h"

#define DATA_STREAM_SIZE 256

struct DataStreams
{
	int stomachContentsIdx;
	int latentInsulinIdx;
	int bloodSugarIdx;
	int bloodInsulinIdx;
	int sensorSugarIdx;

	float stomachContentsStream[DATA_STREAM_SIZE];
	float latentInsulinStream[DATA_STREAM_SIZE];
	float bloodSugarStream[DATA_STREAM_SIZE];
	float bloodInsulinStream[DATA_STREAM_SIZE];
	float sensorSugarStream[DATA_STREAM_SIZE];

	void initialize();
	void pushDataToStreams(BodyChem *bchem);
};

struct PicoCamera
{
	float _FoV,_distance;
	float _azimuth,_elevation;
	PicoVec4f _position;
};

class PicoSimulation
{
private:
	int updateCounter,timeMultiplier;
	float _timeAccum;
	float timeStep;
	float _mouseSensitivity;

	float mealTimer;
	float mutateTimer;
	float fitnessMetric,bestFitnessMetric;

	stdRan *random;
	DataStreams dstreams;
	BodyChem jon;
	ArtificialPancreas pancreas;
	ArtificialPancreas bestPancreas;

	PicoCamera _cam;

	float fitnessMetricFunction(float s);
	void mutateFilters();
public:
	void initialize();
	void update(PicoInput* xinput,float dt);
	void release();

	PicoCamera* getxCamera(){return &_cam;}
	DataStreams* getDataStreams(){return &dstreams;}
};

#endif