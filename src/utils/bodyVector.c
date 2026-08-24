#include "utils/bodyVector.h"
#include "stddef.h"
#include "stdlib.h"

void vector_init(BodyVector *vec, size_t initial_capacity) {
  vec->data = malloc(initial_capacity * sizeof(Body));
  vec->count = 0;
  vec->capacity = initial_capacity;
}

void vector_add(BodyVector *vec, Body body) {
  if (vec->count >= vec->capacity) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, vec->capacity * sizeof(Body));
  }

  vec->data[vec->count++] = body;
}

Body *vector_get_by_index(BodyVector *vec, size_t index) {
  if (index < vec->count) {
    return &vec->data[index];
  }

  return nullptr;
}

Body *vector_get_by_id(BodyVector *vec, unsigned int id) {
  for (size_t i = 0; i < vec->count; i++) {
    if (vec->data[i].id == id) {
      return &vec->data[i];
    }
  }

  return nullptr;
}

void vector_free(BodyVector *vec) {
  free(vec->data);
  vec->data = NULL;
  vec->count = 0;
  vec->capacity = 0;
}
