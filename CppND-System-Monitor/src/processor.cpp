#include "processor.h"
#include "linux_parser"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  Processor_usage = (LinuxParser::Jiffies() - LinuxParser::IdleJiffies()) /
                    LinuxParser::Jiffies();
  Processor_usage *= 100;
	return Processor_usage;
}