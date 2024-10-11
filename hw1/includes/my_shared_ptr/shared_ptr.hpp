#ifndef MY_SHARED_PTR_HPP
#define MY_SHARED_PTR_HPP

#include <compare>


namespace msp{
    template<class T>
    class shared_ptr {
    public:
        using uint = unsigned int;

    private:
        T* ptr;
        uint* ptr_counter;

        void leave_obj();

    public:
        constexpr shared_ptr() noexcept;
        explicit shared_ptr(T* std_ptr);
        shared_ptr(const shared_ptr& other) noexcept;
        shared_ptr(shared_ptr&& other) noexcept;
        ~shared_ptr();

        shared_ptr& operator=(const shared_ptr& other) noexcept;
        shared_ptr& operator=(shared_ptr&& other) noexcept;

        T& operator*() const noexcept;
        T* operator->() const noexcept;

        T* get() const noexcept;
        int use_count() const;
        void reset(T* p = nullptr);
        void swap(shared_ptr& other) noexcept;

        explicit operator bool() const noexcept;
        bool operator==(const shared_ptr& other) const;
        bool operator==(std::nullptr_t) const;
        std::strong_ordering operator<=>(const shared_ptr&) const noexcept;
        std::strong_ordering operator<=>(std::nullptr_t) const noexcept;
    };
}

template<class T>
void msp::shared_ptr<T>::leave_obj() {
    if (ptr_counter != nullptr) {
        (*ptr_counter)--;

        if (*ptr_counter == 0) {
            if (ptr != nullptr) {
                delete ptr;
            }

            delete ptr_counter;
        }
    }

    ptr = nullptr;
    ptr_counter = nullptr;
}

template<class T>
constexpr msp::shared_ptr<T>::shared_ptr() noexcept: ptr(nullptr), ptr_counter(nullptr) {}

template<class T>
msp::shared_ptr<T>::shared_ptr(T* std_ptr): ptr(std_ptr), ptr_counter(new uint(1)) {}

template<class T>
msp::shared_ptr<T>::shared_ptr(const shared_ptr& other) noexcept: ptr(other.ptr), ptr_counter(other.ptr_counter) {
    if (ptr_counter) {
        ++(*ptr_counter);
    }
}

template<class T>
msp::shared_ptr<T>::shared_ptr(shared_ptr&& other) noexcept: ptr(other.ptr), ptr_counter(other.ptr_counter) {
    other.ptr = nullptr;
    other.ptr_counter = nullptr;
}

template<class T>
msp::shared_ptr<T>::~shared_ptr() {
    leave_obj();
}

template<class T>
msp::shared_ptr<T>& msp::shared_ptr<T>::operator=(const shared_ptr& other) noexcept {
    if (this != &other) {
        leave_obj();

        ptr = other.ptr;
        ptr_counter = other.ptr_counter;

        if (ptr_counter) {
            ++(*ptr_counter);
        }
    }
    return *this;
}

template<class T>
msp::shared_ptr<T>& msp::shared_ptr<T>::operator=(shared_ptr&& other) noexcept {
    if (this != &other) {
        leave_obj();

        ptr = other.ptr;
        ptr_counter = other.ptr_counter;

        other.ptr = nullptr;
        other.ptr_counter = nullptr;
    }
    return *this;
}

template<class T>
T& msp::shared_ptr<T>::operator*() const noexcept {
    return *ptr;
}

template<class T>
T* msp::shared_ptr<T>::operator->() const noexcept {
    return ptr;
}

template<class T>
T* msp::shared_ptr<T>::get() const noexcept{
    return ptr;
}

template<class T>
int msp::shared_ptr<T>::use_count() const {
    return (ptr_counter ? *ptr_counter : 0);
}

template<class T>
void msp::shared_ptr<T>::reset(T* p) {
    leave_obj();

    if (p) {
        ptr = p;
        ptr_counter = new uint(1);
    } else {
        ptr = nullptr;
        ptr_counter = nullptr;
    }
}

template<class T>
void msp::shared_ptr<T>::swap(shared_ptr& other) noexcept{
    T* ptr_tmp = ptr;
    ptr = other.ptr;
    other.ptr = ptr_tmp;

    uint* counter_tmp = ptr_counter;
    ptr_counter = other.ptr_counter;
    other.ptr_counter = counter_tmp;
}

template<class T>
msp::shared_ptr<T>::operator bool() const noexcept {
    return ptr != nullptr;
}

template<class T>
bool msp::shared_ptr<T>::operator==(const shared_ptr& other) const {
    return ptr == other.ptr;
}

template<class T>
bool msp::shared_ptr<T>::operator==(std::nullptr_t) const {
    return !ptr;
}

template<class T>
std::strong_ordering msp::shared_ptr<T>::operator<=>(const shared_ptr &obj) const noexcept {
    std::ptrdiff_t ans = ptr - obj.ptr;
    return ans < 0 ? std::strong_ordering::less :
           ans > 0 ? std::strong_ordering::greater :
                     std::strong_ordering::equal;
}

template<class T>
std::strong_ordering msp::shared_ptr<T>::operator<=>(std::nullptr_t) const noexcept {
    return ptr ? std::strong_ordering::greater : std::strong_ordering::equal;
}

#endif