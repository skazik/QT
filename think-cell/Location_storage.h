#ifndef LOCATION_STORAGE_H
#define LOCATION_STORAGE_H
#include <iostream>
#include <vector>
#include <map>

/*We are adding a new feature to the Dispatch system at Axon, which will allow us
to determine the average travel time of police units between different
locations.

Implement a DispatchTravel class that is initialized with a List of data points
related to dispatched police units (unitId, location, timestamp), and that is
able to return the AVERAGE travel time from any one location to another through
the following method:
    getAverageTime(string startLocation, string endLocation)

The average travel time is computed from all trips that happened directly FROM
startLocation TO endLocation.

Input format:
    [[unitId, location, timestamp], [unitId, location, timestamp], ...]

When a Unit ID is first encountered, it means the unit was dispatched and a
trip started from that location and timestamp. When that same Unit ID is
encountered again, it means the trip ended at that location and timestamp. The
travel time is the difference in timestamps.

Example:
input = [
    [1, "SW Station", 3],
    [2, "Clovis", 4],
    [3, "SW Station", 5],
    [1, "Mayfair", 10],
    [3, "Mayfair", 20],
    [2, "Downtown Fresno", 25]
  ]
dt = DispatchTravel(input)
dt.getAverageTime("SW Station", "Mayfair") -------> (7+15)/2 = 11.0
dt.getAverageTime("Clovis", "Downtown Fresno") -------> 21.0

Explanation:
- Unit with ID 1 travels from SW Station at timestamp 3 to Mayfair at timestamp
  10 (Travel time = 7)
- Unit with ID 3 travels from SW Station at timestamp 5 to Mayfair at timestamp
  20 (Travel time = 15)
--> Average travel time from SW Station to Mayfair = 11.0 (average of the 2 trips)

- Unit with ID 2 travels from Clovis at timestamp 4 to Downtown Fresno at timestamp
  25 (Travel time = 21)
--> Average travel time from Clovis to Downtown Fresno = 21.0 (only 1 trip recorded)
*/

using namespace std;

struct DispInfo_t {
  uint32_t unitId;
  string location;
  uint32_t timestamp;
};

class Dispatch {
    struct StorageInfo_t {
        map<string, uint32_t>  collection;
        map<string, uint32_t>  counter;
    } storage;
public:
    void print_db()
    {
        cout << "------\n";
        for (auto it = storage.collection.begin(); it != storage.collection.end(); ++it) {
            cout << "Trip: " << it->first
                 << ", Duration: " << it->second << endl;
        }
        cout << "------\n";
    }

    int DispatchTravel(vector <DispInfo_t>  &input)
    {
      // EXPECTED: SHOULD be sorted by unitId + timestamp
      // EXPECTED: unitId is alwas > 0
      uint32_t uId = 0;
      string start_location = "";
      for (auto it = input.begin(); it != input.end(); ++it)
      {
          if (it->unitId != uId)
          {
              uId = it->unitId;
              string trip = it->location;
              auto next = std::next(it);
              if (next == input.end() || next->unitId != uId) {
                  cout << "...skip trip " << trip << endl;
                  continue;
              }
              uint32_t start_ts = it->timestamp;

              ++it;
              trip += "-";
              trip += it->location;
              uint32_t average = storage.collection[trip];
              uint32_t count = storage.counter[trip];
              cout << trip << " average=" << average << " now=" << (it->timestamp-start_ts) << endl;
              if (count)
                  storage.collection[trip] = ((it->timestamp-start_ts) + average * count) / (count+1);
              else
                  storage.collection[trip] = (it->timestamp-start_ts);
              storage.counter[trip] = ++count;
              cout << trip << "= " << storage.collection[trip] << ", count= " << count << endl;

              trip = "";
              uId = 0;
          }
      }
      print_db();
      return 0; // OK
    }

    uint32_t getAverageTime(string startLocation, string endLocation)
    {
      return storage.collection[startLocation + "-" + endLocation];
    }
};

// usage example - test
int main() {
  // EXPECTED: SHOULD be sorted by unitId + timestamp
  vector <DispInfo_t> input =
  {
      {1, "SW Station", 3},
      {1, "Mayfair", 10},
      {2, "Clovis", 4},
      {2, "Downtown Fresno", 25},
      {2, "Downtown Fresno", 30},
      {2, "Clovis", 40},
      {2, "SW Station", 45},
      {2, "Mayfair", 85},
      {2, "Mayfair", 125},
      {3, "SW Station", 5},
      {3, "Mayfair", 10},
  };

  Dispatch d;

  // NB: sort by unitId + timestamp before calling DispathTravel
  d.DispatchTravel(input);
  cout << "getAverageTime(Clovis-Mayfair) = " << d.getAverageTime("Clovis","Mayfair") << endl;
  cout << "getAverageTime(Clovis-Downtown Fresno) = " << d.getAverageTime("Clovis","Downtown Fresno") << endl << endl;
  return 0;
}

#endif // LOCATION_STORAGE_H
