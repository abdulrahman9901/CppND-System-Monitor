#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key;
  float total_mem, free_mem;
  std::ifstream mem_file(kProcDirectory + kVersionFilename);
  if (mem_file.is_open()) {
    getline(mem_file, line);
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream mem_stream(line);
    mem_file >> key >> total_mem;
    getline(mem_file, line);
    std::replace(line.begin(), line.end(), ':', ' ');
    mem_file >> key >> free_mem;
  }
  return (total_mem - free_mem);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, time;
  long Uptime_in_sec;
  std::ifstream time_file(kProcDirectory + kUptimeFilename);
  if (time_file.is_open()) {
    getline(time_file, line);
    std::istringstream time_stream(line);
    time_stream >> time;
    Uptime_in_sec = long(stof(time));
  }
  return Uptime_in_sec;
}
vector<long int> LinuxParser::CpuUtilization() {
  string value, line, key;
  vector<long> jiffies_list;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      if (key != "cpu") {
        jiffies_list.push_back(stol(key));
      }
    }
  }
  return jiffies_list;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  int long total_jiffs = 0;
  vector<long> all_jiffies = CpuUtilization();
  std::for_each(all_jiffies.begin(), all_jiffies.end(),
                [&](int n) { total_jiffs += n; });
  return total_jiffs;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string pid_s = to_string(pid);
  long total_time;
  std::ifstream pid_file(kProcDirectory + pid_s + kVersionFilename);
  if (pid_file.is_open()) {
    std::getline(pid_file, line);
    std::istringstream buf(line);
    std::istream_iterator<std::string> begin(buf), end;
    std::vector<std::string> values(begin, end);
    float utime = stof(values[13]);
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    total_time = long(utime + stime + cutime + cstime);
  }
  return total_time;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  int long active_jiffs = 0;
  vector<long> all_jiffies = CpuUtilization();
  for (auto i = 0; i < all_jiffies.size(); ++i)
    if (i != kIdle_ && i != kIOwait_) active_jiffs += all_jiffies.at(i);
  return active_jiffs;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<long> all_jiffies = CpuUtilization();
  return (all_jiffies.at(kIdle_) + all_jiffies.at(kIOwait_));
}

// TODO: Read and return CPU utilization

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key;
  long value;
  std::ifstream cpu_file(kProcDirectory + kStatFilename);
  if (cpu_file.is_open()) {
    while (getline(cpu_file, line)) {
      std::istringstream cpu_stream(line);
      cpu_stream >> key >> value;
      if (key == "processes") return value;
    }
  }
}
// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key;
  long value;
  std::ifstream cpu_file(kProcDirectory + kStatFilename);
  if (cpu_file.is_open()) {
    while (getline(cpu_file, line)) {
      std::istringstream cpu_stream(line);
      cpu_stream >> key >> value;
      if (key == "procs_running") return value;
    }
  }
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string pid_s = to_string(pid), line;
  std::ifstream pid_file(kProcDirectory + pid_s + kCmdlineFilename);
  if (pid_file.is_open()) {
    std::getline(pid_file, line);
    return line;
  }
}
// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, kb;
  string pid_s = to_string(pid);
  long value;
  std::ifstream pid_file(kProcDirectory + pid_s + kStatusFilename);
  if (pid_file.is_open()) {
    while (std::getline(pid_file, line)) {
      std::istringstream pid_stream(line);
      pid_stream >> key >> value >> kb;
      if (key == "VmSize") {
        value = value / 1000;
        return to_string(value);
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value;
  string pid_s = to_string(pid);
  std::ifstream pid_file(kProcDirectory + pid_s + kStatusFilename);
  if (pid_file.is_open()) {
    while (std::getline(pid_file, line)) {
      std::istringstream pid_stream(line);
      while (pid_stream >> key >> value)
        if (key == "Uid") return value;
    }
  }
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream pid_file(kPasswordPath);
  string name = "x:" + Uid(pid), line, result;
  if (pid_file.is_open()) {
    while (std::getline(pid_file, line)) {
      if (line.find(name) != std::string::npos) {
        result = line.substr(0, line.find(":"));
        return result;
      }
    }
  }
  return " ";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string pid_s = to_string(pid), line;
  std::ifstream pid_file(kProcDirectory + pid_s + kVersionFilename);
  if (pid_file.is_open()) {
    std::getline(pid_file, line);
    std::istringstream buf(line);
    std::istream_iterator<std::string> begin(buf), end;
    std::vector<std::string> values(begin, end);
    return (LinuxParser::UpTime() -
            long(stof(values[21]) / sysconf(_SC_CLK_TCK)));
  }
}
