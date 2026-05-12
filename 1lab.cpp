#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

class File {
private:
    string filename;
    string date;
    int size;
    
public:
    File(string f, string d, int s) : filename(f), date(d), size(s) {}
    
    string getFilename() const { return filename; }
    string getDate() const { return date; }
    int getSize() const { return size; }
};

class FileManager {
private:
    vector<File> files;
    
public:
    FileManager() {
        files.push_back(File("document.txt", "2023.01.15", 1024));
        files.push_back(File("image.jpg", "2023.02.20", 204800));
        files.push_back(File("video.mp4", "2023.03.10", 5242880));
    }
    
    void addFile(const string& filename, const string& date, int size) {
        files.push_back(File(filename, date, size));
        cout << "\nФайл добавлен\n";
    }
    
    void showAll() const {
        if (files.empty()) {
            cout << "\nСписок пуст\n";
            return;
        }
        
        cout << "\nВСЕ ФАЙЛЫ (" << files.size() << ")" << endl;
        cout << "------------------------------------------------" << endl;
        
        for (const auto& file : files) {
            cout << left << setw(25) << file.getFilename()
                 << " | " << setw(12) << file.getDate()
                 << " | " << file.getSize() << " байт" << endl;
        }
        cout << "------------------------------------------------" << endl;
    }
    
    long long getTotalSize() const {
        long long total = 0;
        for (const auto& file : files) {
            total += file.getSize();
        }
        return total;
    }
    
    int countLargeFiles(int threshold) const {
        int count = 0;
        for (const auto& file : files) {
            if (file.getSize() > threshold) {
                count++;
            }
        }
        return count;
    }
};

int main() {
    FileManager manager;
    string command, filename, date;
    int size;
    
    cout << "УЧЕТ ФАЙЛОВ" << endl;
    cout << "=======================" << endl;
    cout << "Команды:" << endl;
    cout << "  add \"название_файла\" ГГГГ.ММ.ДД Размер - добавить файл" << endl;
    cout << "  list - показать все файлы" << endl;
    cout << "  total - общий размер всех файлов" << endl;
    cout << "  large - показать количество файлов > 1 МБ" << endl;
    cout << "  exit - выход" << endl;
    cout << "=======================" << endl;
    
    manager.showAll();
    
    while (true) {
        cout << "\n> ";
        cin >> command;
        
        if (command == "exit") {
            cout << "Программа завершена" << endl;
            break;
        }
        
        if (command == "list") {
            manager.showAll();
            continue;
        }
        
        if (command == "total") {
            cout << "Общий размер: " << manager.getTotalSize() << " байт" << endl;
            if (manager.getTotalSize() > 1024 * 1024) {
                cout << "(" << fixed << setprecision(2)
                     << manager.getTotalSize() / (1024.0 * 1024.0) << " МБ)" << endl;
            } else if (manager.getTotalSize() > 1024) {
                cout << "(" << fixed << setprecision(2)
                     << manager.getTotalSize() / 1024.0 << " КБ)" << endl;
            }
            continue;
        }
        
        if (command == "large") {
            int count = manager.countLargeFiles(1024 * 1024); // 1 МБ = 1048576 байт
            cout << "Файлов размером более 1 МБ: " << count << endl;
            continue;
        }
        
        if (command == "add") {
            cin >> ws;
            
            // Проверяем, что первым символом идет кавычка
            if (cin.peek() == '"') {
                cin.ignore(); // пропускаем открывающую кавычку
                getline(cin, filename, '"'); // читаем до закрывающей кавычки
                cin >> date >> size;
                
                if (size < 0) {
                    cout << "Ошибка: размер не может быть отрицательным" << endl;
                    continue;
                }
                
                manager.addFile(filename, date, size);
                manager.showAll();
            } else {
                cout << "Ошибка: имя файла должно быть в кавычках" << endl;
                cin.ignore(10000, '\n');
            }
        } else {
            cout << "Неизвестная команда" << endl;
            cin.ignore(10000, '\n');
        }
    }
    return 0;
}
