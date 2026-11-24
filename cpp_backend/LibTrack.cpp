#include <iostream>
#include <string>
using namespace std;

// =========================================================================
// 1. QUEUE DATA STRUCTURE (FIFO - First In, First Out)
// কেন ব্যবহার করা হয়েছে: কোনো বই যদি আগে থেকেই Borrowed থাকে, তবে পরের স্টুডেন্টদের 
// সিরিয়াল অনুযায়ী Waitlist-এ রাখার জন্য Custom Queue ব্যবহার করা হয়েছে।
// =========================================================================
struct QNode {
    string studentName;
    QNode* next;

    QNode(string name) {
        studentName = name;
        next = nullptr;
    }
};

class WaitlistQueue {
private:
    QNode *front, *rear;
    int count;

public:
    WaitlistQueue() {
        front = rear = nullptr;
        count = 0;
    }

    // নতুন স্টুডেন্টকে Waitlist-এর শেষে যোগ করা (Time Complexity: O(1))
    void enqueue(string name) {
        QNode* newNode = new QNode(name);
        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        count++;
    }

    // বই ফেরত এলে Waitlist-এর একদম প্রথম স্টুডেন্টকে বের করা (Time Complexity: O(1))
    string dequeue() {
        if (isEmpty()) {
            return "";
        }
        QNode* temp = front;
        string name = temp->studentName;
        front = front->next;

        if (front == nullptr) {
            rear = nullptr;
        }
        delete temp;
        count--;
        return name;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    int getCount() {
        return count;
    }

    // সম্পূর্ণ Waitlist সিরিয়াল অনুযায়ী দেখানো (Traverse - Time Complexity: O(k))
    void displayWaitlist() {
        if (isEmpty()) {
            cout << "None";
            return;
        }
        QNode* cur = front;
        int pos = 1;
        while (cur != nullptr) {
            cout << pos++ << ". " << cur->studentName;
            if (cur->next != nullptr) cout << " -> ";
            cur = cur->next;
        }
    }

    ~WaitlistQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }
};

// =========================================================================
// 2. BOOK CLASS
// প্রতিটি বইয়ের তথ্য (ID, Title, Author, Status, Borrower এবং Waitlist) সংরক্ষণ করে।
// =========================================================================
class Book {
public:
    int id;
    string title;
    string author;
    bool available;
    string borrower;
    WaitlistQueue waitlist;

    Book(int i = 0, string t = "", string a = "") {
        id = i;
        title = t;
        author = a;
        available = true;
        borrower = "";
    }
};

// =========================================================================
// 3. DOUBLY LINKED LIST (DLL)
// কেন ব্যবহার করা হয়েছে: সব বই সিরিয়াল অনুযায়ী মেমোরিতে সংরক্ষণ, সামনে-পিছনে Traverse 
// এবং যেকোনো বই সহজে Delete বা Update করার জন্য Doubly Linked List ব্যবহৃত হয়েছে।
// =========================================================================
struct DLLNode {
    Book book;
    DLLNode *next, *prev;

    DLLNode(int id, string title, string author) : book(id, title, author) {
        next = prev = nullptr;
    }
};

class BookDoublyList {
private:
    DLLNode *head, *tail;

public:
    BookDoublyList() {
        head = tail = nullptr;
    }

    // লিস্টের শেষে নতুন বই যুক্ত করা (Insert - Time Complexity: O(1))
    DLLNode* insertBook(int id, string title, string author) {
        DLLNode* newNode = new DLLNode(id, title, author);
        if (head == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        return newNode;
    }

    // নির্দিষ্ট নোড ডিলিট করা (Delete - Time Complexity: O(1) যখন নোড পয়েন্টার জানা থাকে)
    void deleteNode(DLLNode* target) {
        if (target == nullptr) return;

        if (target == head && target == tail) {
            head = tail = nullptr;
        } else if (target == head) {
            head = head->next;
            head->prev = nullptr;
        } else if (target == tail) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            target->prev->next = target->next;
            target->next->prev = target->prev;
        }
        delete target;
    }

    DLLNode* getHead() {
        return head;
    }

    bool isEmpty() {
        return head == nullptr;
    }

    ~BookDoublyList() {
        DLLNode* cur = head;
        while (cur != nullptr) {
            DLLNode* temp = cur;
            cur = cur->next;
            delete temp;
        }
    }
};

// =========================================================================
// 4. BINARY SEARCH TREE (BST)
// কেন ব্যবহার করা হয়েছে: বইয়ের ID দিয়ে O(log n) সময়ে দ্রুত বই খুঁজে বের করার 
// জন্য BST তৈরি করা হয়েছে। এর প্রতিটি নোড সরাসরি DLL-এর বইয়ের পয়েন্টার ধরে রাখে।
// =========================================================================
struct BSTNode {
    int id;
    DLLNode* bookPtr;
    BSTNode *left, *right;

