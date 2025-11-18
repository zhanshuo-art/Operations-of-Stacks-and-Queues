#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <string>
#include <vector>

using namespace std;
//第一问
// 存储 DeepCosineAI.csv 中的完整记录
struct CosineRecord {
    vector<string> all_fields; // 存储整行的所有字段
    double similarity;         // 用于判断的相似度
};

int main() {
    // ========== 第18行：文件路径设置区域 ==========
    string deepdive_cited_path = "E:\\InnovationDataset\\DeepInnovationAI\\DeepDiveAI_Cited.csv";
    string cosine_path = "E:\\InnovationDataset\\DeepCosineAI.csv";
    // ============================================

    const int CITED_THRESHOLD = 50;
    const int QUEUE_LIMIT = 1000;
    const double HIGH_SIMILARITY = 0.50;
    const double MEDIUM_SIMILARITY = 0.40;

    // 队列：存储待评估的论文ID
    queue<string> evaluation_queue;

    // 栈：存储待复审的记录（相似度 0.40 < x <= 0.50）
    stack<CosineRecord> review_stack;

    // 栈：存储成功转化的记录（相似度 > 0.50）
    stack<CosineRecord> success_stack;

    // ========== 第一步：入队审核 ==========
    cout << "开始读取 DeepDiveAI_Cited.csv 并入队..." << endl;
    ifstream deepdive_file(deepdive_cited_path);

    string line;
    getline(deepdive_file, line); // 跳过表头

    int count = 0;
    while (getline(deepdive_file, line) && count < QUEUE_LIMIT) {
        stringstream ss(line);
        string id, cited_str;

        getline(ss, id, ',');
        getline(ss, cited_str, ',');

        int cited = stoi(cited_str);
        if (cited > CITED_THRESHOLD) {
            evaluation_queue.push(id);
            count++;
        }
    }
    deepdive_file.close();
    cout << "入队完成，共入队 " << count << " 个论文ID" << endl;

    // ========== 第二步和第三步：出队处理和分流 ==========
    cout << "\n开始处理队列并分流..." << endl;

    int processed = 0;
    int success_count = 0;
    int review_count = 0;
    int ignored_count = 0;

    while (!evaluation_queue.empty()) {
        string paper_id = evaluation_queue.front();
        evaluation_queue.pop();

        // 在 DeepCosineAI.csv 中查找对应记录
        ifstream cosine_file(cosine_path);
        getline(cosine_file, line); // 跳过表头

        bool found = false;
        while (getline(cosine_file, line)) {
            stringstream ss(line);
            string field;
            vector<string> fields;

            while (getline(ss, field, ',')) {
                fields.push_back(field);
            }

            if (fields.size() >= 3) {
                string current_paper_id = fields[0];

                if (current_paper_id == paper_id) {
                    found = true;
                    CosineRecord record;
                    record.all_fields = fields;
                    record.similarity = stod(fields[2]);

                    // 分流处理
                    if (record.similarity > HIGH_SIMILARITY) {
                        success_stack.push(record);
                        success_count++;
                    } else if (record.similarity > MEDIUM_SIMILARITY) {
                        review_stack.push(record);
                        review_count++;
                    } else {
                        ignored_count++;
                    }
                    break;
                }
            }
        }

        if (!found) {
            ignored_count++;
        }

        cosine_file.close();
        processed++;

        if (processed % 100 == 0) {
            cout << "已处理 " << processed << " 个论文ID..." << endl;
        }
    }

    // ========== 输出统计结果 ==========
    cout << "\n========== 处理完成 ==========" << endl;
    cout << "总处理数: " << processed << endl;
    cout << "成功转化 (相似度 > 0.50): " << success_count << endl;
    cout << "待复审 (0.40 < 相似度 <= 0.50): " << review_count << endl;
    cout << "已忽略 (相似度 <= 0.40 或未找到): " << ignored_count << endl;

    return 0;
}
