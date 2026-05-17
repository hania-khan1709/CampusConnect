/*
 * CampusConnect — A helper to guide you through campus life
 *
 * Data structures implemented:
 * Adjacency List Graph    — campus map + Dijkstra shortest path
 * Min-Heap                — booking priority queue
 * Binary Search Tree      — event calendar ordered by date
 * Hash Tables             — student lookup by email and ID
 * Linked-List Stack       — undo/redo history
 * Circular Queue          — notification ring buffer
 * Doubly-Linked List      — bidirectional activity log
 */

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

// ----------------------------------------------------------------
// Utilities
// ----------------------------------------------------------------

int ParseIntFromString(const string& s)
{
    int result = 0;
    for (int i = 0; i < (int)s.length(); i++)
    {
        char c = s[i];
        if (c >= '0' && c <= '9')
            result = result * 10 + (c - '0');
    }
    return result;
}

void SplitString(const string& s, char d, string* out, int& cnt)
{
    cnt = 0;
    string cur = "";
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (s[i] == d) {
            if (!cur.empty()) { out[cnt++] = cur; cur = ""; }
        }
        else {
            cur += s[i];
        }
    }
    if (!cur.empty()) out[cnt++] = cur;
}

string CurrentTimestamp() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

// ----------------------------------------------------------------
// Data models
// ----------------------------------------------------------------

struct Location {
    int  id;
    string name;
    string type;
    int  capacity;

    Location() {
        id = 0;
        capacity = 0;
    }
    Location(int _id, const string& _name, const string& _type, int cap) {
        id = _id;
        name = _name;
        type = _type;
        capacity = cap;
    }
};

struct Student {
    int    id;
    string name;
    string email;
    string department;
    string subjects[10];
    int    subjectCount;
    string interests[10];
    int    interestCount;
    string freeTime;
    int    year;

    Student() {
        id = 0;
        subjectCount = 0;
        interestCount = 0;
        year = 0; // Fixed typo 'o' to '0'
    }
    Student(int _id, const string& _name, const string& _email,
        const string& dept, int _year)
    {
        id = _id;
        name = _name;
        email = _email;
        department = dept;
        subjectCount = 0;
        interestCount = 0;
        year = _year;
    }
};

struct RoomBooking {
    int    bookingId;
    int    studentId;
    int    locationId;
    string date;
    string time;
    int    durationHrs;
    int    priority;

    RoomBooking() {
        bookingId = 0;
        studentId = 0;
        locationId = 0;
        durationHrs = 1;
        priority = 3;
    }
};

struct Event {
    int eventId;
    string title;
    string description;
    string date;
    string time;
    int locationId;
    string organizer;

    Event() {
        eventId = 0;
        locationId = 0;
    }
};

struct LogEntry {
    string timestamp;
    string userId;
    string action;
    string detail;

    LogEntry() {}
    LogEntry(const string& uid, const string& act, const string& det)
    {
        timestamp = CurrentTimestamp(); // Fixed case sensitivity
        userId = uid;
        action = act;
        detail = det;
    }
};

struct UndoAction {
    string type;
    string description;

    UndoAction() {}
    UndoAction(const string& t, const string& desc)
    {
        type = t;
        description = desc;
    }
};

// ----------------------------------------------------------------
// Adjacency List Graph
// ----------------------------------------------------------------

struct GraphEdge {
    int  destIndex;
    int  distance;
    GraphEdge* next;

    GraphEdge(int dest, int dist)
    {
        destIndex = dest;
        distance = dist;
        next = nullptr;
    }
};

struct GraphNode {
    Location   loc;
    GraphEdge* edges;

    GraphNode() {
        edges = nullptr;
    }
    GraphNode(const Location& l) {
        loc = l;
        edges = nullptr;
    }
};

class CampusGraph {
private:
    GraphNode* nodes;
    int nodeCount;
    int nodeCapacity;
    int nextId;

    void grow(int newCap)
    {
        GraphNode* tmp = new GraphNode[newCap];
        for (int i = 0; i < nodeCount; i++) tmp[i] = nodes[i];
        delete[] nodes;
        nodes = tmp;
        nodeCapacity = newCap;
    }

public:
    CampusGraph() : nodeCount(0), nodeCapacity(12), nextId(1) {
        nodes = new GraphNode[nodeCapacity];
        // Fixed method calls to use corrected case sensitivity
        AddLocation(Location(0, "Main Gate", "entrance", 0));
        AddLocation(Location(0, "Library", "library", 200));
        AddLocation(Location(0, "Computer Lab A", "lab", 50));
        AddLocation(Location(0, "Cafeteria", "cafeteria", 150));
        AddLocation(Location(0, "CS Department", "dept", 100));
        AddLocation(Location(0, "Physics Dept", "dept", 80));
        AddLocation(Location(0, "Auditorium", "hall", 500));
        AddLocation(Location(0, "Sports Complex", "sports", 300));
        connect(0, 1, 120);  connect(0, 3, 80);
        connect(1, 2, 60);   connect(1, 4, 90);
        connect(2, 4, 40);   connect(3, 4, 110);
        connect(4, 5, 50);   connect(4, 6, 200);
        connect(6, 7, 150);  connect(3, 7, 180);
    }

    ~CampusGraph() {
        for (int i = 0; i < nodeCount; i++)
        {
            GraphEdge* e = nodes[i].edges;
            while (e) { GraphEdge* t = e; e = e->next; delete t; }
        }
        delete[] nodes;
    }

