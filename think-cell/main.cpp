//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

//#include "llist.h"

#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

using namespace std;
#if 1

#include "Location_storage.h"

#elif 0
    // Create a map of strings to integers
    map<string, uint32_t> mp;

    // Insert some values into the map
    mp["one-two"] = 1;
    mp["two-tree"] = 3;
    mp["three-one"] = 5;

    // Iterate through the map and print the elements
    for (auto it = mp.begin(); it != mp.end(); ++it) {
        cout << "Start: " << it->first
             << ", End: " << it->second << endl;
    }
    cout << "------\n";
    cout << "one-two=" << mp["one-two"] << ", one-four=" << mp["one-four"] << endl;
    // mp["one-two"] = "three";

    cout << "------\n";

    for (auto it = mp.begin(); it != mp.end(); ++it) {
        cout << "Start: " << it->first
             << ", End: " << it->second << endl;
    }

#elif 1
void print_vector(vector <int> &v)
{
    for (int &i : v)
        cout << (i == v.front() ? "{":"") << i << (i == v.back() ? "}":", ");

    cout << endl;
}
int main()
{

    vector <int> v = {1,2,3,4,5};

    print_vector(v);
    for (auto i = v.begin(); i < v.end(); ++i)
    {
        if (*i == v[2])
            v.erase(i);
    }
    print_vector(v);
    return 0;
}


#elif 1
typedef uint32_t CameraId_t;
static vector<CameraId_t> collection= {123,4,345,2};
#define CAMERA_ID_NONE 0xFFFFFFFF
void print_collection()
{
    for (int i : collection)
        cout << i << ", ";
    cout << endl;
}

CameraId_t GetAvailableCamera()
{
  if (collection.empty())
    return CAMERA_ID_NONE;

  uint32_t idx  = rand() % collection.size();
  cout << idx << endl;
  return collection[idx];
}

uint32_t CheckoutById(CameraId_t id)
{
  if (collection.empty())
    return -1;

  for (auto &camid : collection)
  {
      if (id == camid)
      {
          camid = collection[collection.size()-1];
          collection.resize(collection.size()-1);
          // collection.erase(iterator);

          print_collection();
          return 0;
      }
  }

  for (uint32_t i = 0; i < collection.size(); ++i)
  {
    if (id == collection[i])
    {
       collection[i] = collection[collection.size()-1];
       collection.resize(collection.size()-1);
       return 0;
    }
  }
  return -2; // NOT FOUND

}
int CheckinById(CameraId_t cid)
{
  collection.push_back(cid);
  return 0;
}

void main1()
{
    srand(rand());
    print_collection();
    CameraId_t cid = GetAvailableCamera();
    CheckoutById(cid);
    CheckinById(cid);
    print_collection();
}

// power usage
// 0-300    500
// 300-500  20
// 500-1000 150
// Av0_700 = (300-0)*500 + (500-300)*20 + (700-500)*150 / HOUR_IN_SEC

#define HOUR_IN_SEC 1000 // (3600)
struct SomeBase
{
    uint32_t sec;
    uint32_t val;
};

class PowerUsageInfo_t : public SomeBase
{
public:
//    uint32_t sec;
//    uint32_t val;

    PowerUsageInfo_t(uint32_t s, uint32_t v) {sec = s; val = v;};
    PowerUsageInfo_t() = default;
};

float calc_pu(uint32_t st, uint32_t sec, vector<PowerUsageInfo_t> &v)
{
    uint32_t accum = 0;
    uint32_t pre_sec = 0;

    if (sec <= st)
        return 0;

    cout << "calc_pu(" << st << "," << sec << ")" << endl;

    for (auto &it : v)
    {
        if (st <= it.sec)
        {
            if (st > pre_sec)
            {
                if (sec <= it.sec) // in the same block
                    return (float)((sec-st)*it.val / HOUR_IN_SEC);
                else
                    accum += (it.sec - st) * it.val;
            }
            else
            {
                auto interval = [&](uint32_t s){return ((it.sec > s) ? s : it.sec);}(sec);
                accum += (interval - pre_sec) * it.val;
            }
        }
        pre_sec = it.sec;
        cout << accum << endl;
    }

    return (float)(accum / HOUR_IN_SEC);
}
void pu_update_now(uint32_t now_s, vector<PowerUsageInfo_t> &v)
{
    // simulated for test
    v.push_back(PowerUsageInfo_t(now_s+1, 500));
}

