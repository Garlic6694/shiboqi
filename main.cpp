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
    double voltage; // 表示采样电压
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

//// 查找并保存特定电压序列及后续片段的函数
//std::vector<std::vector<SamplePoint>> findAndSaveSequences(const std::vector<SamplePoint> &data) {
//    const std::vector<std::vector<int>> targetSequences = {
//            {1, -1, -1, 1, 1, 0},
//            {1, -1, -1, 1, 1, 1}
//    };
//    std::vector<std::vector<SamplePoint>> sequences;
//    std::vector<SamplePoint> currentSequence;
//    std::vector<SamplePoint> targetPointSequence;
//
//    for (size_t i = 0; i < data.size(); i++) {
//        for (const auto &targetSequence: targetSequences) {
////            targetPointSequence.clear();
//            if (data[i].voltage == targetSequence[0]) {
//                // 检查后续的电压序列是否与目标序列相匹配
//                size_t j = 1;
//                while (i + j < data.size() && j < targetSequence.size() && data[i + j].voltage == targetSequence[j]) {
//                    j++;
//                    targetPointSequence.push_back(data[i + j]);
//                }
//
//                // 如果找到完整的目标序列，继续匹配后一位为0或1的片段
//                if (j == targetSequence.size()) {
//                    size_t k = 1;
//                    while (i + j + k < data.size() && (data[i + j + k].voltage == 0 || data[i + j + k].voltage == 1)) {
//                        currentSequence.push_back(data[i + j + k]);
//                        k++;
//                    }
////                    currentSequence.insert(currentSequence.begin(), targetPointSequence.begin(), targetPointSequence.end());
//                    sequences.push_back(currentSequence);
//                    currentSequence.clear();
//                }
//            }
//        }
//    }
//
//    return sequences;
//}

// 查找并保存特定电压序列及后续片段的函数
std::vector<std::vector<SamplePoint>> findAndSaveSequences(const std::vector<SamplePoint> &data) {
    const std::vector<std::vector<int>> targetSequences = {
            {1, -1, -1, 1, 1, 0},
            {1, -1, -1, 1, 1, 1}
    };
    std::vector<std::vector<SamplePoint>> sequences;
    std::vector<SamplePoint> currentSequence;
    std::vector<SamplePoint> targetPointSequence;

    for (size_t i = 0; i < data.size(); i++) {
        for (const auto &targetSequence: targetSequences) {
            if (data[i].voltage == targetSequence[0]) {
                // 检查后续的电压序列是否与目标序列相匹配
                size_t j = 1;
                while (i + j < data.size() && j < targetSequence.size() && data[i + j].voltage == targetSequence[j]) {
                    j++;
                    targetPointSequence.push_back(data[i + j]);
                }

                // 如果找到完整的目标序列，保存该序列及后续片段
                if (j == targetSequence.size()) {
                    currentSequence.insert(currentSequence.begin(), targetPointSequence.begin(),
                                           targetPointSequence.end());
                    sequences.push_back(currentSequence);
                    currentSequence.clear();
                    targetPointSequence.clear();

                    // 保存目标序列后续的每个电压值和时间值
                    while (i + j < data.size() && data[i + j].voltage != targetSequence[0]) {
                        currentSequence.push_back(data[i + j]);
                        j++;
                    }
                }
            } else if (!currentSequence.empty()) {
                currentSequence.push_back(data[i]);
            }
        }
    }

    return sequences;
}


// 保存sequences到txt文件中
void saveSequencesToFile(const std::vector<std::vector<SamplePoint>> &sequences, const std::string &filename) {
    std::ofstream outputFile(filename);

    if (outputFile.is_open()) {
        for (const auto &sequence: sequences) {
            for (const auto &point: sequence) {
                outputFile << " " << point.voltage;
            }
            outputFile << "-----------\n";
        }
        outputFile.close();
        std::cout << "Sequences saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

int main() {
    setbuf(stdout, 0);

    // 设置高阈值和低阈值
    double highThreshold = 0.1;
    double lowThreshold = -0.1;

    // 从CSV文件中读取电压数据
    std::vector<SamplePoint> voltageData;
    FILE *inputFile = fopen("../CY4457_5G16M.csv", "r");

    if (inputFile) {
        // Get the size of the file
        fseek(inputFile, 0, SEEK_END);
        long fileSize = ftell(inputFile);
        printf("fileSize:%ld\n", fileSize);
        fseek(inputFile, 0, SEEK_SET);

        // Allocate memory to store the entire file content
        char *buffer = new char[fileSize + 1];
        if (!buffer) {
            std::cerr << "Failed to allocate memory for reading file." << std::endl;
            return 1;
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
        std::cerr << "Unable to open file: CY4457_5G16M.csv" << std::endl;
        return 1;
    }

    // 对电压数据进行采样和转换为离散的三电平数字电压
    std::vector<SamplePoint> digitalData = sampleAndConvertToDigital(voltageData, highThreshold, lowThreshold);

    // 打印结果

    int length = digitalData.size();
    int k = 0;
    for (auto &i: digitalData) {
        printf("%f %% ", k * 1.0 / length * 100);
        k++;
        printf("Time %f ,Voltage %f\n", i.time, i.voltage);
    }

    printf("@@@@@@@@@@@@@@@@hello world@@@@@@@@@@@@@@@@\n");


    // 保存离散的数字电压数据到CSV文件
    saveDigitalDataToFile(digitalData, "digital_data.csv");


    // 查找并保存特定电压序列及后续片段
    std::vector<std::vector<SamplePoint>> sequences = findAndSaveSequences(digitalData);

    // 保存sequences到txt文件
    saveSequencesToFile(sequences, "sequences.txt");

    // 输出找到的所有序列及后续片段的时间和电压
    std::cout << "Found sequences and subsequent fragments:" << std::endl;
    for (const auto &sequence: sequences) {
        for (const auto &point: sequence) {
            std::cout << "Time: " << point.time << ", Voltage: " << point.voltage << std::endl;
        }
        std::cout << "-----------" << std::endl;
    }

    return 0;
}
// 101      010         11
// 1,-1     ,-1 1,        1
// 1,-1,-1 1,1