    int AddLocation(const Location& loc)
    {
        if (nodeCount >= nodeCapacity) grow(nodeCapacity * 2);
        Location nl = loc;
        if (nl.id == 0) nl.id = nextId++;
        nodes[nodeCount] = GraphNode(nl);
        return nodeCount++;
    }

    void connect(int a, int b, int dist)
    {
        if (a < 0 || a >= nodeCount || b < 0 || b >= nodeCount) return;
        GraphEdge* e1 = new GraphEdge(b, dist);
        e1->next = nodes[a].edges; nodes[a].edges = e1;
        GraphEdge* e2 = new GraphEdge(a, dist);
        e2->next = nodes[b].edges; nodes[b].edges = e2;
    }

    Location* GetByIndex(int idx) {
        return (idx >= 0 && idx < nodeCount) ? &nodes[idx].loc : nullptr;
    }
    int total() const {
        return nodeCount;
    }
    void ListLocations() const {
        for (int i = 0; i < nodeCount; i++)
        {
            cout << "  [" << i << "] " << nodes[i].loc.name
                << " (" << nodes[i].loc.type << ")";
            if (nodes[i].loc.capacity > 0)
                cout << " — cap: " << nodes[i].loc.capacity;
            cout << "\n";
        }
    }
    int Dijkstra(int src, int dst, int* path, int& pathLen) {
        const int INF = 1e9;
        int* dist = new int[nodeCount];
        int* prev = new int[nodeCount];
        bool* vis = new bool[nodeCount];

        for (int i = 0; i < nodeCount; i++) {
            dist[i] = INF; prev[i] = -1; vis[i] = false;
        }
        dist[src] = 0;

        for (int iter = 0; iter < nodeCount; iter++)
        {
            int u = -1;
            for (int i = 0; i < nodeCount; i++)
                if (!vis[i] && (u == -1 || dist[i] < dist[u])) u = i;
            if (u == -1 || dist[u] == INF) break;
            vis[u] = true;

            for (GraphEdge* e = nodes[u].edges; e; e = e->next) {
                int v = e->destIndex;
                if (!vis[v] && dist[u] + e->distance < dist[v]) {
                    dist[v] = dist[u] + e->distance;
                    prev[v] = u;
                }
            }
        }

        pathLen = 0;
        int totalDist = dist[dst];
        if (totalDist == INF) {
            delete[] dist; delete[] prev; delete[] vis;
            return -1;
        }
        int tmp[50]; int tmpLen = 0;
        for (int v = dst; v != -1; v = prev[v]) tmp[tmpLen++] = v;
        for (int i = tmpLen - 1; i >= 0; i--) path[pathLen++] = tmp[i];

        delete[] dist; delete[] prev; delete[] vis;
        return totalDist;
    }
};

// ----------------------------------------------------------------
// Min-Heap — booking priority queue
// ----------------------------------------------------------------

class BookingPriorityQueue {
private:
    RoomBooking* arr;
    int capacity;
    int size;

    void grow(int newCap) {
        RoomBooking* tmp = new RoomBooking[newCap];
        for (int i = 0; i < size; i++) tmp[i] = arr[i];
        delete[] arr; arr = tmp; capacity = newCap;
    }

    void HeapifyUp(int i)
    {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (arr[i].priority < arr[parent].priority)
            {
                RoomBooking t = arr[i]; arr[i] = arr[parent]; arr[parent] = t;
                i = parent;
            }
            else break;
        }
    }

    void HeapifyDown(int i)
    {
        int smallest = i;
        int l = 2 * i + 1, r = 2 * i + 2;
        if (l < size && arr[l].priority < arr[smallest].priority) smallest = l;
        if (r < size && arr[r].priority < arr[smallest].priority) smallest = r;
        if (smallest != i) {
            RoomBooking t = arr[i]; arr[i] = arr[smallest]; arr[smallest] = t;
            HeapifyDown(smallest);
        }
    }

public:
    BookingPriorityQueue() : capacity(10), size(0)
    {
        arr = new RoomBooking[capacity];
    }
    ~BookingPriorityQueue() { delete[] arr; }
    void push(const RoomBooking& b)
    {
        if (size >= capacity) grow(capacity * 2);
        arr[size] = b;
        size++;
        HeapifyUp(size - 1);
    }
    void pop()
    {
        if (size == 0) return;
        arr[0] = arr[size - 1];
        size--;
        if (size > 0) HeapifyDown(0);
    }
    RoomBooking& top() {
        return arr[0];
    }
    bool empty() const {
        return size == 0;
    }
    int  total() const {
        return size;
    }
    void clear() {
        size = 0;
    }
    void CopyAll(RoomBooking* out, int& cnt) const
    {
        cnt = 0;
        for (int i = 0; i < size; i++) out[cnt++] = arr[i];
    }
};

// ----------------------------------------------------------------
// Binary Search Tree — event calendar
// ----------------------------------------------------------------

struct BSTNode {
    Event data;
    BSTNode* left;
    BSTNode* right;

    BSTNode(const Event& e) {
        data = e;
        left = nullptr;
        right = nullptr;
    }
};

class EventBST {
private:
    BSTNode* root;
    int nextId;

    BSTNode* insert(BSTNode* node, const Event& e) {
        if (!node) return new BSTNode(e);
        if (e.date < node->data.date ||
            (e.date == node->data.date && e.time < node->data.time))
            node->left = insert(node->left, e);
        else
            node->right = insert(node->right, e);
        return node;
    }

    void inorder(BSTNode* node, Event* out, int& idx, int maxSize) const {
        if (!node || idx >= maxSize) return;
        inorder(node->left, out, idx, maxSize);
        if (idx < maxSize) out[idx++] = node->data;
        inorder(node->right, out, idx, maxSize);
    }

