#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <thread>

typedef const std::string string;
// 采样间隔
const int SAMPLE_INTERVAL = 75;
// 前后额外采样点数量
const int EXTRA_SAMPLES = 10;

// 数据结构，表示采样点
struct SamplePoint {
    double time; // 表示采样时间
    double voltage; //表示采样电压
};

// 模拟电压数据
const std::vector<SamplePoint> voltageData = {
        {-0.0016,       -0.004688},
        {-0.0015999998, -0.000781},
        {-0.0015999996, -0.001758},
        {-0.0015999994, -0.00625},
        {-0.0015999992, -0.006055},
        {-0.001599999,  -0.002539},
        {-0.0015999988, -0.001367},
        {-0.0015999986, -0.001563},
        {-0.0015999984, -0.00332},
        {-0.0015999982, -0.008203},
        {-0.001599998,  -0.011133},
        {-0.0015999978, -0.010547},
        {-0.0015999976, -0.010156},
        {-0.0015999974, -0.009961},
        {-0.0015999972, -0.009961},
        {-0.001599997,  -0.011719},
        {-0.0015999968, -0.012695},
        {-0.0015999966, -0.012109},
        {-0.0015999964, -0.014844},
        {-0.0015999962, -0.020117},
        {-0.001599996,  -0.024219},
        {-0.0015999958, -0.024609},
        {-0.0015999956, -0.02207},
        {-0.0015999954, -0.02168},
        {-0.0015999952, -0.025},
        {-0.001599995,  -0.026562},
        {-0.0015999948, -0.025977},
        {-0.0015999946, -0.029492},
        {-0.0015999944, -0.034766},
        {-0.0015999942, -0.035156},
        {-0.001599994,  -0.037109},
        {-0.0015999938, -0.043555},
        {-0.0015999936, -0.04668},
        {-0.0015999934, -0.04668},
        {-0.0015999932, -0.049414},
        {-0.001599993,  -0.051953},
        {-0.0015999928, -0.051758},
        {-0.0015999926, -0.054688},
        {-0.0015999924, -0.060938},
        {-0.0015999922, -0.0625},
        {-0.001599992,  -0.063281},
        {-0.0015999918, -0.067578},
        {-0.0015999916, -0.067969},
        {-0.0015999914, -0.067188},
        {-0.0015999912, -0.075},
        {-0.001599991,  -0.083594},
        {-0.0015999908, -0.083594},
        {-0.0015999906, -0.084961},
        {-0.0015999904, -0.092188},
        {-0.0015999902, -0.096289},
        {-0.00159999,   -0.094922},
        {-0.0015999898, -0.096484},
        {-0.0015999896, -0.102148},
        {-0.0015999894, -0.105664},
        {-0.0015999892, -0.108984},
        {-0.001599989,  -0.112695},
        {-0.0015999888, -0.112891},
        {-0.0015999886, -0.114844},
        {-0.0015999884, -0.120312},
        {-0.0015999882, -0.124414},
        {-0.001599988,  -0.127734},
        {-0.0015999878, -0.130859},
        {-0.0015999876, -0.132812},
        {-0.0015999874, -0.135352},
        {-0.0015999872, -0.139258},
        {-0.001599987,  -0.144336},
        {-0.0015999868, -0.148633},
        {-0.0015999866, -0.150391},
        {-0.0015999864, -0.152344},
        {-0.0015999862, -0.154102},
        {-0.001599986,  -0.154492},
        {-0.0015999858, -0.158008},
        {-0.0015999856, -0.164648},
        {-0.0015999854, -0.167969},
        {-0.0015999852, -0.167383},
        {-0.001599985,  -0.168555},
        {-0.0015999848, -0.171875},
        {-0.0015999846, -0.174805},
        {-0.0015999844, -0.176758},
        {-0.0015999842, -0.17793},
        {-0.001599984,  -0.179297},
        {-0.0015999838, -0.181055},
        {-0.0015999836, -0.181641},
        {-0.0015999834, -0.183398},
        {-0.0015999832, -0.183789},
        {-0.001599983,  -0.182031},
        {-0.0015999828, -0.185742},
        {-0.0015999826, -0.191797},
        {-0.0015999824, -0.191406},
        {-0.0015999822, -0.189453},
        {-0.001599982,  -0.190625},
        {-0.0015999818, -0.19082},
        {-0.0015999816, -0.189844},
        {-0.0015999814, -0.188672},
        {-0.0015999812, -0.188086},
        {-0.001599981,  -0.190234},
        {-0.0015999808, -0.192969},
        {-0.0015999806, -0.192383},
        {-0.0015999804, -0.189648},
        {-0.0015999802, -0.191406},
        {-0.00159998,   -0.194922}
};

// 通过阈值将模拟电压转换为离散的三电平数字电压
int convertToDigitalVoltage(double voltage, double highThreshold, double lowThreshold) {
    if (voltage > highThreshold) {
        return 1;
    } else if (voltage < lowThreshold) {
        return -1;
    } else {
        return 0;
    }
}

// 采样并转换为离散的三电平数字电压
std::vector<SamplePoint>
sampleAndConvertToDigital(const std::vector<SamplePoint> &data, double highThreshold, double lowThreshold) {
    std::vector<SamplePoint> digitalData;

    for (int i = 0; i < data.size(); i += SAMPLE_INTERVAL) {
        SamplePoint samplePoint;

        double sumVoltage = 0.0;

        int count = 0;

        for (int j = i - EXTRA_SAMPLES; j <= i + EXTRA_SAMPLES; ++j) {
            if (j >= 0 && j < data.size()) {
                sumVoltage += data[j].voltage;
                count++;
            }
        }

        double averageVoltage = sumVoltage / count;
        int digitalVoltage = convertToDigitalVoltage(averageVoltage, highThreshold, lowThreshold);

        samplePoint.voltage = (double) digitalVoltage;
        samplePoint.time = data[i].time;

        digitalData.push_back(samplePoint);

    }

    return digitalData;
}

// 保存离散的数字电压数据和采样时间到CSV文件
void saveDigitalDataToFile(const std::vector<SamplePoint> &digitalData, string &filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (auto i: digitalData) {
            file << i.time << "," << i.voltage << "\n";
        }
        file.close();
        std::cout << "Digital data saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

int main() {
    // 设置高阈值和低阈值
    double highThreshold = 0.1;
    double lowThreshold = -0.1;

    int k = 1000000000;

//    while (true) {
//        for (const SamplePoint &point: voltageData) {
//            // 发送数据操作（假设已实现）
//            std::cout << "time:" << point.time << "  volt:" << point.voltage << std::endl;
//
//            // 暂停 10 纳秒
//            std::this_thread::sleep_for(std::chrono::nanoseconds(10));
//        }
//        k--;
//        if (k == 0) {
//            break;
//        }
//    }


    std::vector<SamplePoint> digitalData = sampleAndConvertToDigital(voltageData, highThreshold, lowThreshold);

    for (auto &i: digitalData) {
        std::cout << "data " << i.voltage << " " << i.time << std::endl;
    }

    saveDigitalDataToFile(digitalData, "digital_data.csv");

    return 0;
}