//#include <vector>
//#include <string>
//#include <cstdio>
//#include <deque>
//#include <iostream>
//#include <fstream>
//
//
//std::vector<uint8_t> charToInt(const std::vector<char> &chars) {
//    std::vector<uint8_t> ints;
//    ints.reserve(chars.size());
//    for (const auto &ch: chars) {
//        ints.push_back(static_cast<int>(ch));
//    }
//    return ints;
//}
//
//
////用来将三电平序列转化为符号序列
//std::vector<uint8_t> levelToChar(std::vector<int> levels) {
//    std::vector<char> chars;
//    std::string map[3][3] = {{"000", "001", "010"},
//                             {"011", "",    "100"},
//                             {"101", "110", "111"}};
//    int length = (int) levels.size();
//    for (int i = 0; i < length - 1; i += 2) {
//        int x = levels[i] + 1;
//        int y = levels[i + 1] + 1;
//        std::string symbol = map[x][y];
//        for (const auto &ch: symbol) {
//            chars.push_back(ch);
//        }
//    }
//    return charToInt(chars);
//}
//
//
////用来打印符号序列
//void printChar(const std::vector<char> &chars) {
//    for (const auto &ch: chars) {
//        std::cout << ch << ", ";
//    }
//    std::cout << std::endl;
//}
//
//void printUint(const std::vector<uint8_t> &uints) {
//    for (const auto &uint: uints) {
//        std::cout << uint << ", ";
//    }
//    std::cout << std::endl;
//}
//
//
//char checkPoints(int *points) {
//    int length = 6;
//    //电压 1, -1, -1, 1, 1, 0 or 1, -1, -1, 1, 1, 1
//    // true 电压 -1 1  1 -1  0 -1
//    if (points[0] == -1 && points[1] == 1 && points[2] == 1 && points[3] == -1 && points[4] == 0) {
//        if (points[5] == -1) {
//            return 't';
//        } else {
//            return 'f';
//        }
//    } else {
//        return 'f';
//    }
//}
//
//char checkDeque(const std::deque<int> &points) {
//    if (points[0] == -1 && points[1] == 1 && points[2] == 1 && points[3] == -1 && points[4] == 0) {
//        if (points[5] == -1) {
//            return 't';
//        } else {
//            return 'f';
//        }
//    } else {
//        return 'f';
//    }
//}
//
//////定义一个函数，用来将三电平序列转化为符号序列
////int *point2bio(int *points) {
////    if (1 == 1)
////        return NULL;
////}
//
//
//int main() {
//    double highThreshold = 0.05; // mV
//    double lowThreshold = -0.05; // mv
//
//    std::vector<uint8_t> result;
//
//
//    std::deque<int> points(6); // 定义一个容量为6的deque
//    int tmp;
//    int k = 0; //标识deque是否刷新完毕
//    bool start = false;
//
//    std::ifstream file(R"(C:\Users\garlicv\projects\CLionProjects\shiboqi\test.txt)");
//    std::string line;
//
//    while (getline(file, line)) {
//        double point = strtod(line.c_str(), NULL);
////        cout << point << endl;
//        if (highThreshold <= point) {
//            tmp = 1;
//        } else if (lowThreshold <= point && point < highThreshold) {
//            tmp = 0;
//        } else if (point < lowThreshold) {
//            tmp = -1;
//        }
//
//        if (!start) {
//            if (checkDeque(points) == 't') {
//                printf("find start,start pam3");
//                start = true;
//            } else {
//                points.pop_front(); // 删除第一个元素
//                points.push_back(tmp); // 在尾部添加一个元素
//                k++;
//            }
//        }
//        if (start) {
//            if (k == 6) {
//                std::vector<int> levels(points.begin(), points.end());
//                //将三电平序列转化为符号序列
//                std::vector<uint8_t> symbols_pam3 = levelToChar(levels);
//
//                result.insert(result.end(), symbols_pam3.begin(), symbols_pam3.end());
//
//                printUint(symbols_pam3);
//                k = 0;
//            }
//            points.pop_front(); // 删除第一个元素
//            points.push_back(tmp); // 在尾部添加一个元素
//            k++;
//        }
//    }
//
//    std::cout << result.size() << std::endl;
//
//    return 0;
//}