    void FreeTree(BSTNode* node) {
        if (!node) return;
        FreeTree(node->left);
        FreeTree(node->right);
        delete node;
    }

public:
    EventBST() : root(nullptr), nextId(1) {}
    ~EventBST() { FreeTree(root); }

    void add(Event e) {
        if (e.eventId == 0) e.eventId = nextId++;
        root = insert(root, e);
    }

    int GetAll(Event* out, int maxSize) const {
        int idx = 0;
        inorder(root, out, idx, maxSize);
        return idx;
    }

    bool isEmpty() const {
        return root == nullptr;
    }
    int  total() const {
        Event tmp[1000];
        int idx = 0;
        inorder(root, tmp, idx, 1000);
        return idx;
    }
};

// ----------------------------------------------------------------
// Hash Table — string key (email -> Student*)
// ----------------------------------------------------------------

struct StringHashNode {
    string key;
    Student* value;
    StringHashNode* next;

    StringHashNode(const string& k, Student* v)
    {
        key = k;
        value = v;
        next = nullptr;
    }
};

class StudentEmailIndex {
private:
    StringHashNode** table;
    int tableSize;
    int count;

    int hash(const string& key) const {
        unsigned long h = 5381;
        for (char c : key) h = ((h << 5) + h) + c;
        return (int)(h % tableSize);
    }

public:
    StudentEmailIndex(int size = 127) : tableSize(size), count(0)
    {
        table = new StringHashNode * [tableSize];
        for (int i = 0; i < tableSize; i++) table[i] = nullptr;
    }
    ~StudentEmailIndex() { clear(); delete[] table; }

    void put(const string& key, Student* val) {
        int idx = hash(key);
        for (StringHashNode* cur = table[idx]; cur; cur = cur->next)
        {
            if (cur->key == key) { cur->value = val; return; }
        }
        StringHashNode* node = new StringHashNode(key, val);
        node->next = table[idx];
        table[idx] = node;
        count++;
    }
    bool get(const string& key, Student*& out) const {
        int idx = hash(key);
        for (StringHashNode* cur = table[idx]; cur; cur = cur->next) {
            if (cur->key == key) { out = cur->value; return true; }
        }
        return false;
    }

    bool contains(const string& key) const {
        Student* t; return get(key, t);
    }

    void GetAllKeys(string* keys, int& cnt) const {
        cnt = 0;
        for (int i = 0; i < tableSize; i++)
            for (StringHashNode* cur = table[i]; cur; cur = cur->next)
                keys[cnt++] = cur->key;
    }
    void clear() {
        for (int i = 0; i < tableSize; i++) {
            StringHashNode* cur = table[i];
            while (cur) {
                StringHashNode* t = cur;
                cur = cur->next;
                // Student pointer is managed by the clear system in main or here
                delete t;
            }
            table[i] = nullptr;
        }
        count = 0;
    }
    int total() const {
        return count;
    }
};

// ----------------------------------------------------------------
// Hash Table — integer key (studentId -> Student*)
// ----------------------------------------------------------------

struct IntHashNode {
    int key;
    Student* value;
    IntHashNode* next;

    IntHashNode(int k, Student* v) {
        key = k;
        value = v;
        next = nullptr;
    }
};

class StudentIDIndex {
private:
    IntHashNode** table;
    int tableSize;
    int count;

    int hash(int key) const { return key % tableSize; }

public:
    StudentIDIndex(int size = 127) : tableSize(size), count(0) {
        table = new IntHashNode * [tableSize];
        for (int i = 0; i < tableSize; i++) table[i] = nullptr;
    }

    ~StudentIDIndex() { clear(); delete[] table; }

    void put(int key, Student* val) {
        int idx = hash(key);
        for (IntHashNode* cur = table[idx]; cur; cur = cur->next) {
            if (cur->key == key) { cur->value = val; return; }
        }
        IntHashNode* node = new IntHashNode(key, val);
        node->next = table[idx];
        table[idx] = node;
        count++;
    }

    bool get(int key, Student*& out) const {
        int idx = hash(key);
        for (IntHashNode* cur = table[idx]; cur; cur = cur->next) {
            if (cur->key == key) { out = cur->value; return true; }
        }
        return false;
    }

    bool contains(int key) const { Student* t; return get(key, t); }

    void clear() {
        for (int i = 0; i < tableSize; i++) {
            IntHashNode* cur = table[i];
            while (cur) { IntHashNode* t = cur; cur = cur->next; delete t; }
            table[i] = nullptr;
        }
        count = 0;
    }
    int total() const {
        return count;
    }
};

// ----------------------------------------------------------------
// Trie — for Autocomplete (Added class based on your main usage)
// ----------------------------------------------------------------

class SearchTrie {
    struct TrieNode {
        TrieNode* children[128];
        bool isEndOfWord;
        TrieNode() {
            for (int i = 0; i < 128; i++) children[i] = nullptr;
            isEndOfWord = false;
        }
    };
    TrieNode* root;
    void collect(TrieNode* curr, string prefix, string* results, int& cnt, int max) {
        if (!curr || cnt >= max) return;
        if (curr->isEndOfWord) results[cnt++] = prefix;
        for (int i = 0; i < 128; i++) {
            if (curr->children[i]) collect(curr->children[i], prefix + (char)i, results, cnt, max);
        }
    }
public:
    SearchTrie() { root = new TrieNode(); }
    void insert(string word) {
        TrieNode* curr = root;
        for (char c : word) {
            if (!curr->children[(int)c]) curr->children[(int)c] = new TrieNode();
            curr = curr->children[(int)c];
        }
        curr->isEndOfWord = true;
    }
    int autocomplete(string prefix, string* results, int max) {
        TrieNode* curr = root;
        int cnt = 0;
        for (char c : prefix) {
            if (!curr->children[(int)c]) return 0;
            curr = curr->children[(int)c];
        }
        collect(curr, prefix, results, cnt, max);
        return cnt;
    }
};

