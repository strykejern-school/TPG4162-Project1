#include "segy.h"

#include <stdio.h>
#include <stdlib.h>

segy::segy(
		const char *filename, 
		unsigned int segyHeaderSize, 
		unsigned int traceHeaderSize, 
		unsigned int numberOfTraces, 
		unsigned int dataPointsPerTrace){

	headerSize = segyHeaderSize;

	FILE *dataFile;
	dataFile = fopen(filename, "rb"); // Open segy file
	if (dataFile == NULL) throw 404; // File not found

	// Loading header
	header = (char*)malloc(headerSize * sizeof(char)); // Allocate memory for the header array
	if (fread(header, sizeof(char), headerSize, dataFile) != headerSize){
		fclose(dataFile);
		throw 200; // Error while loading segy header
	}

	// Loading traces
	traces = (trace*)malloc(numberOfTraces * sizeof(segy::trace)); // Allocate memory for the traces
	numTraces = numberOfTraces;
	for (unsigned int i = 0; i < numberOfTraces; i++){
		traces[i] = trace();

		// Loading trace header
		traces[i].header = (char*)malloc(traceHeaderSize * sizeof(char)); // Allocate memory for trace header
		traces[i].headerSize = traceHeaderSize;
		if (fread(traces[i].header, sizeof(char), traceHeaderSize, dataFile) != traceHeaderSize){
			fclose(dataFile);
			throw 201; // Error loading trace header
		}

		// Loading trace data points
		traces[i].dataPoints = (char*)malloc(dataPointsPerTrace * sizeof(char)); // Allocate memory for the trace data points
		traces[i].numDataPoints = dataPointsPerTrace;
		if (fread(traces[i].dataPoints, sizeof(char), dataPointsPerTrace, dataFile) != dataPointsPerTrace){
			fclose(dataFile);
			throw 202; // Error loading trace data points
		}
	}


	fclose(dataFile); // Close segy file
}

char** segy::getDataPoints(){
	static char **dataPoints;
    delete [] dataPoints;
    dataPoints = new char* [numTraces]; //(char**)malloc(numTraces * sizeof(char**)); // Allocate array of pointers to trace samples
	for (unsigned int i = 0; i < numTraces; i++){
		dataPoints[i] = traces[i].dataPoints; // Assign direct pointers to each trace's samples
	}
	return dataPoints;
}

// Deallocate data
void segy::deallocate(){
    free(header);
    
    for (unsigned int i = 0; i < numTraces; ++i){
        free(traces[i].header);
        free(traces[i].dataPoints);
    }
    
    free(traces);
}

