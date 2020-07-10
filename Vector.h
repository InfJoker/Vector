#include <cstddef>
#include <utility>
#include <memory>
template<typename T>
class TData {
private:
    static T* Alloc(size_t n) {
        return static_cast<T*>(operator new(n * sizeof(T)));
    }

    static void Dealloc(T* buf) {
        operator delete(buf);
    }

    void Swap_with(TData& r) {
        std::swap(Ptr, r.Ptr);
        std::swap(Capacity, r.Capacity);
    }

public:
    T* Ptr = nullptr;
    size_t Capacity = 0;

    TData() = default;

    TData(size_t n) {
        Ptr = Alloc(n);
        Capacity = n;
    }

    TData(const TData& other) = delete;

    TData(TData&& other) {
        Swap_With(other);
    }

    ~TData() {
        Dealloc(Ptr);
    }

    TData& operator=(const TData& other) = delete;

    TData& operator=(TData&& other) {
        Swap_With(other);
        return *this;
    }

    const T& operator[](size_t i) const {
        return Ptr[i];
    }

    T& operator[](size_t i) {
        return Ptr[i];
    }
};
namespace NData {
    template<typename T>
    void Swap(TData<T>& l, TData<T>& r) {
        std::swap(l.Ptr, r.Ptr);
        std::swap(l.Capacity, r.Capacity);
    }
}
template<typename T>
class Vector {
private:
    TData<T> Data;
    size_t Size = 0;

public:
    Vector() = default;

    explicit Vector(size_t n)
    : Data(n)
    , Size(n) {
        std::uninitialized_value_construct_n(
            Data.Ptr, Size);
    }

    Vector(const Vector& other)
    : Data(other.Size)
    , Size(other.Size) {
        std::uninitialized_copy_n(
            other.Data.Ptr, Size, Data.Ptr);
    }

    /*Vector(Vector&& other)
    : Data((other.Data))
    , Size((other.Size)) {}*/


    ~Vector() {
        std::destroy_n(Data.Ptr, Size);
    }

    /*Vector& operator=(Vector&& other) {
        Swap(Data, other.Data);
        std::swap(Size, other.Size);
    }*/
    void swap(Vector& other) {
        NData::Swap(Data, other.Data);
        std::swap(Size, other.Size);
    }

    Vector& operator=(Vector& other) {
        if (other.Size > Data.Capacity) {
            Vector tmp(other);
            swap(tmp);
        } else {
            for (size_t i = 0; i != Size && i != other.Size; ++i) {
                Data[i] = other[i];
            }
            if (Size > other.Size) {
                std::destroy_n(
                    Data.Ptr + other.Size,
                    Size - other.Size);
            }
            if (Size < other.Size) {
                std::uninitialized_copy_n(
                    other.Data.Ptr + Size,
                    other.Size - Size,
                    Data.Ptr + Size);
            }
            Size = other.Size;
        }
        return *this;
    }

    void reserve(size_t n) {
        if (n > Data.Capacity) {
            TData<T> data(n);
            std::uninitialized_move_n(
                Data.Ptr, Size, data.Ptr);
            NData::Swap(Data, data);
            std::destroy_n(data.Ptr, Size);
        }
    }

    void resize(size_t n) {
        reserve(n);
        if (Size < n) {
            std::uninitialized_value_construct_n(
                Data.Ptr + Size,
                n - Size);
        } else if (Size > n) {
            std::destroy_n(
                Data.Ptr + n,
                Size - n);
        }
        Size = n;
    }

    void push_back(const T& elem) {
        if (Size == Data.Capacity) {
            reserve(Size ? 2 * Size : 1);
        }
        new (Data.Ptr + Size) T(elem);
        ++Size;
    }

    void push_back(T&& elem) {
        if (Size == Data.Capacity) {
            reserve(Size ? 2 * Size : 1);
        }
        new (Data.Ptr + Size) T(std::move(elem));
        ++Size;
    }

    void pop_back() {
        std::destroy_at(Data.Ptr + Size - 1);
        --Size;
    }

    void clear() {
        if (Size == 0)
            return;
        resize(0);
    }

    T* begin() {
        return Data.Ptr;
    }

    T* end() {
        return Data.Ptr + Size;
    }

    const T* begin() const {
        return Data.Ptr;
    }

    const T* end() const {
        return Data.Ptr + Size;
    }

    size_t size() const {
        return Size;
    }

    size_t capacity() const {
        return Data.Capacity;
    }

    const T& operator[](size_t i) const {
        return Data[i];
    }

    T& operator[](size_t i) {
        return Data[i];
    }
};