// ----------------------------------------------------------------
// Linked-List Stack — undo/redo
// ----------------------------------------------------------------

class ActionStack {
private:
    struct StackNode {
        UndoAction data;
        StackNode* next;
        StackNode(const UndoAction& a) {
            data = a;
            next = nullptr;
        }
    };
    StackNode* topNode;
    int size;

public:
    ActionStack() {
        topNode = nullptr;
        size = 0;
    }
    ~ActionStack() { clear(); }

    void push(const UndoAction& a) {
        StackNode* node = new StackNode(a);
        node->next = topNode;
        topNode = node;
        size++;
    }
    void pop() {
        if (!topNode) return;
        StackNode* t = topNode;
        topNode = topNode->next;
        delete t;
        size--;
    }
    UndoAction& top() {
        return topNode->data;
    }
    bool empty() const {
        return topNode == nullptr;
    }
    int  total() const {
        return size;
    }
    void clear() {
        while (!empty()) pop();
    }
};

// ----------------------------------------------------------------
// Circular Queue — notification ring buffer
// ----------------------------------------------------------------

struct Notification {
    string timestamp;
    string message;
};

class NotificationBuffer {
private:
    static const int CAPACITY = 20;
    Notification buffer[CAPACITY];
    int head;
    int tail;
    int count;

public:
    NotificationBuffer() {
        head = 0;
        tail = 0;
        count = 0;
    }

    void enqueue(const string& msg) {
        buffer[tail] = { CurrentTimestamp(), msg };
        tail = (tail + 1) % CAPACITY;
        if (count < CAPACITY) count++;
        else head = (head + 1) % CAPACITY;
    }

    bool dequeue(Notification& out)
    {
        if (count == 0) return false;
        out = buffer[head];
        head = (head + 1) % CAPACITY;
        count--;
        return true;
    }

    void PeekAll(Notification* out, int& cnt) const {
        cnt = 0;
        for (int i = 0; i < count; i++)
            out[cnt++] = buffer[(head + i) % CAPACITY];
    }

    int  total()   const {
        return count;
    }
    bool isEmpty() const {
        return count == 0;
    }
};

// ----------------------------------------------------------------
// Doubly-Linked List — activity log
// ----------------------------------------------------------------
struct ActivityNode {
    LogEntry data;
    ActivityNode* prev;
    ActivityNode* next;

    ActivityNode(const LogEntry& e) {
        data = e;
        prev = nullptr;
        next = nullptr;
    }
};