    BSTNode(int i, DLLNode* ptr) {
        id = i;
        bookPtr = ptr;
        left = right = nullptr;
    }
};

class BookBST {
private:
    BSTNode* root;

    BSTNode* insertHelper(BSTNode* node, int id, DLLNode* ptr) {
        if (node == nullptr) {
            return new BSTNode(id, ptr);
        }
        if (id < node->id) {
            node->left = insertHelper(node->left, id, ptr);
        } else if (id > node->id) {
            node->right = insertHelper(node->right, id, ptr);
        }
        return node;
    }

    DLLNode* searchHelper(BSTNode* node, int id) {
        if (node == nullptr) return nullptr;
        if (id == node->id) return node->bookPtr;
        if (id < node->id) return searchHelper(node->left, id);
        return searchHelper(node->right, id);
    }

    BSTNode* findMin(BSTNode* node) {
        while (node && node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    BSTNode* deleteHelper(BSTNode* node, int id) {
        if (node == nullptr) return nullptr;

        if (id < node->id) {
            node->left = deleteHelper(node->left, id);
        } else if (id > node->id) {
            node->right = deleteHelper(node->right, id);
        } else {
            if (node->left == nullptr) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }
            BSTNode* temp = findMin(node->right);
            node->id = temp->id;
            node->bookPtr = temp->bookPtr;
            node->right = deleteHelper(node->right, temp->id);
        }
        return node;
    }

    void destroyTree(BSTNode* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    BookBST() {
        root = nullptr;
    }

    void insert(int id, DLLNode* ptr) {
        root = insertHelper(root, id, ptr);
    }

    DLLNode* search(int id) {
        return searchHelper(root, id);
    }

    void remove(int id) {
        root = deleteHelper(root, id);
    }

    ~BookBST() {
        destroyTree(root);
    }
};

// =========================================================================
// 5. MAIN LIBRARY CONTROLLER CLASS (LibTrack)
// Doubly Linked List, BST এবং Queue—এই তিনটি ডেটা স্ট্রাকচারকে একসাথে পরিচালনা করে।
// =========================================================================
class LibTrack {
private:
    BookDoublyList list;
    BookBST tree;
    string adminUser = "admin";
    string adminPass = "admin1234";

    bool verifyAdmin() {
        string u, p;
        cout << "\n--- Admin Verification Required ---\n";
        cout << "Username: ";
        getline(cin, u);
        cout << "Password: ";
        getline(cin, p);
        if (u == adminUser && p == adminPass) {
            return true;
        }
        cout << "[Error] Invalid Admin Credentials!\n";
        return false;
    }

    void seedBook(int id, string title, string author) {
        DLLNode* node = list.insertBook(id, title, author);
        tree.insert(id, node);
    }

public:
    LibTrack() {
        seedBook(1, "Introduction to Computing Systems", "Yale N. Patt");
        seedBook(2, "Let Us C", "Yashavant Kanetkar");
        seedBook(3, "C Programming Language", "Brian W. Kernighan");
        seedBook(4, "Object-Oriented Programming in C++", "Robert Lafore");
        seedBook(5, "Data Structures and Algorithms", "Mark Allen Weiss");
        seedBook(6, "Discrete Mathematics and Its Applications", "Kenneth H. Rosen");
        seedBook(7, "Database System Concepts", "Abraham Silberschatz");
        seedBook(8, "Operating System Concepts", "Abraham Silberschatz");
        seedBook(9, "Computer Networking: A Top-Down Approach", "James Kurose");
        seedBook(10, "Computer Organization and Design", "David A. Patterson");
        seedBook(11, "Software Engineering", "Ian Sommerville");
        seedBook(12, "Artificial Intelligence: A Modern Approach", "Stuart Russell");
        seedBook(13, "Introduction to Algorithms", "Thomas H. Cormen");
        seedBook(14, "Clean Code", "Robert C. Martin");
        seedBook(15, "Head First Java", "Kathy Sierra & Bert Bates");
    }

    void addBook(int id, string title, string author) {
        if (!verifyAdmin()) return;
        if (tree.search(id) != nullptr) {
            cout << "[Overflow/Duplicate] Book ID " << id << " already exists!\n";
            return;
        }
        DLLNode* node = list.insertBook(id, title, author);
        tree.insert(id, node);
        cout << "[Success] Book added to LibTrack!\n";
    }

    void displayAllBooks() {
        if (list.isEmpty()) {
            cout << "[Empty] Library has no books.\n";
            return;
        }
        DLLNode* cur = list.getHead();
        cout << "\n================ LIBTRACK CATALOG ================\n";
        while (cur != nullptr) {
            cout << "ID: " << cur->book.id << " | " << cur->book.title
                 << " (" << cur->book.author << ")\n";
            cout << "Status: " << (cur->book.available ? "Available" : "Borrowed by " + cur->book.borrower);
            if (!cur->book.waitlist.isEmpty()) {
                cout << " | Waitlist: ";
                cur->book.waitlist.displayWaitlist();
            }
            cout << "\n--------------------------------------------------\n";
            cur = cur->next;
        }
    }

    void searchBook(int id) {
        DLLNode* node = tree.search(id);
        if (node == nullptr) {
            cout << "[Not Found] No book found with ID " << id << ".\n";
            return;
        }
        cout << "\n--- Book Details ---\n";
        cout << "ID: " << node->book.id << "\nTitle: " << node->book.title
             << "\nAuthor: " << node->book.author
             << "\nStatus: " << (node->book.available ? "Available" : "Borrowed by " + node->book.borrower) << "\n";
    }

    void updateBook(int id, string newTitle, string newAuthor) {
        if (!verifyAdmin()) return;
        DLLNode* node = tree.search(id);
        if (node == nullptr) {
            cout << "[Not Found] Book ID does not exist.\n";
            return;
        }
        node->book.title = newTitle;
        node->book.author = newAuthor;
        cout << "[Success] Book info updated!\n";
    }

    void deleteBook(int id) {
        if (!verifyAdmin()) return;
        DLLNode* node = tree.search(id);
        if (node == nullptr) {
            cout << "[Not Found] Book ID does not exist.\n";
            return;
        }
        tree.remove(id);
        list.deleteNode(node);
        cout << "[Success] Book ID " << id << " removed from system.\n";
    }

    void borrowBook(int id, string student) {
        DLLNode* node = tree.search(id);
        if (node == nullptr) {
            cout << "[Not Found] Book ID " << id << " does not exist.\n";
            return;
        }
        if (node->book.available) {
            node->book.available = false;
            node->book.borrower = student;
            cout << "[Success] '" << node->book.title << "' issued to " << student << ".\n";
        } else {
            node->book.waitlist.enqueue(student);
            cout << "[Waitlisted] Book is currently borrowed by " << node->book.borrower
                 << ". " << student << " added to Queue (Position #" << node->book.waitlist.getCount() << ").\n";
        }
    }

    void returnBook(int id) {
        DLLNode* node = tree.search(id);
        if (node == nullptr) {
            cout << "[Not Found] Book ID " << id << " does not exist.\n";
            return;
        }
        if (node->book.available) {
            cout << "[Underflow/Invalid] Book is already available in the shelf!\n";
            return;
        }
        if (node->book.waitlist.isEmpty()) {
            node->book.available = true;
            node->book.borrower = "";
            cout << "[Returned] Book is now available on the shelf.\n";
        } else {
            string nextStudent = node->book.waitlist.dequeue();
            node->book.borrower = nextStudent;
            cout << "[Auto-Assigned] Book returned and automatically issued to next student in Queue: " << nextStudent << "!\n";
        }
    }
};

int main() {
    LibTrack app;
    int choice, id;
    string title, author, student;

    do {
        cout << "\n================ LIBTRACK SYSTEM ================\n";
        cout << "1. Display All Books (Traverse)\n";
        cout << "2. Search Book by ID (BST Search)\n";
        cout << "3. Borrow Book (Issue / Waitlist Queue)\n";
        cout << "4. Return Book (Release / Dequeue Next)\n";
        cout << "5. Add New Book (Admin - Insert)\n";
        cout << "6. Update Book Info (Admin - Update)\n";
        cout << "7. Delete Book (Admin - Delete)\n";
        cout << "8. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: app.displayAllBooks(); break;
            case 2: cout << "Enter Book ID: "; cin >> id; app.searchBook(id); break;
            case 3:
                cout << "Enter Book ID: "; cin >> id; cin.ignore();
                cout << "Enter Student Name: "; getline(cin, student);
                app.borrowBook(id, student);
                break;
            case 4: cout << "Enter Book ID: "; cin >> id; app.returnBook(id); break;
            case 5:
                cout << "Enter New Book ID: "; cin >> id; cin.ignore();
                cout << "Enter Title: "; getline(cin, title);
                cout << "Enter Author: "; getline(cin, author);
                app.addBook(id, title, author);
                break;
            case 6:
                cout << "Enter Book ID to Update: "; cin >> id; cin.ignore();
                cout << "Enter New Title: "; getline(cin, title);
                cout << "Enter New Author: "; getline(cin, author);
                app.updateBook(id, title, author);
                break;
            case 7: cout << "Enter Book ID to Delete: "; cin >> id; app.deleteBook(id); break;
            case 8: cout << "Exiting LibTrack...\n"; break;
            default: cout << "Invalid Choice! Try again.\n";
        }
    } while (choice != 8);

    return 0;
}
