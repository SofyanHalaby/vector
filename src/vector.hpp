/*
 * Copyright (C) 2023 Sofyan Gaber <sofyanhalaby@gmail.com>
 *
 * This code is for demonstration and educational purposes. It 
 * implements a basic vector class with some basic functionalities.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ASD_VECTOR_2023
#define ASD_VECTOR_2023
#include <stdlib.h>
#include <exception>
#include <memory>
#include <new>
#include <algorithm>

namespace asd
{
    /*
    asd::move(T *input, int count, T *output)
    initializes count items in output array from input array by moving ownership
    */
    template <typename T>
    void move(T *input, int count, T *output)
    {
        for (size_t i = 0; i < count; i++)
        {
            new (output + i) T(std::move(input[i]));
        }
    }

    /*
    asd::copy(T *input, int count, T *output)
    initializes count items in output array from input array by copying
    */
    template <typename T>
    void copy(T *input, int count, T *output)
    {
        for (size_t i = 0; i < count; i++)
        {
            new (output + i) T(input[i]);
        }
    }

    /*
    provides basic allocator functionality allocate/deallocate
    */
    template <typename T>
    class allocator
    {
    public:
        allocator() = default;
        virtual ~allocator() noexcept {}

        /*
        allocates raw memory that can hold n T items, 
        the created memory is not initialized 
        it is the responsibility of class clients to initialize their data on this memory
        this is done to avoid unneeded object creation
        - thows bad_alloc exception in case of allocation failure 
        */
        T *allocate(std::size_t n)
        {
            T* ptr = static_cast<T *>(std::malloc(n * sizeof(T)));
            if (ptr == nullptr)
            {
                throw std::bad_alloc();
            }
            return ptr;
        }

        /*
        deallocates the memory pointed by ptr
        */
        void deallocate(T* ptr) noexcept
        {
            free(ptr);
        }
    };

    /*
    class asd::vector provides basic functionality of a vector, however some other functionality not implemente yet
    */
    template <typename T>
    class vector
    {
        std::size_t m_count; // number of items in the container
        std::size_t m_capacity; // number of items container can have without need to reallocation 
        T *m_data_ptr; //pointer to the allocated memory
        allocator<T> m_allocator; 

        void reset() noexcept
        {
            m_count = 0;
            m_capacity = 0;
            m_data_ptr = nullptr;
        }
        
        void init(std::size_t capacity, std::size_t count, T *data_ptr) noexcept
        {
            m_count = count;
            m_capacity = capacity;
            m_data_ptr = data_ptr;
        }

        /*
        allocate bigger memory,
        move the container elements to the new memory portion
        release the old memory
        then updates capacity and data pointer
        */
        void reallocate()
        {
            std::size_t new_capacity = m_capacity == 0 ? 1 : m_capacity * 2;
            T *new_data_ptr = m_allocator.allocate(new_capacity);
            asd::move(m_data_ptr, m_count, new_data_ptr);
            m_allocator.deallocate(m_data_ptr);
            init(new_capacity, m_count, new_data_ptr);
        }

        void destroy() noexcept
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                std::destroy(m_data_ptr, m_data_ptr + m_count);
            }
            m_allocator.deallocate(m_data_ptr);
        }
    public:

        /*
        non-prameterized consructor
        */
        vector() noexcept
        {
            reset();
        }

        /*
        copy consructor
        */
        vector(const vector &other)
        {
            if (other.size() == 0)
            {
                reset();
                return;
            }
            m_data_ptr = m_allocator.allocate(other.size());
            asd::copy(other.m_data_ptr, other.size(), m_data_ptr);
            init(other.size(), other.size(), m_data_ptr);
        }

        /*
        move constructor
        */
        vector(vector &&other) noexcept
        {
            init(other.m_capacity, other.m_count, other.m_data_ptr);
            other.reset();
        }
        
        /*
        copy assignment operator
        */
        vector& operator= (const vector& other)
        {
            if(m_capacity < other.m_count)
            {
                T* new_data_ptr = m_allocator.allocate(other.m_count);
                destroy();
                m_data_ptr = new_data_ptr;
            }
            asd::copy(other.m_data_ptr, other.m_count, m_data_ptr);
            init(other.size(), other.size(), m_data_ptr);
            return *this;
        }

        /*
        move assignment operator
        */
        vector& operator= (vector&& other) noexcept
        {
            destroy();
            init(other.m_capacity, other.m_count, other.m_data_ptr);
            other.reset();
        }

        /*
        destructor
        */
        virtual ~vector() noexcept
        {
            destroy();
        }

        /*
        add new item
        it accepts universal reference which binds to both lvalue and rvalue references
        it raises the bad_alloc exception
        it doesn't provide special handling/cleaning up so that API clien can do handling as per thier preference
        */
        template <class U>
        void push_back(U &&item)
        {
            if (m_count == m_capacity)
            {
                reallocate();
            }
            new (m_data_ptr + m_count) T(std::forward<U>(item));
            ++m_count;
        }

        /*
        creates new item and from passed arguments and adds it to the container
        it raises the bad_alloc exception
        it doesn't provide special handling/cleaning up so that API clien can do handling as per thier preference
        */
        template <typename... Args>
        void emplace_back(Args... args)
        {
            if (m_count == m_capacity)
            {
                reallocate();
            }
            new (m_data_ptr + m_count) T(std::forward<Args>(args)...);
            ++m_count;
        }

        /*
        doesn't provide boundary checks to give faster access, API client should take care 
        */
        T &operator[](std::size_t idx)
        {
            return m_data_ptr[idx];
        }

        /*
        doesn't provide boundary checks to give faster access, API client should take care 
        */
        const T &operator[](std::size_t idx) const
        {
            return m_data_ptr[idx];
        }

        std::size_t size() const noexcept
        {
            return m_count;
        }

        std::size_t capacity() const noexcept
        {
            return m_capacity;
        }
    };
}
#endif //ifndef ASD_VECTOR_2023

