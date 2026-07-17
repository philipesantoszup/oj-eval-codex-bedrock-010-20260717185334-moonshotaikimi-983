#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>
#include <functional>

namespace sjtu {

template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;
        
        node() : data(nullptr), prev(nullptr), next(nullptr) {}
        node(const T &val) : data(new T(val)), prev(nullptr), next(nullptr) {}
        ~node() {
            // Only delete data if this node owns it
            if (data) {
                delete data;
                data = nullptr;
            }
        }
    };

protected:
    node *head;
    node *tail;
    size_t numElements;

    static void mergeSort(T** arr, size_t left, size_t right) {
        if (left + 1 >= right) return;
        
        size_t mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid, right);
        
        // Merge [left, mid) and [mid, right)
        T** temp = new T*[right - left];
        size_t i = left, j = mid, k = 0;
        
        while (i < mid && j < right) {
            // Use only operator<: a <= b is !(b < a)
            if (!(*(arr[j]) < *(arr[i]))) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
            }
        }
        while (i < mid) {
            temp[k++] = arr[i++];
        }
        while (j < right) {
            temp[k++] = arr[j++];
        }
        
        // Copy back
        for (size_t p = 0; p < k; ++p) {
            arr[left + p] = temp[p];
        }
        
        delete[] temp;
    }

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        if (pos == nullptr) {
            // Insert at the end
            if (tail == nullptr) {
                head = tail = cur;
                cur->prev = cur->next = nullptr;
            } else {
                tail->next = cur;
                cur->prev = tail;
                cur->next = nullptr;
                tail = cur;
            }
        } else {
            // Insert before pos
            cur->next = pos;
            cur->prev = pos->prev;
            if (pos->prev) {
                pos->prev->next = cur;
            } else {
                head = cur;
            }
            pos->prev = cur;
        }
        numElements++;
        return cur;
    }
    
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        if (pos->prev) {
            pos->prev->next = pos->next;
        } else {
            head = pos->next;
        }
        if (pos->next) {
            pos->next->prev = pos->prev;
        } else {
            tail = pos->prev;
        }
        numElements--;
        return pos;
    }

