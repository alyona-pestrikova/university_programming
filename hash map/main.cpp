#include <iostream>
#include <iomanip>
#include <string>
#include <functional>

using namespace std;

class Matrix {
private:
    int** mx;
    int size;
public:
    Matrix() {
        size = 0;
        mx = new int* [size];
    }
    Matrix(int sz) {
        size = sz;
        mx = new int* [size];
        for (int i = 0; i < size; i++) {
            mx[i] = new int[size];
        }
    }
    Matrix(int sz, int elem) {
        size = sz;
        mx = new int* [size];
        for (int i = 0; i < size; i++) {
            mx[i] = new int[size];
        }
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i == j) {
                    mx[i][j] = elem;
                }
                else {
                    mx[i][j] = 0;
                }
            }
        }
    }
    Matrix(Matrix const& mx_to_copy) {
        size = mx_to_copy.size;
        mx = new int* [size];
        for (int i = 0; i < size; i++) {
            mx[i] = new int[size];
            for (int j = 0; j < size; j++) {
                mx[i][j] = mx_to_copy.mx[i][j];
            }
        }
    }
    Matrix& operator = (Matrix const& that) //оператор присваивания
    {
        for (int i = 0; i < size; i++) {
            delete[] mx[i];
        }
        delete[] mx;
        size = that.size;
        mx = new int* [size];
        if (this != &that) {
            for (int i = 0; i < size; i++) {
                mx[i] = new int[size];
                for (int j = 0; j < size; j++) {
                    mx[i][j] = that.mx[i][j];
                }
            }
        }
        return *this;
    }
    ~Matrix() {
        for (int i = 0; i < size; i++)
        {
            delete[] mx[i];
        }
        delete[] mx;
    }
    int get_size() const {
        return size;
    }
    class Column //класс для работы со столбцами матрицы
    {
    private:
        int num; //номер столбца
        Matrix* col;
    public:
        Column(int j, Matrix* a)
        {
            this->num = j;
            this->col = a;
        }
        int& operator [] (int i)
        {
            return this->col->mx[i][this->num];
        }
    };
    Column operator () (int j) //оператор взятия столбца
    {
        if (j < 0 || j >= this->size) {
            throw "Incorrect position";
        }
        Column col(j, this);
        return col;
    }
    int*& operator [] (int j) const { //оператор взятия строки
        return this->mx[j];
    }
    void print_matrix() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << this->mx[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    void cin_matrix() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cin >> this->mx[i][j];
            }
        }
    }
    Matrix operator () (int i1, int j1) {

        Matrix minor(size - 1);

        int counter_i = 0;  //становятся равными 1 в момент, когда достигается индекс i1 или j1
        int counter_j = 0;
        for (int i = 0; i < size - 1; i++) {
            counter_j = 0;
            if (i == i1) {
                counter_i = 1;
            }
            for (int j = 0; j < size - 1; j++) {
                if (j == j1) {
                    counter_j = 1;
                }
                minor.mx[i][j] = mx[i + counter_i][j + counter_j];
            }
        }
        return minor;
    }
    Matrix operator + (Matrix m) {
        if (m.size != this->size) {
            throw "different sizes of matrixes";
        }
        int size = this->size;
        Matrix sum(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                sum.mx[i][j] = m.mx[i][j] + this->mx[i][j];
            }
        }
        return sum;
    }
    Matrix operator - (Matrix m) {
        if (m.size != this->size) {
            throw "different sizes of matrixes";
        }
        int size = this->size;
        Matrix sum(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                sum.mx[i][j] = this->mx[i][j] - m.mx[i][j];
            }
        }
        return sum;
    }
    Matrix operator * (Matrix m) {
        if (m.size != this->size) {
            throw "different sizes of matrixes";
        }
        int size = this->size;
        Matrix mult(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int sum = 0;
                for (int k = 0; k < size; k++) {
                    sum += this->mx[i][k] * m.mx[k][j];
                }
                mult.mx[i][j] = sum;
            }
        }
        return mult;
    }
    bool operator == (Matrix m) {
        if (m.size != this->size) {
            return false;
        }
        int size = this->size;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (m.mx[i][j] != this->mx[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }
    bool operator != (Matrix m) {
        if (m.size != this->size) {
            return true;
        }
        int size = this->size;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (this->mx[i][j] != m.mx[i][j]) {
                    return true;
                }
            }
        }
        return false;
    }
    Matrix operator ! () const { //оператор транспонирования
        int size = this->size;
        Matrix T(size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                T.mx[i][j] = this->mx[j][i];
            }
        }
        T.print_matrix();
        return T;
    }
};

template<>
struct hash<Matrix> {
    size_t operator () (Matrix const& mx) const noexcept {
        hash<string> hasher;
        size_t hsh = 0;
        int size = mx.get_size();
        for (int i = 0; i < size; i++) {
            string a = "";
            for (int j = 0; j < size; j++) {
                a += mx[i][j];
            }
            hsh += hasher(a);
        }
        hsh /= size;
        return hsh;
    }
};

