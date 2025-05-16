#ifndef DOUBLY_LIST_HPP
#define DOUBLY_LIST_HPP

#include <iostream>

namespace Linear {
    template <typename T>
    class DoublyList;
    template <typename T>
    class DoublyListIterator;

    template <typename T>
    struct Node {
        T data;
        Node* prev;
        Node* next;

        Node() : data(T()), next(nullptr), prev(nullptr) {}
        Node(const T& val) : data(val), next(nullptr), prev(nullptr) {}
        Node(const T& val, Node* next) : data(val), next(next), prev(nullptr) {}
        Node(const T& val, Node* next, Node* prev) : data(val), next(next), prev(prev) {}

        bool operator==(const Node& other) const {
            return data == other.data;
        }
        bool operator!=(const Node& other) const {
            return !(*this == other);
        }
    };

    template <typename T>
    class DoublyListIterator {
    private:
        Node<T>* current_;

    public:
        explicit DoublyListIterator(Node<T>* node) : current_(node) {}

        T& operator*() const {
            return current_->data;
        }

        DoublyListIterator& operator++() {
            current_ = current_->next;
            return *this;
        }
        DoublyListIterator& operator--() {
            current_ = current_->prev;
            return *this;
        }

        bool operator==(const DoublyListIterator& other) const {
            return current_ == other.current_;
        }
        bool operator!=(const DoublyListIterator& other) const  {
            return !(*this == other);
        }

        friend class DoublyList<T>;
    };

    template <typename T>
    class DoublyList {
    private:
        Node<T>* head_;
        Node<T>* tail_;
        size_t size_;

    private:
        template <typename Compare>
        Node<T>* merge_sort(Node<T>* other, Compare& comp) {
            if (other == tail_ || other->next == tail_) {
                return other;
            }

            Node<T>* slow = other;
            Node<T>* fast = other->next;
            while (fast != tail_ && fast->next != tail_) {
                slow = slow->next;
                fast = fast->next->next;
            }

            Node<T>* mid = slow->next;
            slow->next = tail_;
            mid->prev = head_;
            
            Node<T>* left = merge_sort(other, comp);
            Node<T>* right = merge_sort(mid, comp);

            return merge_sort(left, right, comp);
        }
        template <typename Compare>
        Node<T>* merge_sort(Node<T>* left, Node<T>* right, Compare& comp) {
            Node<T> dummy;
            Node<T>* cur = &dummy;
            
            while (left != tail_ && right != tail_) {
                if (comp(left->data, right->data)) {
                    cur->next = left;
                    left->prev = cur;
                    left = left->next;
                } else {
                    cur->next = right;
                    right->prev = cur;
                    right = right->next;
                }
                cur = cur->next;
            }

            cur->next = (left != tail_) ? left : right;
            
            if (cur->next != tail_) {
                cur->next->prev = cur;
            } else {
                tail_->prev = cur;
            }

            return dummy.next;
        }

    public:
        DoublyList() : size_(0) {
            head_ = new Node<T>();
            tail_ = new Node<T>();

            head_->next = tail_;
            tail_->prev = head_;
        }
        ~DoublyList() {
            clear();
            delete head_;
            delete tail_;
        }

        void push_back(const T& val) {
            Node<T>* cur = new Node<T>(val, tail_, tail_->prev);

            tail_->prev->next = cur;
            tail_->prev = cur;

            size_++;
        }
        void push_front(const T& val) {
            Node<T>* cur = new Node<T>(val, head_->next, head_);

            head_->next->prev = cur;
            head_->next = cur;

            size_++;
        }

        void pop_back() {
            if (empty()) return;

            Node<T>* cur = tail_->prev;
            tail_->prev = cur->prev;
            cur->prev->next = tail_;

            delete cur;
            size_--;
        }
        void pop_front() {
            if (empty()) return;

            Node<T>* cur = head_->next;
            head_->next = cur->next;
            cur->next->prev = head_;

            delete cur;
            size_--;
        }

