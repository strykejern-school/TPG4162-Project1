#ifndef SEGY_H
#define SEGY_H

class segy{
public:
	struct trace{
		char *header;
		unsigned int headerSize;
        
		char *dataPoints;
		unsigned int numDataPoints;
	};
private:
	char *header;
	unsigned int headerSize;

	trace *traces;
	unsigned int numTraces;

public:
	segy(
		const char *filename, 
		unsigned int headerSize, 
		unsigned int traceHeaderSize, 
		unsigned int numberOfTraces, 
		unsigned int dataPointsPerTrace);

	char** getDataPoints();
	unsigned int dataPointColumns() { return numTraces; }
	unsigned int dataPointRows() { return traces[0].numDataPoints; }
    void deallocate();
};

#endif