class StudentActivityLog {
private:
    ActivityNode* head;
    ActivityNode* tail;
    int count;

public:
    StudentActivityLog() {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    ~StudentActivityLog() {
        ActivityNode* cur = head;
        while (cur) {
            ActivityNode* t = cur; cur = cur->next; delete t;
        }
    }
    void append(const LogEntry& entry) {
        ActivityNode* node = new ActivityNode(entry);
        if (!tail) { head = tail = node; }
        else { tail->next = node; node->prev = tail; tail = node; }
        count++;
    }
    void printForward() const {
        for (ActivityNode* cur = head; cur; cur = cur->next)
            cout << "  " << cur->data.timestamp << "  "
            << cur->data.action << "  " << cur->data.detail << "\n";
    }
    void printBackward() const {
        for (ActivityNode* cur = tail; cur; cur = cur->prev)
            cout << "  " << cur->data.timestamp << "  "
            << cur->data.action << "  " << cur->data.detail << "\n";
    }

    int total() const { return count; }
};

// ----------------------------------------------------------------
// Global state
// ----------------------------------------------------------------

StudentEmailIndex emailIndex(127);
StudentIDIndex idIndex(127);
CampusGraph campusGraph;
BookingPriorityQueue bookingQueue;
EventBST eventCalendar;
ActionStack undoStack;
ActionStack redoStack;
NotificationBuffer notifBuffer;
StudentActivityLog activityLog;
SearchTrie locationTrie;
SearchTrie studentTrie;

LogEntry  systemLogs[2000];
int logCount = 0;
Student* activeUser = nullptr;
bool isAdminMode = false;
int nextBookingId = 1;

void LogAction(const string& action, const string& detail) {
    string uid = activeUser ? to_string(activeUser->id) : "system";
    LogEntry entry(uid, action, detail);
    if (logCount < 2000) systemLogs[logCount++] = entry;
    activityLog.append(entry);

    ofstream f("campus_log.txt", ios::app);
    if (f.is_open())
        f << entry.timestamp << "| User: " << uid
        << " | " << action << "|" << detail << "\n";
}
void notify(const string& msg) {
    notifBuffer.enqueue(msg);
}

// ----------------------------------------------------------------
// File persistence
// ----------------------------------------------------------------

void SaveStudents() {
    ofstream f("students.dat");
    if (!f.is_open()) return;
    string keys[1000]; int kc = 0;
    emailIndex.GetAllKeys(keys, kc);
    for (int i = 0; i < kc; i++) {
        Student* s; if (!emailIndex.get(keys[i], s)) continue;
        f << s->id << "," << s->name << "," << s->email << ","
            << s->department << "," << s->year << "," << s->freeTime << ","
            << s->subjectCount << ",";
        for (int j = 0; j < s->subjectCount; j++) {
            f << s->subjects[j]; if (j < s->subjectCount - 1) f << ";";
        }
        f << "," << s->interestCount << ",";
        for (int j = 0; j < s->interestCount; j++) {
            f << s->interests[j]; if (j < s->interestCount - 1) f << ";";
        }
        f << "\n";
    }
}

void LoadStudents() {
    ifstream f("students.dat");
    if (!f.is_open()) return;
    string line;
    while (getline(f, line)) {
        string parts[20]; int pc = 0;
        SplitString(line, ',', parts, pc);
        if (pc < 6) continue;
        Student* s = new Student();
        s->id = ParseIntFromString(parts[0]);
        s->name = parts[1];
        s->email = parts[2];
        s->department = parts[3];
        s->year = ParseIntFromString(parts[4]);
        s->freeTime = parts[5];
        if (pc > 7) {
            string subs[10]; int sc = 0;
            SplitString(parts[7], ';', subs, sc);
            for (int j = 0; j < sc; j++) s->subjects[s->subjectCount++] = subs[j];
        }
        if (pc > 9) {
            string ints[10]; int ic = 0;
            SplitString(parts[9], ';', ints, ic);
            for (int j = 0; j < ic; j++) s->interests[s->interestCount++] = ints[j];
        }
        emailIndex.put(s->email, s);
        idIndex.put(s->id, s);
        studentTrie.insert(s->name);
    }
}

void SaveBookings() {
    ofstream f("bookings.dat");
    if (!f.is_open()) return;
    f << nextBookingId << "\n";
    RoomBooking all[1000]; int cnt = 0;
    bookingQueue.CopyAll(all, cnt);
    for (int i = 0; i < cnt; i++)
        f << all[i].bookingId << "," << all[i].studentId << ","
        << all[i].locationId << "," << all[i].date << ","
        << all[i].time << "," << all[i].durationHrs << ","
        << all[i].priority << "\n";
}

void LoadBookings() {
    ifstream f("bookings.dat");
    if (!f.is_open()) return;
    string line;
    getline(f, line); nextBookingId = ParseIntFromString(line);
    while (getline(f, line)) {
        string p[10]; int pc = 0;
        SplitString(line, ',', p, pc);
        if (pc < 7) continue;
        RoomBooking b;
        b.bookingId = ParseIntFromString(p[0]);
        b.studentId = ParseIntFromString(p[1]);
        b.locationId = ParseIntFromString(p[2]);
        b.date = p[3]; b.time = p[4];
        b.durationHrs = ParseIntFromString(p[5]);
        b.priority = ParseIntFromString(p[6]);
        bookingQueue.push(b);
    }
}

void SaveEvents() {
    ofstream f("events.dat");
    if (!f.is_open()) return;
    Event all[1000]; int cnt = eventCalendar.GetAll(all, 1000);
    for (int i = 0; i < cnt; i++)
        f << all[i].eventId << "," << all[i].title << ","
        << all[i].description << "," << all[i].date << ","
        << all[i].time << "," << all[i].locationId << ","
        << all[i].organizer << "\n";
}

void LoadEvents() {
    ifstream f("events.dat");
    if (!f.is_open()) return;
    string line;
    while (getline(f, line)) {
        string p[10]; int pc = 0;
        SplitString(line, ',', p, pc);
        if (pc < 7) continue;
        Event e;
        e.eventId = ParseIntFromString(p[0]);
        e.title = p[1]; e.description = p[2];
        e.date = p[3]; e.time = p[4];
        e.locationId = ParseIntFromString(p[5]);
        e.organizer = p[6];
        eventCalendar.add(e);
    }
}

void LoadAll() {
    LoadStudents();
    LoadBookings();
    LoadEvents();
    LogAction("STARTUP", "Data loaded");
}
void SaveAll() {
    SaveStudents();
    SaveBookings();
    SaveEvents();
    LogAction("SHUTDOWN", "Data saved");

    // Memory Clean up - critical fix
    string keys[1000]; int kc = 0;
    emailIndex.GetAllKeys(keys, kc);
    for (int i = 0; i < kc; i++) {
        Student* s; if (emailIndex.get(keys[i], s)) delete s;
    }
}

// ----------------------------------------------------------------
// UI helpers
// ----------------------------------------------------------------

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void PressEnterToContinue() {
    cout << "\n Press ENTER to continue..";
    cin.ignore(); cin.get();
}

void PrintBanner(const string& title) {
    int width = 56;
    string line(width, '=');
    string pad(width - 2, ' ');
    int padLen = (width - 2 - (int)title.length()) / 2;
    string left(padLen, ' ');
    string right(width - 2 - padLen - (int)title.length(), ' ');
    cout << "\n  +" << line << "+\n";
    cout << "  |" << pad << "|\n";
    cout << "  |" << left << title << right << "|\n";
    cout << "  |" << pad << "|\n";
    cout << "  +" << line << "+\n\n";
}

void PrintSectionHeader(const string& title) {
    cout << "\n-- " << title << " ";
    int dashes = 50 - (int)title.length();
    for (int i = 0; i < dashes; i++) cout << '-';
    cout << "\n";
}

void PrintRow(const string& label, const string& value) {
    cout << "|" << label;
    int pad = 22 - (int)label.length();
    for (int i = 0; i < pad; i++) cout << ' ';
    cout << ":" << value << "\n";
}

void ShowNotifications() {
    if (!notifBuffer.isEmpty())
        cout << "\n  [!] You have " << notifBuffer.total() << " notification(s)\n";
}

// ----------------------------------------------------------------
// Features
// ----------------------------------------------------------------

void PrintMainMenu() {
    clearScreen();
    PrintBanner("CampusConnect v2.0");

    cout << "  ACCOUNT\n";
    cout << "  [1] Student Login        [3] Register New Student\n";
    cout << "  [2] Admin Login\n\n";

    cout << "  CAMPUS\n";
    cout << "  [4] Campus Map & Navigation\n";
    cout << "  [5] Location Search (Autocomplete)\n\n";

    cout << "  BOOKINGS\n";
    cout << "  [6] Book a Room          [8] My Bookings\n";
    cout << "  [7] Undo Last Booking\n\n";

    cout << "  COMMUNITY\n";
    cout << "  [9] Find Study Partners\n";
    cout << " [10] Campus Events\n\n";

    cout << "  SYSTEM\n";
    cout << " [11] Dashboard / Stats\n";
    cout << " [12] Notifications\n";
    cout << " [13] My Activity Log\n";
    cout << " [14] Exit\n\n";

    if (activeUser)
        cout << "Logged in as : " << activeUser->name
        << " (" << activeUser->department << ", Year " << activeUser->year << ")\n";
    else if (isAdminMode)
        cout << "Logged in as: Administrator\n";
    else
        cout << "Not logged in.\n";

    ShowNotifications();
    cout << "\nChoice: ";
}

void StudentLogin() {
    clearScreen();
    PrintBanner("Student Login");
    string email;
    cout << "  Email: "; cin >> email;

    Student* s;
    if (emailIndex.get(email, s)) {
        activeUser = s;
        isAdminMode = false;
        notify("Welcome back, " + s->name + "!");
        LogAction("LOGIN", "Student: " + email);
        cout << "\n Welcome," << s->name << "!\n";
    }
    else {
        cout << "\n[!] No account found. Use option [3] to register.\n";
    }
    PressEnterToContinue();
}

void AdminLogin() {
    clearScreen();
    PrintBanner("Admin Login");
    string username, password;
    cout << "  Username: "; cin >> username;
    cout << "  Password: "; cin >> password;

    if (username == "admin" && password == "admin123") {
        isAdminMode = true;
        activeUser = nullptr;
        LogAction("ADMIN_LOGIN", "Administrator authenticated");
        cout << "\nAdmin access granted.\n";
    }
    else {
        cout << "\n [!] Invalid credentials.\n";
    }
    PressEnterToContinue();
}

void RegisterStudent() {
    clearScreen();
    PrintBanner("Register New Student");
    Student* s = new Student();

    cout << "  Student ID   : "; string sid; cin >> sid;
    s->id = ParseIntFromString(sid);

    if (idIndex.contains(s->id)) {
        cout << "\n  [!] That ID is already registered.\n";
        delete s; PressEnterToContinue(); return;
    }
    cin.ignore();
    cout << " Full Name: "; getline(cin, s->name);
    cout << " Email: "; getline(cin, s->email);
    cout << " Department: "; getline(cin, s->department);
    cout << " Year (1-4): "; string yr; getline(cin, yr);
    s->year = ParseIntFromString(yr);
    cout << "  Free Time: "; getline(cin, s->freeTime);

    s->subjects[s->subjectCount++] = "Data Structures";
    s->subjects[s->subjectCount++] = "Algorithms";
    s->subjects[s->subjectCount++] = "Database Systems";
    s->interests[s->interestCount++] = "Programming";
    s->interests[s->interestCount++] = "Study Groups";

    emailIndex.put(s->email, s);
    idIndex.put(s->id, s);
    studentTrie.insert(s->name);

    notify("New student registered: " + s->name);
    LogAction("REGISTER", "New student: " + s->email);
    cout << "\n  Registration successful!\n";
    PressEnterToContinue();
}

void CampusMapAndNavigation() {
    clearScreen();
    PrintBanner("Campus Map & Navigation");
    PrintSectionHeader("Locations");
    campusGraph.ListLocations();

    cout << "\n -- Find Shortest Path ------------------------------------\n";
    cout << "From index: "; string frStr; cin >> frStr;
    cout << " To index  : "; string toStr; cin >> toStr;

    int path[50]; int pathLen = 0;
    int dist = campusGraph.Dijkstra(ParseIntFromString(frStr),
        ParseIntFromString(toStr), path, pathLen);
    if (dist < 0) {
        cout << "\n[!] No path found between those locations.\n";
    }
    else {
        cout << "\n  Shortest route (" << dist << " metres):\n  ";
        for (int i = 0; i < pathLen; i++) {
            Location* loc = campusGraph.GetByIndex(path[i]);
            if (loc) cout << loc->name;
            if (i < pathLen - 1) cout << " -> ";
        }
        cout << "\n";
    }

    LogAction("NAVIGATE", frStr + " to " + toStr);
    PressEnterToContinue();
}

void SearchAutocomplete() {
    clearScreen();
    PrintBanner("Location Search (Autocomplete)");
    cout << "Prefix: "; string prefix; cin >> prefix;

    string results[20]; int cnt = locationTrie.autocomplete(prefix, results, 20);
    if (cnt == 0) {
        cout << "\nNo matches for \"" << prefix << "\".\n";
    }
    else {
        cout << "\n Matches (" << cnt << "):\n";
        for (int i = 0; i < cnt; i++)
            cout << "[" << (i + 1) << "] " << results[i] << "\n";
    }

    cout << "\n Search student names? (y/n): "; char ch; cin >> ch;
    if (ch == 'y' || ch == 'Y') {
        cout << "  Prefix: "; cin >> prefix;
        cnt = studentTrie.autocomplete(prefix, results, 20);
        if (cnt == 0) cout << "No student names matched.\n";
        else for (int i = 0; i < cnt; i++)
            cout << "[" << (i + 1) << "] " << results[i] << "\n";
    }

    LogAction("SEARCH", "Prefix:" + prefix);
    PressEnterToContinue();
}

void BookRoom() {
    if (!activeUser && !isAdminMode) {
        cout << "\n[!] Please log in first.\n";
        PressEnterToContinue(); return;
    }
    clearScreen();
    PrintBanner("Book a Room");
    campusGraph.ListLocations();

    RoomBooking b;
    b.bookingId = nextBookingId++;
    b.studentId = activeUser ? activeUser->id : 0;

    cout << "\n Location index : "; string ls; cin >> ls; b.locationId = ParseIntFromString(ls);
    cout << "  Date (YYYY-MM-DD) : "; cin >> b.date;
    cout << "  Start time (HH:MM): "; cin >> b.time;
    cout << "  Duration (hours)  : "; string hs; cin >> hs; b.durationHrs = ParseIntFromString(hs);
    cout << "  Priority (1=high) : "; string ps; cin >> ps; b.priority = ParseIntFromString(ps);

    bookingQueue.push(b);

    UndoAction ua("BOOK", "Booking #" + to_string(b.bookingId) + " at loc " + ls);
    undoStack.push(ua);
    redoStack.clear();

    notify("Room booking confirmed — ID #" + to_string(b.bookingId));
    LogAction("BOOK", "Booking #" + to_string(b.bookingId));
    cout << "\n  Confirmed! ID: #" << b.bookingId
        << "|  Queue size: " << bookingQueue.total() << "\n";
    PressEnterToContinue();
}

void UndoLastBooking() {
    if (undoStack.empty()) {
        cout << "\n  [!] Nothing to undo.\n";
        PressEnterToContinue(); return;
    }
    UndoAction last = undoStack.top();
    undoStack.pop();
    redoStack.push(last);
    LogAction("UNDO", last.description);
    notify("Undo: " + last.description);
    cout << "\n  Undone: " << last.description << "\n";
    PressEnterToContinue();
}

void ViewBookings() {
    if (!activeUser && !isAdminMode) {
        cout << "\n  [!] Please log in first.\n"; PressEnterToContinue(); return;
    }
    clearScreen();
    PrintBanner("My Bookings");
    RoomBooking all[1000]; int cnt = 0;
    bookingQueue.CopyAll(all, cnt);
    bool found = false;
    for (int i = 0; i < cnt; i++) {
        if (!isAdminMode && all[i].studentId != activeUser->id) continue;
        found = true;
        cout << "  +------------------------------------------+\n";
        PrintRow("Booking ID", "#" + to_string(all[i].bookingId));
        PrintRow("Location", "Index " + to_string(all[i].locationId));
        PrintRow("Date", all[i].date);
        PrintRow("Time", all[i].time);
        PrintRow("Duration", to_string(all[i].durationHrs) + " hr(s)");
        PrintRow("Priority", to_string(all[i].priority) + " / 5");
        PrintRow("Status", "Pending");
    }
    if (!found) cout << "  No bookings found.\n";
    cout << "  +------------------------------------------+\n";
    PressEnterToContinue();
}

int ComputeMatchScore(const Student& a, const Student& b) {
    int score = 0;
    if (a.department == b.department) score += 30;
    if (a.year == b.year)       score += 20;
    if (a.freeTime == b.freeTime)   score += 25;
    for (int j = 0; j < a.subjectCount; j++)
        for (int k = 0; k < b.subjectCount; k++)
            if (a.subjects[j] == b.subjects[k]) {
                score += 10;
                break;
            }
    for (int j = 0; j < a.interestCount; j++)
        for (int k = 0; k < b.interestCount; k++)
            if (a.interests[j] == b.interests[k]) {
                score += 5;
                break;
            }
    return score;
}

void FindStudyPartners() {
    if (!activeUser) {
        cout << "\n  [!] Please log in as a student first.\n";
        PressEnterToContinue(); return;
    }
    clearScreen();
    PrintBanner("Find Study Partners");
    cout << "  Finding matches for: " << activeUser->name << "\n\n";

    string keys[1000]; int kc = 0;
    emailIndex.GetAllKeys(keys, kc);

    struct MatchResult { int score; Student* student; };
    MatchResult matches[1000]; int mc = 0;

    for (int i = 0; i < kc; i++) {
        Student* s;
        if (!emailIndex.get(keys[i], s) || s->id == activeUser->id) continue;
        int sc = ComputeMatchScore(*activeUser, *s);
        if (sc > 0) { matches[mc].score = sc; matches[mc].student = s; mc++; }
    }

    for (int i = 1; i < mc; i++) {
        MatchResult key = matches[i]; int j = i - 1;
        while (j >= 0 && matches[j].score < key.score) { matches[j + 1] = matches[j]; j--; }
        matches[j + 1] = key;
    }
    int show = mc < 5 ? mc : 5;
    if (show == 0) {
        cout << "No compatible partners found yet.\n";
    }
    else {
        cout << "Top " << show << " Matches:\n\n";
        for (int i = 0; i < show; i++) {
            Student* p = matches[i].student;
            cout << "  [" << (i + 1) << "] " << p->name << "\n";
            PrintRow("Compatibility", to_string(matches[i].score) + "%");
            PrintRow("Department", p->department);
            PrintRow("Year", to_string(p->year));
            PrintRow("Free Time", p->freeTime);
            PrintRow("Email", p->email);
            cout << "\n";
        }
    }
    LogAction("MATCH_SEARCH", "By student #" + to_string(activeUser->id));
    PressEnterToContinue();
}

void ViewAndManageEvents() {
    clearScreen();
    PrintBanner("Campus Events");
    Event all[1000]; int cnt = eventCalendar.GetAll(all, 1000);

    if (cnt == 0) {
        cout << "No upcoming events scheduled.\n";
    }
    else {
        cout << "  " << cnt << " event(s) — sorted chronologically:\n\n";
        for (int i = 0; i < cnt; i++) {
            cout << "  [" << all[i].eventId << "] " << all[i].title << "\n";
            PrintRow("Date", all[i].date);
            PrintRow("Time", all[i].time);
            PrintRow("Organizer", all[i].organizer);
            PrintRow("Location", "Index " + to_string(all[i].locationId));
            if (!all[i].description.empty()) PrintRow("Info", all[i].description);
            cout << "\n";
        }
    }
    if (isAdminMode) {
        cout << "  [A] Add new event   [B] Back\n  Choice: ";
        char c; cin >> c;
        if (c == 'A' || c == 'a') {
            Event e; cin.ignore();
            cout << "  Title: "; getline(cin, e.title);
            cout << "  Description: "; getline(cin, e.description);
            cout << "  Date: "; getline(cin, e.date);
            cout << "  Time: "; getline(cin, e.time);
            cout << "  Loc index: "; string ls; getline(cin, ls);
            e.locationId = ParseIntFromString(ls);
            cout << "  Organizer  : "; getline(cin, e.organizer);
            eventCalendar.add(e);
            notify("New event added: " + e.title);
            LogAction("ADD_EVENT", e.title);
            cout << "\n  Event added.\n";
        }
    }
    PressEnterToContinue();
}

void ShowDashboard() {
    clearScreen();
    PrintBanner("System Dashboard");

    PrintSectionHeader("Students");
    cout << "  Registered: " << emailIndex.total() << "\n";
    string depts[50]; int deptCounts[50]; int dc = 0;
    string keys[1000]; int kc = 0;
    emailIndex.GetAllKeys(keys, kc);
    for (int i = 0; i < kc; i++) {
        Student* s; if (!emailIndex.get(keys[i], s)) continue;
        bool found = false;
        for (int j = 0; j < dc; j++) {
            if (depts[j] == s->department) { deptCounts[j]++; found = true; break; }
        }
        if (!found && dc < 50) { depts[dc] = s->department; deptCounts[dc++] = 1; }
    }
    for (int i = 0; i < dc; i++)
        cout << "  " << depts[i] << ": " << deptCounts[i] << "\n";

    PrintSectionHeader("Bookings");
    cout << "  Total in queue : " << bookingQueue.total() << "\n";
    cout << "  Undo stack: " << undoStack.total() << " action(s)\n";
    cout << "  Redo stack : " << redoStack.total() << " action(s)\n";
    if (!bookingQueue.empty())
        cout << "Next to process: Priority " << bookingQueue.top().priority
        << " — Booking #" << bookingQueue.top().bookingId << "\n";

    PrintSectionHeader("Events");
    cout << " Scheduled: " << eventCalendar.total() << "\n";

    PrintSectionHeader("Campus");
    cout << " Locations: " << campusGraph.total() << "\n";
    cout << " Notif queue: " << notifBuffer.total() << " pending\n";
    cout << " System logs: " << logCount << " entries\n";

    LogAction("DASHBOARD", "Viewed");
    PressEnterToContinue();
}

void ViewNotifications() {
    clearScreen();
    PrintBanner("Notifications");
    Notification all[20]; int cnt = 0;
    notifBuffer.PeekAll(all, cnt);
    if (cnt == 0) {
        cout << "No notifications.\n";
    }
    else {
        for (int i = cnt - 1; i >= 0; i--)
            cout << "[" << all[i].timestamp << "] " << all[i].message << "\n";
    }
    PressEnterToContinue();
}

void ViewActivityLog() {
    clearScreen();
    PrintBanner("My Activity Log");
    cout << "  Recent actions (newest first):\n\n";
    activityLog.printBackward();
    PressEnterToContinue();
}

// ----------------------------------------------------------------
// Entry point
// ----------------------------------------------------------------

int main() {
    cout << "\n  CampusConnect — Loading data...\n";
    LoadAll();

    locationTrie.insert("Main Gate");
    locationTrie.insert("Library");
    locationTrie.insert("Computer Lab A");
    locationTrie.insert("Cafeteria");
    locationTrie.insert("CS Department");
    locationTrie.insert("Physics Department");
    locationTrie.insert("Auditorium");
    locationTrie.insert("Sports Complex");

    notify("CampusConnect started. Welcome!");

    string input; int choice;
    do {
        PrintMainMenu();
        cin >> input;
        choice = ParseIntFromString(input);

        switch (choice) {
        case  1: StudentLogin();
            break;
        case  2: AdminLogin();
            break;
        case  3: RegisterStudent();
            break;
        case  4: CampusMapAndNavigation();
            break;
        case  5: SearchAutocomplete();
            break;
        case  6: BookRoom();
            break;
        case  7: UndoLastBooking();
            break;
        case  8: ViewBookings();
            break;
        case  9: FindStudyPartners();
            break;
        case 10: ViewAndManageEvents();
            break;
        case 11: ShowDashboard();
            break;
        case 12: ViewNotifications();
            break;
        case 13: ViewActivityLog();
            break;
        case 14:
            cout << "\nSaving data...\n";
            SaveAll();
            cout << "Goodbye!\n\n";
            break;
        default:
            cout << "\n[!] Invalid option. Please choose 1-14.\n";
            PressEnterToContinue();
        }
    } while (choice != 14);

    return 0;
}