template <typename K, typename V>
class Node {
public:
    K val1;
    V val2;
    Node<K, V>* next;
    Node<K, V>() {
        next = nullptr;
    }
    Node<K, V>(K val1_, V val2_) {
        val1 = val1_;
        val2 = val2_;
        next = nullptr;
    }
};

template <typename K, typename V>
class List {
private:
    Node<K, V>* head;
    int size;
public:
    List<K, V>() {
        head = nullptr;
        size = 0;
    }
    ~List() {
        while (size) {
            remove(head->val1);
        }
        size = 0;
    }
    void add_d(K val1_, V val2_) {
        Node<K, V>* new_head = new Node<K, V>(val1_, val2_);
        new_head->next = head;
        head = new_head;
        size++;
    }
    Node<K, V> operator [] (int index) {
        Node<K, V>* i = head;
        for (int j = 0; j < index; j++) {
            if (i->next == nullptr) {
                cout << "SIzE = " << size << endl;
                throw "Element does not exist\n";
            }
            i = i->next;
        }
        return *i;
    }
    Node<K, V> get_head() {
        return *head;
    }
    bool remove(K val1_) { //вернет true, если узел удалился
        if (head == nullptr) return false;
        if (head->val1 == val1_) {
            head = head->next;
            size--;
            return true;
        }
        Node<K, V>* node = head->next;
        while (head->val1 != val1_) {
            if (head->val1 == val1_) {
                if (node == nullptr) {
                    head->next = node;
                }
                else {
                    head->next = node->next;
                }
                delete node;
                size--;
                return true;
            }
            if (node == nullptr) {
                return false;
            }
            head = node;
            node = node->next;
        }
        return false;
    }
    bool add(K val1_, V val2_) { //нет элементов с одинаковыми ключами. Вернет true, если ключ был уникальным
        Node<K, V>* new_node = new Node<K, V>(val1_, val2_);
        if (head == nullptr) {
            head = new_node;
            head->next = nullptr;
            size++;
            return true;
        }
        if (head->val1 == val1_) {
            new_node->next = head->next;
            head = new_node;
            return false;
        }
        Node<K, V>* node = head->next;
        if (node == nullptr) {
            new_node->next = nullptr;
            head->next = new_node;
            size++;
            return true;
        }
        while (node->val1 != val1_) {
            if (node->next == nullptr) {
                new_node->next = nullptr;
                node->next = new_node;
                size++;
                return true;
            }
            node = node->next;
        }
        if (node->val1 == val1_) {

            if (node->next == nullptr) {
                new_node->next = nullptr;
                node->next = new_node;
                return false;
            }
            else {
                new_node->next = node->next;
                head->next = new_node;
                return false;
            }
        }
        return false;
    }
    int get_size() { return size; }
};

template <typename K, typename V>
class HashMap {
private:
    const float FULL = 0.75;
    List<K, V>* map;
    int size;
    int alive;
    hash<K> hasher;

