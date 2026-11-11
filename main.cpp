#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include<windows.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;
struct Task
{
    int id;
    string description;
    string status;
    string createdAt;
    string updatedAt;
};

string getCurrentTimestamp()
{
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);

    tm utcTime{};

    if (gmtime_s(&utcTime, &currentTime) != 0)
    {
        return "";
    }

    stringstream ss;
    ss << put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");

    return ss.str();
}

bool parseTaskID(const string &input, int &targetID)
{
    try
    {
        size_t pos = 0;
        targetID = stoi(input, &pos);

        return pos == input.size() && targetID > 0;
    }
    catch (const exception &)
    {
        return false;
    }
}

bool saveTasks(const vector<Task> &tasks)
{
    json data = json::array();

    for (const Task &task : tasks)
    {
        json item;

        item["id"] = task.id;
        item["description"] = task.description;
        item["status"] = task.status;
        item["createdAt"] = task.createdAt;
        item["updatedAt"] = task.updatedAt;

        data.push_back(item);
    }
const string tempPath = "tasks.json.tmp";
const string targetPath = "tasks.json";

{
    ofstream file(tempPath, ios::binary | ios::trunc);

    if (!file.is_open())
    {
        cerr << "Error opening temporary file" << endl;
        return false;
    }

    file << data.dump(4);
    file.close();

    if (!file)
    {
        cerr << "Error writing temporary file" << endl;
        return false;
    }
}

DWORD attributes = GetFileAttributesA(targetPath.c_str());
if (attributes == INVALID_FILE_ATTRIBUTES)
{
    DWORD error = GetLastError();

    if (error != ERROR_FILE_NOT_FOUND &&
        error != ERROR_PATH_NOT_FOUND)
    {
        cerr << "Error accessing tasks.json: " << error << endl;
        return false;
    }
}

if (attributes == INVALID_FILE_ATTRIBUTES)
{
    if (!MoveFileExA(
            tempPath.c_str(),
            targetPath.c_str(),
            MOVEFILE_WRITE_THROUGH))
    {
        cerr << "Error creating tasks.json: " << GetLastError() << endl;
        return false;
    }
}
else
{
    if (!ReplaceFileA(
            targetPath.c_str(),
            tempPath.c_str(),
            nullptr,
            REPLACEFILE_IGNORE_MERGE_ERRORS,
            nullptr,
            nullptr))
    {
        cerr << "Error replacing tasks.json: " << GetLastError() << endl;
        return false;
    }
}

return true;
}

bool loadTasks(vector<Task> &tasks)
{
    DWORD attributes = GetFileAttributesA("tasks.json");

    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        DWORD error = GetLastError();

        if (error == ERROR_FILE_NOT_FOUND ||
            error == ERROR_PATH_NOT_FOUND)
        {
            return true;
        }

        cerr << "Error accessing tasks.json: " << error << endl;
        return false;
    }

    ifstream file("tasks.json");

    if (!file.is_open())
    {
        cerr << "Error opening tasks.json" << endl;
        return false;
    }

    try
    {
        json data;
        file >> data;

        if (!data.is_array())
        {
            cerr << "Error: Expected a JSON array." << endl;
            return false;
        }

        vector<Task> loaded;

        for (const auto &item : data)
        {
            Task task;

            task.id = item.at("id").get<int>();
            task.description = item.at("description").get<string>();
            task.status = item.at("status").get<string>();
            task.createdAt = item.value("createdAt", string(""));
            task.updatedAt = item.value("updatedAt", string(""));

            loaded.push_back(task);
        }

        tasks = loaded;
        return true;
    }
    catch (const json::exception &e)
    {
        cerr << "JSON loading error: " << e.what() << endl;
        return false;
    }
}

