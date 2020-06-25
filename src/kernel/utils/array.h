#pragma once

#include <types.h>

namespace qos {

  template<typename T>
  class dynamic_array {
    private:
      T* data;
      size_t allocated_space; 
      size_t current_size;

    public:

      using iterator = T*;

      dynamic_array() { }
      ~dynamic_array() { }

      void reserve(size_t num_elements) {

      }
      void push_back(const T& value) {

      }
      void pop_back() {

      }

      iterator begin() { return &data[0]; }
      iterator end() { return &data[current_size]; }

      size_t size() const { return current_size; }
      bool empty() const { return current_size == 0; }
  };

}
