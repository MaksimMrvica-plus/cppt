#include <iostream>
#include <vector>
#include "a.h"
using namespace std;
// 打印数组
void printArray(const vector<int>& arr) {
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;
}
// 分区函数：将数组分为两部分，返回基准元素的正确位置
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high]; // 选择最后一个元素作为基准
    int i = low - 1;       // i 是较小元素的索引

    for (int j = low; j < high; j++) {
        // 如果当前元素小于或等于基准
        if (arr[j] <= pivot) {
            i++; // 增加较小元素的索引
            cout << 'i' << arr[i] << " j" << arr[j]<<endl; 
            swap(arr[i], arr[j]); // 交换 arr[i] 和 arr[j]


        }
                    cout<< "-\t";
            printArray(arr);
    }
    // 将基准元素放到正确的位置
    swap(arr[i + 1], arr[high]);
    cout<< "-\t";
    
    printArray(arr);
    return i + 1; // 返回基准元素的索引
}

// 快速排序递归函数
void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        // 找到分区点
        int pi = partition(arr, low, high);

        // 递归排序左半部分
        quickSort(arr, low, pi - 1);

        // 递归排序右半部分
        quickSort(arr, pi + 1, high);
    }
}



// int main() {
//     vector<int> arr = {9,6,4,3,1,2,5,7,10,0};
//     int n = arr.size();

//     cout << "Original array: ";
//     printArray(arr);

//     quickSort(arr, 0, n - 1);

//     cout << "Sorted array: ";
//     printArray(arr);
//     ssd::myFcn(5);
//     return 0;
// }
