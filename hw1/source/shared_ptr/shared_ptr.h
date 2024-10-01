#ifndef MY_SHARED_PTR
#define MY_SHARED_PTR

namespace msp{
    template<class T>
    class shared_ptr;
}

template<class T>
class msp::shared_ptr{
public:
    using uint = unsigned int;

private:
    T* ptr;
    uint* ptr_counter;

    void leave_obj(){
        if (ptr_counter != nullptr){
            (*ptr_counter)--;

            if (*ptr_counter == 0){
                if (ptr != nullptr){
                    delete ptr;
                }

                delete ptr_counter;
            }
        }

        ptr = nullptr;
        ptr_counter = nullptr;
    }

public:
    constexpr shared_ptr() noexcept: ptr(nullptr), ptr_counter(nullptr){}

    explicit shared_ptr(T* std_ptr): ptr(std_ptr), ptr_counter(new uint(1)){}

    shared_ptr(const shared_ptr& other) noexcept: ptr(other.ptr), ptr_counter(other.ptr_counter){
        if (ptr_counter){
            ++(*ptr_counter);
        }
    }

    shared_ptr(shared_ptr&& other) noexcept: ptr(other.ptr), ptr_counter(other.ptr_counter){
        other.ptr = nullptr;
        other.ptr_counter = nullptr;
    }

    ~shared_ptr(){
        leave_obj();
    }

    shared_ptr<T>& operator=(const shared_ptr& other) noexcept {
        if (ptr != other.ptr){
            leave_obj();

            ptr = other.ptr;
            ptr_counter = other.ptr_counter;
            if (ptr_counter){
                ++(*ptr_counter);
            }
        }

        return *this;
    }

    shared_ptr<T>& operator=(shared_ptr&& other) noexcept{
        if (ptr != other.ptr){
            leave_obj();

            ptr = other.ptr;
            ptr_counter = other.ptr_counter;

            other.ptr = nullptr;
            other.ptr_counter = nullptr;
        }

        return *this;
    }

    T& operator*() const noexcept{
        return *ptr;
    }

    T* operator->() const noexcept{
        return ptr;
    }

    T* get() const noexcept{
        return ptr;
    }

    int use_count() const {
        return (ptr_counter ? *ptr_counter : 0);
    }

    void reset(T* p = nullptr) {
        leave_obj();

        if (p) {
            ptr = p;
            ptr_counter = new uint(1);
        } else {
            ptr = nullptr;
            ptr_counter = nullptr;
        }
    }

    void swap(shared_ptr& other) noexcept{
        T* ptr_tmp = ptr;
        ptr = other.ptr;
        other.ptr = ptr_tmp;

        uint* counter_tmp = ptr_counter;
        ptr_counter = other.ptr_counter;
        other.ptr_counter = counter_tmp;
    }

    operator bool(void) const noexcept{
        return get() != nullptr;
    }

    bool operator==(const shared_ptr& other) const {
        return ptr == other.ptr;
    }

    bool operator!=(const shared_ptr& other) const {
        return !(*this == other);
    }

    bool operator<(const shared_ptr& other) const {
        return ptr < other.ptr;
    }

    bool operator<=(const shared_ptr& other) const {
        return ptr <= other.ptr;
    }

    bool operator>(const shared_ptr& other) const {
        return ptr > other.ptr;
    }

    bool operator>=(const shared_ptr& other) const {
        return ptr >= other.ptr;
    }

};

#endif