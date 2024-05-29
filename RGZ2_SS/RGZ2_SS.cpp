#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <windows.h>

using namespace std;

// Функция для разбивания строки на слова
vector<string> split(const string& str) {
    vector<string> words;
    string word;
    for (char c : str) {
        if (isspace(static_cast<unsigned char>(c))) {  // приведение к unsigned char
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

// Функция для подсчета количества пар слов
unordered_map<string, int> countPairs(const vector<string>& words) {
    unordered_map<string, int> pairsCount;
    if (words.size() < 2) {
        return pairsCount;  // Возвращаем пустой map, если недостаточно слов для пар
    }
    for (size_t i = 0; i < words.size() - 1; ++i) {
        string pair = words[i] + " " + words[i + 1];
        pairsCount[pair]++;
    }
    return pairsCount;
}

// Функция для подсчета количества троек слов
unordered_map<string, int> countTriplets(const vector<string>& words) {
    unordered_map<string, int> tripletsCount;
    if (words.size() < 3) {
        return tripletsCount;  // Возвращаем пустой map, если недостаточно слов для троек
    }
    for (size_t i = 0; i < words.size() - 2; ++i) {
        string triplet = words[i] + " " + words[i + 1] + " " + words[i + 2];
        tripletsCount[triplet]++;
    }
    return tripletsCount;
}

// Функция для подсчета релевантности текста в файле
double calculateRelevance(const vector<string>& fileWords, const unordered_map<string, int>& queryPairsCount, const unordered_map<string, int>& queryTripletsCount) {
    if (fileWords.empty()) {
        return 0.0;  // Возвращаем 0 релевантности, если файл пустой
    }

    unordered_map<string, int> filePairsCount = countPairs(fileWords);
    unordered_map<string, int> fileTripletsCount = countTriplets(fileWords);

    double pairsRelevance = 0.0;
    for (const auto& pair : queryPairsCount) {
        double frequency = static_cast<double>(filePairsCount[pair.first]) / (fileWords.size() - 1);
        pairsRelevance += frequency * 0.4;
    }

    double tripletsRelevance = 0.0;
    for (const auto& triplet : queryTripletsCount) {
        double frequency = static_cast<double>(fileTripletsCount[triplet.first]) / (fileWords.size() - 2);
        tripletsRelevance += frequency * 0.6;
    }

    return pairsRelevance + tripletsRelevance;
}

// Функция для чтения слов из файла
vector<string> readWordsFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return {};
    }
    string word;
    vector<string> words;

    while (file >> word) {
        words.push_back(word);
    }

    return words;
}

int main() {
    // Установка кодовой страницы для корректного отображения русских и украинских символов
    SetConsoleOutputCP(1251);

    while (true) {
        // Заданные файлы для поиска
        vector<string> filenames = { "file1.txt", "file2.txt", "file3.txt" };

        // Заданный пользователем текст для поиска
        string queryText;
        cout << "\nВведите текст для поиска: ";
        getline(cin, queryText);

        if (queryText.empty()) {
            cerr << "Ошибка: Поисковый текст не должен быть пустым" << endl;
            return 1;
        }

        vector<string> queryWords = split(queryText);
        unordered_map<string, int> queryPairsCount = countPairs(queryWords);
        unordered_map<string, int> queryTripletsCount = countTriplets(queryWords);

        if (queryPairsCount.empty() && queryTripletsCount.empty()) {
            cerr << "Ошибка: Поисковый текст должен содержать хотя бы два слова для поиска пар или три слова для поиска троек" << endl;
            return 1;
        }

        bool anyRelevant = false;

        for (const auto& filename : filenames) {
            vector<string> fileWords = readWordsFromFile(filename);
            if (fileWords.empty()) {
                cerr << "Файл " << filename << " пуст или не может быть прочитан." << endl;
                continue;
            }
            double relevance = calculateRelevance(fileWords, queryPairsCount, queryTripletsCount);
            if (relevance > 0.0) {
                anyRelevant = true;
                cout << "Релевантность для файла " << filename << ": " << relevance << endl;
            }
            else {
                cout << "Поисковый текст не найден в файле " << filename << endl;
            }
        }

        if (!anyRelevant) {
            cout << "Поисковый текст не найден ни в одном из файлов." << endl;
        }
    }
    return 0;
}