int main(int argc, char *argv[])
{
    vector<Task> tasks;
    if (!loadTasks(tasks))
    {
        return 1;
    }

    if (argc < 2)
    {
        cout << "Please provide a command" << endl;
        return 1;
    }
    string command = argv[1];
    if (command == "add")
    {
        if (argc < 3)
{
    cerr << "Please provide a task description." << endl;
    return 1;
}

if (string(argv[2]).empty())
{
    cerr << "Task description cannot be empty." << endl;
    return 1;
}
        int newID = 1;

        for (const Task &task : tasks)
        {
            if (task.id >= newID)
            {
                newID = task.id + 1;
            }
        }

        Task task1;
        task1.id = newID;
        task1.description = argv[2];
        task1.status = "todo";
        string timestamp = getCurrentTimestamp();

        task1.createdAt = timestamp;
        task1.updatedAt = timestamp;

        cout << task1.id << "\n"
             << task1.description << "\n"
             << task1.status << endl;
        tasks.push_back(task1);

        if (!saveTasks(tasks))
        {
            cout << "Failed to save task." << endl;
            return 1;
        }

        cout << "Total tasks: " << tasks.size() << endl;
    }
    else if (command == "list")
    {
        string filter = "";

        if (argc >= 3)
        {
            filter = argv[2];
        }
        if (filter != "" &&
            filter != "todo" &&
            filter != "in-progress" &&
            filter != "done")
        {
            cout << "Invalid status filter" << endl;
            return 1;
        }
        for (int i = 0; i < tasks.size(); i++)
        {
            if (filter == "" || filter == tasks[i].status)
            {
                cout << tasks[i].description << endl;
            }
        }

        // Your existing for loop remains here.
    }

    else if (command == "update")
    {
        if (argc < 4)
        {
            cerr << "Please provide a task ID and new description" << endl;
            return 1;
        }if (string(argv[3]).empty())
{
    cerr << "Task description cannot be empty." << endl;
    return 1;
}

        int targetID;

        if (!parseTaskID(argv[2], targetID))
        {
            cout << "Invalid task ID." << endl;
            return 1;
        }

        bool found = false;

        for (Task &task : tasks)
        {

            if (task.id == targetID)
            {
                task.description = argv[3];
                task.updatedAt = getCurrentTimestamp();

                if (!saveTasks(tasks))
                {
                    cout << "Failed to update task." << endl;
                    return 1;
                }

                cout << "Task updated successfully! " << endl;

                found = true;
                break;
            }
        }

       if (!found)
{
    cerr << "Task not found" << endl;
    return 1;
}
    }
    else if (command == "mark-done" || command == "mark-in-progress")
    {
        if (argc < 3)
        {
            cout << "Please provide a task ID";
            return 1;
        }
        int targetID;

        if (!parseTaskID(argv[2], targetID))
        {
            cout << "Invalid task ID." << endl;
            return 1;
        }
        string newStatus;
        if (command == "mark-done")
        {
            newStatus = "done";
        }
        else
        {
            newStatus = "in-progress";
        }
        bool found = false;
        for (Task &task : tasks)
        {
            if (task.id == targetID)
            {
                task.status = newStatus;
                task.updatedAt = getCurrentTimestamp();
                if (!saveTasks(tasks))
                {
                    cout << "Failed to update task." << endl;
                    return 1;
                }

                cout << "Task updated successfully! " << endl;
                found = true;
                break;
            }
        }
       if (!found)
{
    cerr << "Task not found" << endl;
    return 1;
}// Your implementation
    }

    else if (command == "delete")
    {
        if (argc < 3)
        {
            cout << "Please provide a task ID" << endl;
            return 1;
        }

        int targetID;

        if (!parseTaskID(argv[2], targetID))
        {
            cout << "Invalid task ID." << endl;
            return 1;
        }

        bool found = false;

        for (size_t i = 0; i < tasks.size(); i++)
        {
          if (tasks[i].id == targetID)
{
    cout << "Deleting: " << tasks[i].description << endl;

    tasks.erase(tasks.begin() + i);

    if (!saveTasks(tasks))
    {
        cout << "Failed to delete task." << endl;
        return 1;
    }

    cout << "Task deleted successfully!" << endl;

    found = true;
    break;
}
        }

      if (!found)
{
    cerr << "Task not found" << endl;
    return 1;
}
    }
   else
{
    cerr << "Unknown command" << endl;
    return 1;
}

    return 0;
}
