#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//NOTA Non localizzata:
//Name return value application
//vector v=read_vector(argv[1]); userà un solo costruttore 
//se si fa prima la dichiarazione poi la chiamata verrà creato il secondo oggetto
int compare_integers(const void* a, const void* b)
{
    int ia = *(int*)a;
    int ib = *(int*)b;
    if (ia < ib) {
        return -1;
    }
    else if (ia > ib) {
        return 1;
    }
    else {
        return 0;
    }
}

struct vector {
    size_t capacity_;
    size_t size_;
    int* data_;

    vector() {
        capacity_ = 10; // Initial capacity
        size_ = 0;
        data_ = (int*)malloc(capacity_ * sizeof(int));
    }
    /*costruttore vuoto
    costruttore di copia
    costruttore con parametri*/
    vector(const vector& other)
    {
        capacity_ = other.capacity_;
        size_ = other.size_;
        data_ = (int*)malloc(capacity_ * sizeof(int));
        for (size_t i = 0; i < size_; i++)
        {
            data_[i] = other.data_[i];
        }

    }
    //assignment operator
    vector& operator=(const vector& rhs)
    {
        if (&rhs!=this)
        {
            free(data_);
            capacity_ = rhs.capacity_;
            size_ = rhs.size_;
            data_ = (int*)malloc(capacity_ * sizeof(int));
            for (size_t i = 0; i < size_; i++)
            {
                data_[i] = rhs.data_[i];
            }
        }
        return *this;
    }
    ~vector() { 
        free(data_);
        /*stare attenti con gli oggetti chiamati come parametri
        perchè le copie puntano alla stessa porzione di memoria
        shallow copy
        quindi il distruttore partirà verso la copia originale*/
    }
    void push_back(int num) {
        // Resize the array if necessary
        if (size_ == capacity_) {
            capacity_ *= 2;
            data_ = (int*)realloc(data_, capacity_ * sizeof(int));
        }
        data_[size_] = num;
        size_++;
    }
    void sort() {
        qsort(data_, size_, sizeof(int), compare_integers);
    }
    int at(int i) const {
        assert(i >= 0 && i < size_);
        return data_[i];
    }
    
    const int& operator[](int i) const { //il primo parametro sarà un puntatore all'oggetto const
        return data_[i];
    }

    size_t size() const {
        return size_;
    }
};
void printVector(FILE* f, const vector& v)
    {
        for (int i = 0; i < v.size(); i++) {
            fprintf(f, "%d\n", v[i]);
        }
    }
void change(int& x)
{
    x = 7;
}
int x;
int& access()
{
    return x;
}
void print(int x)
{
    printf("%i\n",x);
}
void print(double x)
{
    printf("%f\n",x);
}
int main(int argc, char* argv[])
{//7 7
     print(67);
    print(34.56);
    //reference example in c++
    /*int x = 5;
    int& y = x;
    change(x);
    access() = 5;
    *///definizione ed assegnazione sono due cose completamente diverse

    if (argc != 3) {
        return 1;
    }

    FILE* input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        return 1;
    }

    FILE* output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        fclose(input_file);
        return 1;
    }

    vector v;
    vector x = v;//copy constructor
    vector y(v);
    x = v;//shallow copy

    int num;
    while (fscanf(input_file, "%d", &num) == 1) {
        v.push_back(num);
    }
    fclose(input_file);
    v.data_[0] = 0;
    // Sort the numbers
    v.sort();
    // Write sorted numbers to the output file
    printVector(output_file, v);
    fclose(output_file);

    return 0;
}