public:
    class const_iterator;
    
    class iterator {
        friend class list;
        friend class const_iterator;
    private:
        node *ptr;
        const list *container;

    public:
        iterator() : ptr(nullptr), container(nullptr) {}
        iterator(node *p, const list *c) : ptr(p), container(c) {}
        
        /**
         * iter++ - post-increment
         */
        iterator operator++(int) {
            if (ptr == nullptr) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        
        /**
         * ++iter - pre-increment
         */
        iterator & operator++() {
            if (ptr == nullptr) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        
        /**
         * iter-- - post-decrement
         */
        iterator operator--(int) {
            if (ptr == nullptr) {
                // Decrementing end() goes to last element
                if (container == nullptr || container->tail == nullptr) throw invalid_iterator();
                iterator tmp = *this;
                ptr = container->tail;
                return tmp;
            }
            if (ptr->prev == nullptr) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        
        /**
         * --iter - pre-decrement
         */
        iterator & operator--() {
            if (ptr == nullptr) {
                // Decrementing end() goes to last element
                if (container == nullptr || container->tail == nullptr) throw invalid_iterator();
                ptr = container->tail;
                return *this;
            }
            if (ptr->prev == nullptr) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        
        /**
         * *it
         */
        T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) throw invalid_iterator();
            return *(ptr->data);
        }
        
        /**
         * it->field
         */
        T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) throw invalid_iterator();
            return ptr->data;
        }
        
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };
    
    /**
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
        friend class list;
    private:
        node *ptr;
        const list *container;
        
    public:
        const_iterator() : ptr(nullptr), container(nullptr) {}
        const_iterator(node *p, const list *c) : ptr(p), container(c) {}
        const_iterator(const iterator &other) : ptr(other.ptr), container(other.container) {}
        
        /**
         * iter++ - post-increment
         */
        const_iterator operator++(int) {
            if (ptr == nullptr) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        
        /**
         * ++iter - pre-increment
         */
        const_iterator & operator++() {
            if (ptr == nullptr) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        
        /**
         * iter-- - post-decrement
         */
        const_iterator operator--(int) {
            if (ptr == nullptr) {
                // Decrementing end() goes to last element
                if (container == nullptr || container->tail == nullptr) throw invalid_iterator();
                const_iterator tmp = *this;
                ptr = container->tail;
                return tmp;
            }
            if (ptr->prev == nullptr) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        
        /**
         * --iter - pre-decrement
         */
        const_iterator & operator--() {
            if (ptr == nullptr) {
                // Decrementing end() goes to last element
                if (container == nullptr || container->tail == nullptr) throw invalid_iterator();
                ptr = container->tail;
                return *this;
            }
            if (ptr->prev == nullptr) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        
        /**
         * *it
         */
        const T & operator *() const {
            if (ptr == nullptr || ptr->data == nullptr) throw invalid_iterator();
            return *(ptr->data);
        }
        
        /**
         * it->field
         */
        const T * operator ->() const {
            if (ptr == nullptr || ptr->data == nullptr) throw invalid_iterator();
            return ptr->data;
        }
        
        /**
         * compare iterators
         */
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };
    
    /**
     * Constructs
     */
    list() : head(nullptr), tail(nullptr), numElements(0) {}
    
    list(const list &other) : head(nullptr), tail(nullptr), numElements(0) {
        for (node *cur = other.head; cur != nullptr; cur = cur->next) {
            node *newNode = new node(*(cur->data));
            insert(nullptr, newNode);
        }
    }
    
    /**
     * Destructor
     */
    virtual ~list() {
        clear();
    }
    
    /**
     * Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (node *cur = other.head; cur != nullptr; cur = cur->next) {
            node *newNode = new node(*(cur->data));
            insert(nullptr, newNode);
        }
        return *this;
    }
    
    /**
     * access the first / last element
     */
    const T & front() const {
        if (empty()) throw container_is_empty();
        return *(head->data);
    }
    
    const T & back() const {
        if (empty()) throw container_is_empty();
        return *(tail->data);
    }
    
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(head, this);
    }
    
    const_iterator cbegin() const {
        return const_iterator(head, this);
    }
    
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(nullptr, this);
    }
    
    const_iterator cend() const {
        return const_iterator(nullptr, this);
    }
    
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const {
        return numElements == 0;
    }
    
    /**
     * returns the number of elements
     */
    virtual size_t size() const {
        return numElements;
    }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *cur = head;
        while (cur != nullptr) {
            node *next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        numElements = 0;
    }
    
    /**
     * insert value before pos
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) throw invalid_iterator();
        
        if (pos.ptr != nullptr) {
            // Validate iterator
            bool valid = false;
            for (node *cur = head; cur != nullptr; cur = cur->next) {
                if (cur == pos.ptr) {
                    valid = true;
                    break;
                }
            }
            if (!valid) throw invalid_iterator();
        }
        
        node *newNode = new node(value);
        insert(pos.ptr, newNode);
        return iterator(newNode, this);
    }
    
    /**
     * remove the element at pos
     */
    virtual iterator erase(iterator pos) {
        if (empty()) throw container_is_empty();
        if (pos.ptr == nullptr) throw invalid_iterator();
        if (pos.container != this) throw invalid_iterator();
        
        // Validate iterator
        bool valid = false;
        for (node *cur = head; cur != nullptr; cur = cur->next) {
            if (cur == pos.ptr) {
                valid = true;
                break;
            }
        }
        if (!valid) throw invalid_iterator();
        
        node *next = pos.ptr->next;
        // Mark data as moved to avoid double free by sort()
        pos.ptr->data = nullptr;
        node *saved = erase(pos.ptr);
        delete saved;
        return iterator(next, this);
    }
    
    /**
     * adds an element to the end
     */
    void push_back(const T &value) {
        insert(nullptr, new node(value));
    }
    
    /**
     * removes the last element
     */
    void pop_back() {
        if (empty()) throw container_is_empty();
        node *toRemove = tail;
        erase(toRemove);
        delete toRemove;
    }
    
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) {
        insert(head, new node(value));
    }
    
    /**
     * removes the first element.
     */
    void pop_front() {
        if (empty()) throw container_is_empty();
        node *toRemove = head;
        erase(toRemove);
        delete toRemove;
    }
    
    /**
     * sort the values in ascending order with operator< of T
     * Uses merge sort on data pointers
     */
    void sort() {
        if (numElements <= 1) return;
        
        // Collect data pointers into array
        T **values = new T*[numElements];
        size_t idx = 0;
        for (node *cur = head; cur != nullptr; cur = cur->next) {
            values[idx++] = cur->data;
            // Mark data as moved
            cur->data = nullptr;
        }
        
        // Merge sort on data pointers
        mergeSort(values, 0, numElements);
        
        // Delete old nodes
        node *cur = head;
        while (cur != nullptr) {
            node *next = cur->next;
            delete cur;
            cur = next;
        }
        
        // Rebuild list with sorted data
        head = tail = nullptr;
        numElements = 0;
        for (size_t i = 0; i < idx; ++i) {
            node *newNode = new node();
            newNode->data = values[i];
            newNode->prev = tail;
            newNode->next = nullptr;
            if (tail) {
                tail->next = newNode;
            } else {
                head = newNode;
            }
            tail = newNode;
            numElements++;
        }
        
        delete[] values;
    }
    
    /**
     * merge two sorted lists into one (both in ascending order)
     */
    void merge(list &other) {
        if (this == &other) return;
        if (other.empty()) return;
        
        node *cur1 = head;
        node *cur2 = other.head;
        node *newHead = nullptr;
        node *newTail = nullptr;
        
        // Merge without copying data
        while (cur1 != nullptr && cur2 != nullptr) {
            node *next = nullptr;
            // Use only operator<: cur1 <= cur2 is equivalent to !(cur2 < cur1)
            if (!(*(cur2->data) < *(cur1->data))) {
                next = cur1;
                cur1 = cur1->next;
            } else {
                next = cur2;
                cur2 = cur2->next;
            }
            
            next->prev = newTail;
            next->next = nullptr;
            if (newTail) {
                newTail->next = next;
            } else {
                newHead = next;
            }
            newTail = next;
        }
        
        // Append remaining
        node *rem = (cur1 != nullptr) ? cur1 : cur2;
        while (rem != nullptr) {
            node *next = rem->next;
            rem->prev = newTail;
            rem->next = nullptr;
            if (newTail) {
                newTail->next = rem;
            } else {
                newHead = rem;
            }
            newTail = rem;
            rem = next;
        }
        
        head = newHead;
        tail = newTail;
        numElements += other.numElements;
        
        // Clear other list without deleting nodes
        other.head = other.tail = nullptr;
        other.numElements = 0;
    }
    
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (numElements <= 1) return;
        
        node *cur = head;
        node *temp = nullptr;
        
        // Swap prev and next for each node
        while (cur != nullptr) {
            temp = cur->prev;
            cur->prev = cur->next;
            cur->next = temp;
            cur = cur->prev; // prev was next
        }
        
        // Swap head and tail
        temp = head;
        head = tail;
        tail = temp;
    }
    
    /**
     * remove all consecutive duplicate elements from the container
     */
    void unique() {
        if (numElements <= 1) return;
        
        node *cur = head;
        while (cur != nullptr && cur->next != nullptr) {
            if (*(cur->data) == *(cur->next->data)) {
                node *toRemove = cur->next;
                cur->next = toRemove->next;
                if (toRemove->next) {
                    toRemove->next->prev = cur;
                } else {
                    tail = cur;
                }
                numElements--;
                delete toRemove;
            } else {
                cur = cur->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