        void insert(const T& val) {
            push_front(val);
        }
        void insert(const T& val, DoublyListIterator<T> pos) {
            Node<T>* target = pos.current_;
            Node<T>* cur = new Node<T>(val, target, target->prev);

            target->prev->next = cur;
            target->prev = cur;

            size_++;
        }

        void erase() {
            pop_front();
        }
        void erase(DoublyListIterator<T> pos) {
            if (pos == end()) return;

            Node<T>* target = pos.current_;

            target->prev->next = target->next;
            target->next->prev = target->prev;

            delete target;
            size_--;
        }

        T& back() {
            if (empty()) throw std::out_of_range("List is empty");

            return tail_->prev->data;
        }
        T& front() {
            if (empty()) throw std::out_of_range("List is empty");

            return head_->next->data;
        }
        T& at(DoublyListIterator<T> pos) {
            return pos.current_->data;
        }

        void clear() {
            while (!empty()) pop_front();
        }
        bool empty() {
            return size_ == 0;
        }
        size_t size() {
            return size_;
        }

        void splice(DoublyList<T>& other) {
            if (other.empty()) return;
            if (empty()) {
                head_->next = other.head_->next;
                other.head_->next->prev = head_;
                tail_->prev = other.tail_->prev;
                other.tail_->prev->next = tail_;
            } else {
                tail_->prev->next = other.head_->next;
                other.head_->next->prev = tail_->prev;
                tail_->prev = other.tail_->prev;
                other.tail_->prev->next = tail_;
            }

            size_ += other.size_;

            other.head_->next = other.tail_;
            other.tail_->prev = other.head_;
            other.size_ = 0;
        }
        void splice(DoublyList<T>& other, DoublyListIterator<T> pos) {
            if (other.empty()) return;
            if (pos == end()) {
                splice(other);
                return;
            } else {
                pos.current_->prev->next = other.head_->next;
                other.head_->next->prev = pos.current_->prev;
                other.tail_->prev->next = pos.current_;
                pos.current_->prev = other.tail_->prev;
            }

            size_ += other.size_;

            other.head_->next = other.tail_;
            other.tail_->prev = other.head_;
            other.size_ = 0;
        }

        void merge(DoublyList<T>& other) {
            merge(other, [](const T& a, const T& b) { return a < b; });
        }
        template <typename Compare>
        void merge(DoublyList<T>& other, Compare comp) {
            if (this == &other) return;

            DoublyListIterator<T> it1 = begin();
            DoublyListIterator<T> it2 = other.begin();

            while (it2 != other.end()) {
                if (it1 == end() || comp(*it2, *it1)) {
                    insert(*it2, it1);
                    ++it2;
                } else {
                    ++it1;
                }
            }

            other.clear();
            other.head_->next = other.tail_;
            other.tail_->prev = other.head_;
        }

        void remove(const T& val) {
            DoublyListIterator<T> it = begin();
            while (it != end()) {
                if (*it == val) {
                    erase(it);
                } else {
                    ++it;
                }
            }
        }

        void unique() {
            if (size_ <= 1) return;

            DoublyListIterator<T> it = ++begin();
            while (it != end()) {
                if (*it == it.current_->prev->data) {
                    DoublyListIterator<T> cur = it;
                    ++it;

                    erase(cur);
                } else {
                    ++it;
                }
            }
        }
        void reverse() {
            if (size_ <= 1) return;

            Node<T>* current = head_->next;
            head_->next = tail_->prev;
            tail_->prev = current;

            while (current != tail_) {
                std::swap(current->next, current->prev);
                current = current->prev;
            }

            head_->next->prev = head_;
            tail_->prev->next = tail_;
        }

        void sort() {
            sort([](const T& a, const T& b) { return a < b; });
        }
        template <typename Compare>
        void sort(Compare comp) {
            if (size_ <= 1) return;

            Node<T>* data_head = merge_sort(head_->next, comp);

            head_->next = data_head;
            data_head->prev = head_;
        }

        DoublyListIterator<T> end() {
            return DoublyListIterator<T>(tail_);
        }
        DoublyListIterator<T> begin() {
            return DoublyListIterator<T>(head_->next);
        }
    };
}

#endif