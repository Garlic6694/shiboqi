#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>


// 重载+号运算符
template<typename T>
std::vector<T> &operator+(std::vector<T> &v1, std::vector<T> &v2) {
    v1.insert(v1.end(), v2.begin(), v2.end());
    return v1;
}

struct SamplePoint {
    double time;
    double voltage;
};

// 均值滤波函数
std::vector<double> meanFilter(const std::vector<SamplePoint> &data, int windowSize) {
    int n = data.size();
    std::vector<double> smoothedData(n, 0.0);

    for (int i = 0; i < n; ++i) {
        int start = std::max(0, i - windowSize / 2);
        int end = std::min(n - 1, i + windowSize / 2);

        double sum = 0.0;
        for (int j = start; j <= end; ++j) {
            sum += data[j].voltage;
        }

        smoothedData[i] = sum / (end - start + 1);
    }

    return smoothedData;
}

// 寻找第一个波峰位置的函数
int findFirstPeakSlidingWindow(const std::vector<double> &smoothedData, int windowSize) {
    int n = smoothedData.size();

    for (int i = 1; i < n - 1; ++i) {
        bool isPeak = true;

        // 在当前窗口内查找峰值
        for (int j = i + 1; j < i + windowSize; ++j) {
            if (smoothedData[j] <= smoothedData[j - 1]) {
                isPeak = false;
                break;
            }
        }

        if (isPeak) {
            return i + windowSize / 2; // 返回窗口中间位置作为峰值位置
        }
    }

    return -1; // 如果没有找到峰值，则返回-1表示失败
}


// 将采样后的数据转换为新的数据格式
std::vector<SamplePoint> transformData(const std::vector<SamplePoint> &sampledData,
                                       double highThreshold, double lowThreshold) {
    std::vector<SamplePoint> transformedData;

    for (const auto &point: sampledData) {
        double voltage = 0.0;
        if (point.voltage > highThreshold) {
            voltage = 1.0; // 设置为1，表示电压大于高阈值
        } else if (point.voltage < lowThreshold) {
            voltage = -1.0; // 设置为-1，表示电压小于低阈值
        } else {
            voltage = 0.0; // 设置为0，表示电压在高低阈值之间
        }
        transformedData.push_back({point.time, voltage});
    }

    return transformedData;
}


// 采样函数，按照指定采样间隔对数据进行采样
std::vector<SamplePoint> sampleData(const std::vector<SamplePoint> &data, int samplingInterval, int peakPosition) {
    std::vector<SamplePoint> sampledData;

    int currentIndex = peakPosition;
    while (currentIndex < data.size()) {
        sampledData.push_back(data[currentIndex]);
        currentIndex += samplingInterval;
    }

    return sampledData;
}

// 读取文件获取电压数据
std::vector<SamplePoint> readVoltageDataFromFile(const std::string &filename) {
    std::vector<SamplePoint> voltageData;

    FILE *inputFile = fopen(filename.c_str(), "r");
    if (inputFile) {
        // Get the size of the file
        fseek(inputFile, 0, SEEK_END);
        long fileSize = ftell(inputFile);
        printf("fileSize:%ld\n", fileSize);
        fseek(inputFile, 0, SEEK_SET);

        // Allocate memory to store the entire file content
        char *buffer = new char[fileSize + 1];
        if (!buffer) {
            fprintf(stderr, "Failed to allocate memory for reading file.\n");
        }

        // Read the entire file into the buffer
        size_t bytesRead = fread(buffer, 1, fileSize, inputFile);
        buffer[bytesRead] = '\0'; // Null-terminate the buffer

        // Close the file
        fclose(inputFile);

        // Process the data in the buffer
        std::istringstream iss(buffer);
        std::string line;
        while (std::getline(iss, line)) {
            std::istringstream lineStream(line);
            SamplePoint sample{};
            char comma;
            if (lineStream >> sample.time >> comma >> sample.voltage) {
                voltageData.push_back(sample);
            }
        }

        // Free the allocated memory
        delete[] buffer;
    } else {
        fprintf(stderr, "Unable to open file: sample_data.csv\n");
    }

    return voltageData;
}

// Function to split transformedData with the given pattern
std::vector<std::vector<SamplePoint>>
splitData(const std::vector<SamplePoint> &transformedData, const std::vector<double> &pattern) {
    int patternSize = pattern.size();
    int dataIndex = 0;
    std::vector<std::vector<SamplePoint>> segments;
    std::vector<SamplePoint> currentSegment;

    for (const auto &point: transformedData) {
        currentSegment.push_back(point);

        if (dataIndex < patternSize && pattern[dataIndex] == point.voltage) {
            dataIndex++;
        }

        // Check if the pattern is complete, and if so, add the segment to the list and reset
        if (dataIndex == patternSize) {
            segments.push_back(currentSegment);
            currentSegment.clear();
            dataIndex = 0;
        }
    }

    // If there's any remaining data in the current segment, add it to the list
    if (!currentSegment.empty()) {
        segments.push_back(currentSegment);
    }

    return segments;
}

int main() {

    std::cout << std::setprecision(32); // 设置输出精度


    std::string filename = "../CY4457_5G16M.csv";
    int windowSize = 75; // 设置滑动窗口的大小，根据数据特性调整
    int samplingInterval = 75; // 设置采样间隔

    // 从CSV文件中读取电压数据
    std::vector<SamplePoint> voltageData = readVoltageDataFromFile(filename);

    // 均值滤波处理数据
    std::vector<double> smoothedData = meanFilter(voltageData, windowSize);

    // 寻找第一个波峰位置
    int peakPosition = findFirstPeakSlidingWindow(smoothedData, windowSize);

    std::vector<SamplePoint> sampledData;

    if (peakPosition != -1) {
        // 输出第一个波峰的位置信息
        printf("First peak detected at index: %d\n", peakPosition);
        printf("Time: %lf\n", voltageData[peakPosition].time);
        printf("Voltage: %lf\n", voltageData[peakPosition].voltage);

        // 按照采样间隔对voltageData进行采样
        sampledData = sampleData(voltageData, samplingInterval, peakPosition);

        // 输出采样后的数据
        printf("%s", "Sampled Data:\n");
        for (const auto &point: sampledData) {
            printf("Time: %.16lf,Voltage: %.16lf\n", point.time, point.voltage);
        }

    } else {
        printf("No peak detected.\n");
    }


    double highThreshold = 0.1;
    double lowThreshold = -0.1;

    // 转换采样后的数据
    std::vector<SamplePoint> transformedData = transformData(sampledData, highThreshold, lowThreshold);

    // 输出采样后的数据
    printf("Sampled Data:\n");
    for (const auto &point: sampledData) {
        printf("Time: %.16lf,Voltage: %.16lf\n", point.time, point.voltage);
    }

    // 定义给定的序列
    std::vector<double> pattern = {1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1,
                                   -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1, -1, 1, 1,
                                   -1, -1, 1, 1, -1, 0, -1};

    // 使用函数进行处理
    std::vector<std::vector<SamplePoint>> segments = splitData(transformedData, pattern);


    printf("\nsegments size %zu\n", segments.size());

    // 打印分割后的数据
    printf("\nSegments of Transformed Data:\n");
    for (const auto &segment: segments) {
        for (const auto &point: segment) {
            printf("%.16lf, %.16lf\n", point.time, point.voltage);
        }
        printf("\n");
    }

    return 0;

}
