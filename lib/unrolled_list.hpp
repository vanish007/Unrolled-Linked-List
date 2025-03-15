#pragma once

#include <memory>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <initializer_list>

static int cnt = 0;

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using difference_type = ptrdiff_t;
    using size_type = size_t;
    using allocator_type = Allocator;

private:
    struct Node {
        T elements[NodeMaxSize];
        size_t num_elements = 0;
        Node* prev;
        Node* next;
    };

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

    Node* head = nullptr;
    Node* tail = nullptr;
    size_type total_elements_cnt = 0;
    Allocator allocator;
    NodeAllocator node_allocator;

public:
    class iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
    
    private:
        Node* current_node;
        size_type current_pos;

        friend unrolled_list;
    public:
        iterator(Node* node = nullptr, size_type pos = 0)
        : 
            current_node(node), 
            current_pos(pos) 
        {}
        
        ~iterator() = default;

        reference operator*() const {
            return current_node->elements[current_pos];
        }

        pointer operator->() {
            return &(current_node->elements[current_pos]);
        }

        iterator& operator++() {            
            if (current_node) {
                if (current_pos + 1 < current_node->num_elements) {
                    ++current_pos;
                } else {
                    current_node = current_node->next;
                    current_pos = 0;
                }
            }

            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        iterator& operator--() {
            if (!current_node) {
                return *this;
            }

            if (current_pos == 0) {
                current_node = current_node->prev;
                
                if (current_node) {
                    current_pos = current_node->num_elements - 1;
                }
            } else {
                --current_pos;
            }

            return *this;
        }

        iterator operator--(int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const iterator& other) const {
            return current_node == other.current_node && current_pos  == other.current_pos;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    class const_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using const_reference = const T&;
        using const_pointer = const T*;
        using difference_type = ptrdiff_t;
    
    private:
        Node* current_node;
        size_t current_pos;

        friend unrolled_list;
    public:
        const_iterator(Node* node = nullptr, size_t pos = 0)
        : 
            current_node(node), 
            current_pos(pos) 
        {}

        const_iterator(const iterator& other)
        : 
            current_node(other.current_node), 
            current_pos(other.current_pos)
        {}

        const_reference operator*() const {
            return current_node->elements[current_pos];
        }

        const_pointer operator->() const {
            return &(current_node->elements[current_pos]);
        }

        const_iterator& operator++() {
            
            if (current_node) {
                if (current_pos + 1 < current_node->num_elements) {
                    ++current_pos;
                } else {
                    current_node = current_node->next;
                    current_pos = 0;
                }
            }

            return *this;
        }

        const_iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        const_iterator& operator--() {
            if (!current_node) {
                return *this;
            }

            if (current_pos == 0) {
                current_node = current_node->prev;
                
                if (current_node) {
                    current_pos = current_node->num_elements - 1;
                }
            } else {
                --current_pos;
            }

            return *this;
        }

        const_iterator operator--(int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const const_iterator& other) const {
            return current_node == other.current_node && current_pos == other.current_pos;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    unrolled_list() = default;

    unrolled_list(const unrolled_list& other) {
        for (const auto& item : other) {
            push_back(item);
        }
    }

    unrolled_list(const size_type count, const value_type value) {
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }
    
    unrolled_list(std::initializer_list<value_type> il) {
        for (const auto& item : il) {
            push_back(item);
        }
    }

    unrolled_list(const Allocator& alloc) 
    : 
        allocator(alloc),
        node_allocator(alloc) 
    {
        head = nullptr;
        tail = nullptr;
        total_elements_cnt = 0;
    }

    unrolled_list(const unrolled_list& other, const Allocator& alloc)
        : allocator(alloc),
          node_allocator(alloc),
          head(other.head),
          tail(other.tail),
          total_elements_cnt(other.total_elements_cnt)
    {
        other.head = nullptr;
        other.tail = nullptr;
        other.total_elements_cnt = 0;
    }

    template<typename InputIt>
    unrolled_list(InputIt i, InputIt j) {
        unrolled_list temp;

        for (; i != j; ++i) {
            temp.push_back(*i);
        }


        std::swap(temp, *this);
    }

    template<typename InputIt>
    unrolled_list(InputIt i, InputIt j, const allocator_type& alloc)
    :
        allocator(alloc),
        node_allocator(alloc),
        head(nullptr),
        tail(nullptr),
        total_elements_cnt(0)
    {
        unrolled_list temp;
        try {
            for (; i != j; ++i) {
                temp.push_back(*i);
            }
        } catch (...) {
            temp.~unrolled_list();
            throw;
        }
        swap(temp);
    }

    ~unrolled_list() {
        clear();
    }

    unrolled_list& operator=(const unrolled_list& other) {
        if (this != &other) {
            unrolled_list tmp(other);
            swap(tmp);
        }
        
        return *this;
    }
    
    unrolled_list& operator=(std::initializer_list<value_type> il) {
        clear();

        for (const auto& item : il) {
            push_back(item);
        }

        return *this;
    }

    allocator_type get_allocator() {
        return allocator;
    }

    iterator insert(const_iterator pos, const value_type& value) {
        Node* current_node = pos.current_node;
        size_t pos_in_node = pos.current_pos;
    
        if (pos == end()) {
            push_back(value);
            return iterator(tail, tail->num_elements - 1);
        }
    
        if (current_node->num_elements < NodeMaxSize) {
            T temp = value;
    
            size_t i = current_node->num_elements;
            try {
                for (; i > pos_in_node; --i) {
                    current_node->elements[i] = current_node->elements[i - 1];
                }
            } catch (...) {
                for (size_t j = i; j < current_node->num_elements; ++j) {
                    current_node->elements[j] = current_node->elements[j + 1];
                }
                throw;
            }
    
            current_node->elements[pos_in_node] = temp;
            current_node->num_elements++;
            total_elements_cnt++;
            return iterator(current_node, pos_in_node);
        }
    
        Node* new_node = nullptr;
        T* temp_buffer = nullptr;
        try {
            temp_buffer = static_cast<T*>(::operator new((NodeMaxSize + 1) * sizeof(T)));
            size_t temp_index = 0;
    
            for (size_t i = 0; i < pos_in_node; ++i) {
                new (&temp_buffer[temp_index++]) T(current_node->elements[i]);
            }
    
            new (&temp_buffer[temp_index++]) T(value);
    
            for (size_t i = pos_in_node; i < NodeMaxSize; ++i) {
                new (&temp_buffer[temp_index++]) T(current_node->elements[i]);
            }
    
            new_node = node_allocator.allocate(1);
            new_node->num_elements = 0;
            new_node->prev = new_node->next = nullptr;
    
            const size_t split_pos = (NodeMaxSize + 1) / 2;
    
            for (size_t i = 0; i < NodeMaxSize; ++i) {
                current_node->elements[i].~T();
            }
    
            current_node->num_elements = 0;
            for (size_t i = 0; i < split_pos; ++i) {
                new (&current_node->elements[current_node->num_elements++]) T(temp_buffer[i]);
            }
    
            for (size_t i = split_pos; i < temp_index; ++i) {
                new (&new_node->elements[new_node->num_elements++]) T(temp_buffer[i]);
            }
    
            new_node->next = current_node->next;
            new_node->prev = current_node;
            if (current_node->next) current_node->next->prev = new_node;
            current_node->next = new_node;
            if (current_node == tail) tail = new_node;
    
            total_elements_cnt++;
    
            for (size_t i = 0; i < temp_index; ++i) {
                temp_buffer[i].~T();
            }
            ::operator delete(temp_buffer);
    
        } catch (...) {
            if (temp_buffer) {
                for (size_t i = 0; i < NodeMaxSize + 1; ++i) temp_buffer[i].~T();
                ::operator delete(temp_buffer);
            }
            if (new_node) node_allocator.deallocate(new_node, 1);
            throw;
        }

        const size_t kSplitPos = NodeMaxSize / 2;
    
        if (pos_in_node < kSplitPos) {
            return iterator(current_node, pos_in_node);
        } else {
            return iterator(new_node, pos_in_node - kSplitPos);
        }
    }

    iterator insert(const_iterator pos, size_type n, const value_type& value) {
        iterator it = pos;

        for (size_type i = 0; i < n; ++i) {
            it = insert(it, value);
            ++it;
        }

        return it;
    }

    iterator insert(const_iterator pos, std::initializer_list<value_type> il) {
        iterator it = pos;

        for (const auto& item : il) {
            it = insert(it, item);
            ++it;
        }

        return it;
    }

    iterator erase(const_iterator pos) {
        if (pos == end()) {
            return end();
        }

        Node* node = pos.current_node;
        size_t pos_in_node = pos.current_pos;
    
        std::allocator_traits<Allocator>::destroy(allocator, &node->elements[pos_in_node]);
        for (size_t i = pos_in_node; i < node->num_elements - 1; ++i) {
            node->elements[i] = std::move(node->elements[i+1]);
        }
        --node->num_elements;
        --total_elements_cnt;
    
        if (node->num_elements == 0) {
            if (node->prev) node->prev->next = node->next;
            if (node->next) node->next->prev = node->prev;
            if (node == head) head = node->next;
            if (node == tail) tail = node->prev;
            destroy_node(node);
            return iterator(head, 0);
        }
    
        if (node->num_elements < NodeMaxSize / 2) {
            if (node->next && node->next->num_elements + node->num_elements <= NodeMaxSize) {
                merge_with_next(node);
            } else if (node->prev && node->prev->num_elements + node->num_elements <= NodeMaxSize) {
                merge_with_prev(node);
            }
        }
    
        return (pos_in_node < node->num_elements) ? iterator(node, pos_in_node) : iterator(node->next, 0);
    }

    iterator erase(const_iterator first, const_iterator last) {
        iterator it = first;

        while (it != last) {
            it = erase(it);
        }
        
        return it;
    }

    reference front() {
        return head->elements[0];
    }

    const_reference front() const {
        return head->elements[0];
    }

    reference back() {
        return tail->elements[tail->num_elements - 1];
    }

    const_reference back() const {
        return tail->elements[tail->num_elements - 1];
    }

    void push_front(const value_type& t) {
        if (head && head->num_elements < NodeMaxSize) {
            for (size_t i = head->num_elements; i > 0; --i) {
                std::allocator_traits<Allocator>::construct(allocator, head->elements + i,
                std::move_if_noexcept(head->elements[i - 1]));
                std::allocator_traits<Allocator>::destroy(allocator, head->elements + i - 1);
            }
            try {
                std::allocator_traits<Allocator>::construct(allocator, head->elements, t);
            } catch (...) {
                for (size_t i = 0; i < head->num_elements; ++i) {
                    std::allocator_traits<Allocator>::construct(allocator, head->elements + i,
                        std::move_if_noexcept(head->elements[i + 1]));
                    std::allocator_traits<Allocator>::destroy(allocator, head->elements + i + 1);
                }
                throw;
            }
            ++head->num_elements;
            ++total_elements_cnt;
        } else {
            Node* new_node = std::allocator_traits<NodeAllocator>::allocate(node_allocator, 1);
            new_node->num_elements = 0;
            new_node->next = head;
            new_node->prev = nullptr;
            try {
                std::allocator_traits<Allocator>::construct(allocator, new_node->elements, t);
                new_node->num_elements = 1;
                if (head) {
                    head->prev = new_node;
                } else {
                    tail = new_node;
                }
                head = new_node;
                ++total_elements_cnt;
            } catch (...) {
                std::allocator_traits<NodeAllocator>::deallocate(node_allocator, new_node, 1);
                throw;
            }
        }
    }

    void pop_front() noexcept {
        if (!head) return;
        
        std::allocator_traits<Allocator>::destroy(allocator, head->elements);
        for (size_t i = 1; i < head->num_elements; ++i) {
            std::allocator_traits<Allocator>::construct(allocator, head->elements + i - 1,
            std::move_if_noexcept(head->elements[i]));
            std::allocator_traits<Allocator>::destroy(allocator, head->elements + i);
        }
        --head->num_elements;
        --total_elements_cnt;
        
        if (head->num_elements == 0) {
            Node* old_head = head;
            head = head->next;
            if (head) head->prev = nullptr;
            else tail = nullptr;
            std::allocator_traits<NodeAllocator>::deallocate(node_allocator, old_head, 1);
        }
    }

    void push_back(const value_type& t) {
        if (tail && tail->num_elements < NodeMaxSize) {
            try {
                std::allocator_traits<Allocator>::construct(allocator, tail->elements + tail->num_elements, t);
                ++tail->num_elements;
                ++total_elements_cnt;
            } catch (...) {
                throw;
            }
        } else {
            Node* new_node = std::allocator_traits<NodeAllocator>::allocate(node_allocator, 1);
            new_node->num_elements = 0;
            new_node->prev = tail;
            new_node->next = nullptr;
            try {
                std::allocator_traits<Allocator>::construct(allocator, new_node->elements, t);

                ++new_node->num_elements; 
                if (tail) {
                    tail->next = new_node;
                } else {
                    head = new_node;
                }
                tail = new_node;
                ++total_elements_cnt;
            } catch (...) {
                std::allocator_traits<NodeAllocator>::deallocate(node_allocator, new_node, 1);
                throw;
            }
        }
    }

    void pop_back() noexcept {
        if (!tail) return;
        
        --tail->num_elements;
        std::allocator_traits<Allocator>::destroy(allocator, tail->elements + tail->num_elements);
        --total_elements_cnt;
        
        if (tail->num_elements == 0) {
            Node* old_tail = tail;
            tail = tail->prev;
            if (tail) tail->next = nullptr;
            else head = nullptr;
            std::allocator_traits<NodeAllocator>::deallocate(node_allocator, old_tail, 1);
        }
    }

    void clear() noexcept {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;

            for (size_t i = 0; i < current->num_elements; ++i) {
                std::allocator_traits<Allocator>::destroy(allocator, &current->elements[i]);
            }

            // std::allocator_traits<NodeAllocator>::destroy(node_allocator, current);
            std::allocator_traits<NodeAllocator>::deallocate(node_allocator, current, 1);
            current = next;
        }
        tail = nullptr;
        head = nullptr;
        total_elements_cnt = 0;
    }

    reference at(size_type n) {
        if (n >= total_elements_cnt) {
            throw std::out_of_range("unrolled_list::at");
        }

        return *(begin() + n);
    }

    const_reference at(size_type n) const {
        if (n >= total_elements_cnt) {
            throw std::out_of_range("unrolled_list::at");
        }

        return *(begin() + n);
    }

    iterator begin() { 
        return iterator(head, 0); 
    }

    iterator end() { 
        return iterator(nullptr, 0); 
    }

    const_iterator begin() const { 
        return const_iterator(head, 0); 
    }

    const_iterator end() const { 
        return const_iterator(nullptr, 0); 
    }

    const_iterator cbegin() const {
        return begin(); 
    }

    const_iterator cend() const { 
        return end(); 
    }

    reverse_iterator rbegin() { 
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin()); 
    }

    const_reverse_iterator rbegin() const { 
        return const_reverse_iterator(end());
    }
    
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const { 
        return const_reverse_iterator(cend()); 
    }

    const_reverse_iterator crend() const { 
        return const_reverse_iterator(cbegin()); 
    }

    // std::strong_ordering operator<=>(const iterator& other) const
    // requires std::random_access_iterator<iterator> {
    //     return  std::lexicographical_compare_three_way( begin(), end(),
    //                                                     other.begin(), other.end() );
    // }

    bool operator==(const unrolled_list& other) const {
        if (size() != other.size()) {
            return false;
        }

        return std::equal(begin(), end(), other.begin(), other.end());
    }

    void swap(unrolled_list& other) noexcept(
        std::allocator_traits<NodeAllocator>::propagate_on_container_swap::value ||
        std::allocator_traits<NodeAllocator>::is_always_equal::value
    ) {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(total_elements_cnt, other.total_elements_cnt);
        
        if constexpr (std::allocator_traits<NodeAllocator>::propagate_on_container_swap::value) {
            std::swap(node_allocator, other.node_allocator);
            std::swap(allocator, other.allocator);
        }
    }

    size_type size() const noexcept {
        return total_elements_cnt;
    }

    size_type max_size() const noexcept {
        const size_type node_max = std::allocator_traits<NodeAllocator>::max_size(node_allocator);
        const size_type elements_max = node_max * NodeMaxSize;
        return std::min(elements_max, (size_type)std::numeric_limits<difference_type>::max());
    }
    
    bool empty() {
        return begin() == end();
    }


private:
    Node* create_node() {
        Node* node = node_allocator.allocate(1);
        node->prev = node->next = nullptr;
        node->num_elements = 0;
        return node;
    }

    void destroy_node(Node* node) noexcept {
        for (size_t i = 0; i < node->num_elements; ++i) {
            std::allocator_traits<Allocator>::destroy(allocator, &node->elements[i]);
        }
        node_allocator.deallocate(node, 1);
    }

    void merge_with_next(Node* node) {
        Node* next_node = node->next;
        for (size_t i = 0; i < next_node->num_elements; ++i) {
            node->elements[node->num_elements++] = std::move(next_node->elements[i]);
        }
        node->next = next_node->next;
        if (next_node->next) next_node->next->prev = node;
        else tail = node;
        destroy_node(next_node);
    }
};

// template<typename T, size_t NodeMaxSize, typename Allocator>
// std::strong_ordering operator<=>(
//     const unrolled_list<T, NodeMaxSize, Allocator>& lhs, 
//     const unrolled_list<T, NodeMaxSize, Allocator>& rhs 
//     ) 
// requires std::random_access_iterator<
//     typename unrolled_list<T, NodeMaxSize, Allocator>::iterator
//     > {
//     return lhs.operator<=>(rhs);
// }

template<typename T, size_t NodeMaxSize, typename Allocator>
bool operator==( const unrolled_list<T, NodeMaxSize, Allocator>& lhs, 
                 const unrolled_list<T, NodeMaxSize, Allocator>& rhs ) {
    return lhs.operator==(rhs);
}

template<typename T, size_t NodeMaxSize, typename Allocator>
void swap( unrolled_list<T, NodeMaxSize, Allocator>& lhs, 
           unrolled_list<T, NodeMaxSize, Allocator>& rhs ) {
    lhs.swap(rhs);
}