int main()
{

    main1();
    // lambda
//    int x = 3, l = 5
//    bool res = [](int val, int lim){return val > lim;}(5, x);
//    cout << "the res is " << res << endl;
//    cout << "the res is " << [&](){return x > l;}() << endl;
//    vector<int> v = {1,2,3,4,5};
//    for (auto i : v)
//        cout << "val = " << i << endl;
    vector<PowerUsageInfo_t> v = {{100,500}, {500,200}, {1000,300}};
    cout << "is_base_of<SomeBase, PowerUsageInfo_t> = " << std::is_base_of <SomeBase, PowerUsageInfo_t>() << endl;

    uint32_t now_s = 700;
    uint32_t start_s = 100;

    if (v.back().sec < now_s)
    {
        // call to update
        pu_update_now(now_s, v);
    }
    for (auto &i : v)
        cout << i.sec << " : " << i.val << (&i == &v.back() ? "" : ", ");
    cout << endl;

    auto res = calc_pu(start_s, now_s, v);
    cout << "power usage " << res << " mAmp" << endl;

    return 0;
}


#elif 1

int main()
{
#include <bits/types.h>
#include <stdio.h>
#define DEV_ADDR 0x12
#define STATUS_DATA 0x00
#define DATA1 0x01
#define CONTROL 0x02
#define COUNTER 10

int main() {
    // you can write to stdout for debugging purposes, e.g.
    printf("This is a debug message\n");

    uint8_t data, data1 = 0;
    BOOLEAN ret = FALSE;


    for (int i = 0; i < COUNTER; i++)
    {
        if (TRUE == (ret = Read_I2c(DEV_ADDR, STATUS_DATA, &data))
        {
            if (data & 0x1)
                break;
        }
    }
    if (!(ret == TRUE && (data & 0x1)))
        return -1;

    // set start capture
    data = (1 << 1); // 2
    if (TRUE != (ret = Write_I2c(DEV_ADDR, CONTROL, &data))
        return -2;

    for (int i = 0; i < COUNTER; i++)
    {
        if (TRUE == (ret = Read_I2c(DEV_ADDR, STATUS_DATA, &data))
        {
            if ((data >> 2) & 0x1)
                break;
        }
    }
    if (!(ret == TRUE && ((data >> 2) & 0x1))) // replace with macro
        return -1;

    if (FALSE == (ret = Read_I2c(DEV_ADDR, DATA1, &data1)))
        return -3; // todo:add number counter to try

    return 0;
}
#elif 1
int main()
{
    // simplesafe
    int *b = new int(24);
    cout << *b << endl;

    {
        std::shared_ptr<int> fp(b);
        cout << *fp << endl;
    }

    std::shared_ptr<int> sp(b);
    cout << *sp << endl;  // "0" and double free problem

    return 0;
}

#elif 1


/*
The formula for a simple discrete-time low-pass filter can be expressed as follows:
y[n]= α⋅x[n]+(1−α)⋅y[n−1]
Where:
- y[n] is the output signal at time index n
- x[n] is the input signal at time index n
- y[n−1] is the previous output value [at time index (n-1)]
- α is the filter coefficient
*/

/*
    std::vector<int> v = {1, 2,3, 4};
    v.push_back(33);

    for (auto el : v)
        cout << el << ", " ;
    cout << endl;

    for (auto it = v.end() - 1; it >= v.begin(); it--)
        cout << *it << " ";
    cout << endl;

 */

#include <iostream>

#define sayHI(name) "Hello dear "#name

class LowPassFilter {
public:
    LowPassFilter(float alpha) : alpha(alpha), prevOutput(0.0) {}

    float filter(float input) {

        //Calculate the output using the filter formula here
        //write your answer here

        // y[n]= α⋅x[n]+(1−α)⋅y[n−1]
        return prevOutput = alpha * input + (1-alpha) * prevOutput;
    }

private:
    float alpha;       // Filter coefficient (0 <= alpha <= 1)
    float prevOutput;  // Previous output value
};

int main() {
    // Create a low-pass filter with a specified alpha (cutoff frequency)
    float alpha = 0.2;
    LowPassFilter filter(alpha);

    // cout << sayHI(Robert) << endl;

    // Sample input signal (e.g., a sine wave)
    float inputSignal[] = {0.1, 0.2, 0.4, 0.6, 0.8, 1.0, 0.9, 0.7, 0.5, 0.3};
    // Desired output given alpha = 0.2
    float correctOutput[] = {0.02, 0.056, 0.1248, 0.21984, 0.335872, 0.468698, 0.554958, 0.583966, 0.567173, 0.513739};



    // Apply the low-pass filter to the input signal
    int signalLength = sizeof(inputSignal) / sizeof(inputSignal[0]);
    float filteredSignal[signalLength];



    for (int i = 0; i < signalLength; ++i) {
        filteredSignal[i] = filter.filter(inputSignal[i]);
    }



    // Display the desired output
    std::cout << "Desired Output:   ";
    for (int i = 0; i < signalLength; ++i) {
        std::cout << correctOutput[i] << " ";
    }

    std::cout << std::endl;

     // Display the filtered signal
    std::cout << "Filtered Signal:  ";
    for (int i = 0; i < signalLength; ++i) {
        std::cout << filteredSignal[i] << " ";
    }

    std::cout << std::endl;

     return 0;
}





#elif 1

int main()
{
    // Create a vector containing integers
    std::vector<int> v = {2, 4, 5, 9};
    std::vector<int> w = {1, 3, 4, 8};
    vector<vector<int>> lv = {v, w};

    // Add two more integers to vector
    v.push_back(99);
    w.push_back(88);

    // Overwrite element at position 2
    lv[0][2] = -1;
    lv[1][2] = -1;

    // Print out the vectors
    for (auto l : lv) {
        for (int n : l)
            std::cout << n << ' ';
        std::cout << '\n';
    }
}

#elif 1
//        cout << "begin:"
//             << (m_map.lower_bound(keyBegin))->first << "|"
//             << (m_map.upper_bound(keyBegin))->first << " end:"
//             << (m_map.lower_bound(keyEnd))->first << "|"
//             << (m_map.upper_bound(keyEnd))->first << endl;


template<typename K, typename V>
class interval_map {
    friend void IntervalMapTest();
    V m_valBegin;
    std::map<K,V> m_map;
public:
    // constructor associates whole range of K with val
    interval_map(V const& val)
    : m_valBegin(val)
    {}

    // look-up of the value associated with key
    V const& operator[]( K const& key ) const {
        auto it=m_map.upper_bound(key);
        if(it==m_map.begin()) {
            return m_valBegin;
        } else {
            return (--it)->second;
        }
    }

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    // Conforming to the C++ Standard Library conventions, the interval
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval,
    // and assign must do nothing.
    void assign( K const& keyBegin, K const& keyEnd, V const& val ) {
#ifdef QT_QML_DEBUG
        cout << "\nassign(" << keyBegin << "," << keyEnd << "," << val << ")\n";
#endif
        // check beg/end
        if (keyBegin >= keyEnd ) {
#ifdef QT_QML_DEBUG
            cout << "if !( keyBegin < keyEnd ), this designates an empty interval: " << keyBegin << "<" << keyEnd << endl;
#endif
            return;
        }

        auto it = m_map.lower_bound(keyBegin);
        if (it == m_map.begin()) {
            if (m_valBegin == val) {
                // check upper bound of new entry
                it = m_map.upper_bound(keyBegin);
                if (it != m_map.end() && it->first < keyEnd) {
                    auto nval = it->second;
                    m_map.erase(it);
                    m_map[keyEnd] = nval;
                }
#ifdef QT_QML_DEBUG
                else {
                    cout << "the first entry in m_map must not contain the same value as m_valBegin " << val << endl;
                }
                printf_map();
#endif
                return;
            }
            else {
                m_map[keyBegin] = val;
            }
        }
        else if (((it)->second != val) && ((--it)->second != val) && ((++it)->second != val)) {
            // consecutive map entries must not contain the same value
            m_map[keyBegin] = val;
        }

        // check lower bound of new entry
        it = m_map.upper_bound(keyBegin);
        if (it != m_map.end() && it->second == val && it->first > keyBegin) {
            m_map.erase(it);
            m_map[keyBegin] = val;
        }

        // check upper bound of new entry
        it = m_map.upper_bound(keyBegin);
        if (it != m_map.end() && it->first < keyEnd) {
            auto nval = it->second;
            m_map.erase(it);
            m_map[keyEnd] = nval;
        }

#ifdef QT_QML_DEBUG
        printf_map();
#endif
    }

    void printf_map() {
        cout << "map:-->";
        for (const auto& [key, value] : m_map)
            cout << '[' << key << "] = " << value << "; ";
        cout << "\n";
    }
};

// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.

int main()
{
    cout << "Hello, think-cell!" << endl;
    interval_map<int, char> im('A');

    im.assign(7, 12,'Z');
    im.assign(1, 3,'B');
    im.assign(5, 5,'A'); // test for begin < end
    im.assign(0, 1,'A'); // test for m_valBegin
    im.assign(0, 2,'A'); // test for m_valBegin
    im.assign(4, 8,'C');
    im.assign(3, 5,'C'); // test the same
    im.assign(3, 9,'C'); // test consecutive
    im.assign(6, 10,'C'); // test consecutive

    // here is the killer:
    im.assign(1, 12,'E');

    // test
    cout << "\n------------------\n";
    for (int i = -2; i < 13; i++)
    {
        cout << i << " = " << im[i] << endl;
    }

    return 0;
}

#endif
