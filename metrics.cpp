#include "metrics.h"

using namespace std;

Metrics::Metrics() {
	// Constructor for metrics. Sets all the counters to 0.
    TransmissionCount = 0;
    CollisionCount = 0;
    SuccessCount = 0;
    DroppedCount = 0;
    TotalPacketCount = 0;
    SimulationTime = 0;
}