    template <typename L, typename M>
    class Iterator {
    private:
        HashMap<L, M>* hm;
        int map_i;
        int list_i;
    public:
        Iterator() {
            hm = nullptr;
        }
        Iterator(HashMap<L, M> & that, int i, int j) {
            hm = &that;
            map_i = i;
            list_i = j;
        }
        Iterator<K, V>& operator ++ () {
            if (map_i >= hm->get_size()) {
                return *this;
            }
            if (list_i + 1 < hm->get_map()[map_i].get_size()) {
                ++list_i;
                return *this;
            }
            list_i = 0;
            do {
                ++map_i;
            } while (hm->get_map()[map_i].get_size() == 0 && map_i < hm->get_size());
            return *this;
        }
        Node<K, V> operator * () {
            if (map_i >= hm->get_size()) {
                throw "Map index > map size in *\n";
            }
            if (list_i >= hm->get_map()[map_i].get_size()) {
                throw "List index > list size in *\n";
            }
            return hm->get_map()[map_i][list_i];
        }
    };

public:
    HashMap() {
        size = 16;
        alive = 0;
        map = new List<K, V>[size];
    }
    ~HashMap() {
        for (int i = 0; i < size; i++) {
            map[i].~List();
        }
        delete[] map;
    }
    template <typename L, typename M>
    void add(L val1_, M val2_) {
        int index = int(hasher(val1_) % (size - 1));
        if (map[index].add(val1_, val2_)) { //увеличиваем количесвто элементов, если ключ был уникальным
            alive++;
        }
        if (alive >= size * FULL) {
            rehash();
        }
    }
    void remove(K val1_) {
        int index = int(hasher(val1_) % (size - 1));
        while (map[index].remove(val1_)) {
            alive--;
        }
    }
    V find(K val1_) {
        int index = int(hasher(val1_) % (size - 1));
        for (int j = 0; j < map[index].get_size(); j++) {
            if (map[index][j].val1 == val1_) {
                return map[index][j].val2;
            }
        }
        throw "There is no element with such key\n";
    }
    int get_size() {
        return size;
    }
    int get_alive() {
        return alive;
    }
    int get_unique() {
        HashMap<V, K> hm_r;
        Iterator<K, V> iter(*this, 0, 0);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < map[i].get_size(); j++) {
                int index = int(hasher((*iter).val1) % (size - 1));
                hm_r.add((*iter).val2, (*iter).val1);
                if (j < map[i].get_size() - 1) {
                    ++iter;
                }
            }
            if (map[i].get_size() || !i) {
                ++iter;
            }
        }
        return hm_r.get_alive();
    }
    List<K, V>* get_map() {
        return map;
    }
    void print(string name = "") {
        Iterator<K, V> iter(*this, 0, 0);
        cout << "~HASH MAP " << name << " ~\n";
        for (int i = 0; i < size; i++) {
            cout << setw(3) << i;
            cout << ") ";
            for (int j = 0; j < map[i].get_size(); j++) {
                cout << "-> ('" << (*iter).val1 << "', " << (*iter).val2 << ") ";
                if (j < map[i].get_size() - 1) {
                    ++iter;
                }
            }
            if (map[i].get_size() || !i) {
                ++iter;
            }
            cout << endl;
        }
        cout << endl;
    }
    void rehash() {
        size *= 2;
        List<K, V>* new_map = new List<K, V>[size];
        Iterator<K, V> iter(*this, 0, 0);
        for (int i = 0; i < size / 2; i++) {
            for (int j = 0; j < map[i].get_size(); j++) {
                int index = int(hasher((*iter).val1) % (size - 1));
                new_map[index].add((*iter).val1, (*iter).val2);
                if (j < map[i].get_size() - 1) {
                    ++iter;
                }
            }
            if (map[i].get_size() || !i) {
                ++iter;
            }
        }
        for (int i = 0; i < size/2; i++) {
            map[i].~List();
        }
        delete[] map;
        map = new_map;
    }
    int co_elem_key(K val1_) {
        int counter = 0;
        int index = int(hasher(val1_) % (size - 1));
        for (int i = 0; i < map[index].get_size(); i++) {
            if (map[index][i].val1 == val1_) {
                counter++;
            }
        }
        return counter;
    }
    V* all_elems_key(K val1_) {
        int count = co_elem_key(val1_);
        int index = int(hasher(val1_) % (size - 1));
        V* all = new V[count];
        int j = 0;
        for (int i = 0; i < map[index].get_size() && j < count; i++) {
            if (map[index][i].val1 == val1_) {
                all[j] = map[index][i].val2;
                j++;
            }
        }
        return all;
    }
};

template<typename T> //шаблонная функция, которая будет перегружена для приведения строки к одному из типов: int, double, string
T cast(string s) {}

template<>
int cast<int>(string s) {
    return stoi(s);
}
template<>
double cast<double>(string s) {
    return stod(s);
}
template<>
string cast<string>(string s) {
    return s;
}

template <typename K, typename V>
void init() {
    int count;
    cin >> count;
    string s;
    getline(cin, s); //считывание пустой строки
    string* comands = new string[count];
    for (int i = 0; i < count; i++) {
        getline(cin, comands[i]);
    }
    HashMap<K, V> hm;
    for (int i = 0; i < count; i++) {
        char com = comands[i][0]; //тип команды: A или R
        string key;
        int idx_end = 0; //начало value в строке
        for (int j = 2; j < comands[i].length() && comands[i][j] != ' '; j++) {
            key += comands[i][j];
            idx_end = j + 1;
        }
        K key_K = cast<K>(key);
        if (com == 'A') {
            string value;
            for (int j = idx_end; j < comands[i].length(); j++) {
                value += comands[i][j];
            }
            V value_V = cast<V>(value);
            hm.add(key_K, value_V);
        }
        else if (com == 'R') {
            hm.remove(key_K);

        }
    }
    cout << hm.get_alive() << " " << hm.get_unique();
}


int main()
{
    try
    {
        char s1;
        char s2;
        cin >> s1 >> s2;

        if (s1 == 'I') {
            if (s2 == 'I') init<int, int>();
            else if (s2 == 'D') init<int, double>();
            else if (s2 == 'S') init<int, string>();
        }
        else if (s1 == 'D') {
            if (s2 == 'I') init<double, int>();
            else if (s2 == 'D') init<double, double>();
            else if (s2 == 'S') init<double, string>();
        }
        else if (s1 == 'S') {
            if (s2 == 'I') init<string, int>();
            else if (s2 == 'D') init<string, double>();
            else if (s2 == 'S') init<string, string>();
        }
    }
    catch (const char* ex)
    {
        cout << ex;
    }
    return 0;
}