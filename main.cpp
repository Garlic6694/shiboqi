#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

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

// 程序A的功能
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

// 共享数据队列
std::queue<SamplePoint> sharedQueue;
std::mutex queueMutex;
std::condition_variable queueCV;

// 程序B：无休止发送数据
void sendData() {
    while (true) {
        for (const SamplePoint &point: voltageData) {
            // 发送数据操作（假设已实现）
//            std::cout << "Sending data: time " << point.time << ", voltage " << point.voltage << std::endl;

            // 将数据加入共享队列
            std::lock_guard<std::mutex> lock(queueMutex);
            sharedQueue.push(point);
            queueCV.notify_one();

            // 暂停 10 纳秒
            std::this_thread::sleep_for(std::chrono::nanoseconds(10000000));
        }
    }
}


int main() {
    // 设置高阈值和低阈值
    double highThreshold = 0.1;
    double lowThreshold = -0.1;

    // 创建发送数据的线程
    std::thread senderThread(sendData);

    // 接收数据并进行处理
    std::vector<SamplePoint> result;
    int counter = 0;

    while (true) {
        // 从共享队列中取出数据
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCV.wait(lock, [] { return !sharedQueue.empty(); });

        // 处理数据
        SamplePoint point = sharedQueue.front();
        sharedQueue.pop();


        int digitalVoltage = convertToDigitalVoltage(point.voltage, highThreshold, lowThreshold);
        std::cout << point.time << " " << digitalVoltage << std::endl;


//        // 按照采样间隔进行采样
//        if (counter % SAMPLE_INTERVAL == 0) {
//            counter = 0;
//            SamplePoint sample{};
//            int digitalVoltage = convertToDigitalVoltage(point.voltage, highThreshold, lowThreshold);
//            sample.voltage = static_cast<double>(digitalVoltage);
//            sample.time = point.time;
//            result.push_back(sample);
//            // 输出处理后的数据
//            std::cout << "Processed data: voltage " << sample.voltage << ", time " << sample.time << std::endl;
//        }
//        counter++;

        // 退出循环的条件
        if (sharedQueue.empty() && senderThread.joinable()) {
            break;
        }
    }

    senderThread.join(); // 等待发送数据的线程结束

    return 0;
}
