extern "C" {
#include "vector.h"
}

#include "gtest/gtest.h"

TEST(NullArguments, allFunctions) {
    vector_t* vector;

    vector = vector_create(5);

    int val = 0;
    void* ptr;

    EXPECT_EQ(vector_destroy(nullptr), VECTOR_FAILURE);
    EXPECT_EQ(vector_push_end(nullptr, &val), VECTOR_FAILURE);
    EXPECT_EQ(vector_push_begin(nullptr, &val), VECTOR_FAILURE);
    EXPECT_EQ(vector_pop_begin(nullptr, &ptr), VECTOR_FAILURE);
    EXPECT_EQ(vector_pop_end(nullptr, &ptr), VECTOR_FAILURE);
    EXPECT_EQ(vector_peek_begin(nullptr, &ptr), VECTOR_FAILURE);
    EXPECT_EQ(vector_status(nullptr), VECTOR_FAILURE);
    EXPECT_EQ(vector_status(nullptr), VECTOR_FAILURE);

    vector_destroy(vector);
}

TEST(SmokeTest, pushPeekPop)
{
    vector_t* vector = nullptr;

    int var1 = 10;
    void* var2;

    vector = vector_create(5);

    EXPECT_EQ(vector_push_end(vector, &var1), VECTOR_SUCCESS);
    EXPECT_EQ(vector_peek_begin(vector, &var2), VECTOR_SUCCESS);

    EXPECT_EQ(&var1, var2);
    var2 = NULL;

    EXPECT_EQ(vector_pop_begin(vector, &var2), VECTOR_SUCCESS);
    EXPECT_EQ(&var1, var2);

    vector_destroy(vector);
}

TEST(FullQueue, pushReturnsOverflow)
{
    double var1 = 1;

    vector_t* vector = nullptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &var1 + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &var1 + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &var1 + 2), VECTOR_SUCCESS);

    EXPECT_EQ(vector_push_end(vector, &var1 + 3), VECTOR_OVERFLOW);

    vector_destroy(vector);
}

TEST(FullQueue, pushOverwritesAtFront)
{
    vector_t* vector = nullptr;
    float var1 = 5;
    void* var2;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &var1 + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &var1 + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &var1 + 2), VECTOR_SUCCESS);

    ASSERT_EQ(vector_push_end(vector, &var1 + 3), VECTOR_OVERFLOW);

    ASSERT_EQ(vector_pop_begin(vector, &var2), VECTOR_SUCCESS);
    ASSERT_EQ(vector_pop_begin(vector, &var2), VECTOR_SUCCESS);
    ASSERT_EQ(vector_pop_begin(vector, &var2), VECTOR_SUCCESS);
    EXPECT_EQ(var2, &var1 + 3);

    vector_destroy(vector);
}

TEST(EmptyQueue, popReturnsUnderflow)
{
    vector_t* vector = nullptr;
    void* ptr;

    vector = vector_create(3);

    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_UNDERFLOW);

    vector_destroy(vector);
}

TEST(EmptyQueue, popUnderflowDoesNotChangeValueOfArgument)
{
    vector_t* vector = nullptr;
    void* ptr = nullptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_UNDERFLOW);
    EXPECT_EQ(ptr, nullptr);

    vector_destroy(vector);
}

TEST(ReuseAfterUnderflow, pushPop)
{
    vector_t* vector = nullptr;
    int val = 101;
    void* ptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &val), VECTOR_SUCCESS);
    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_UNDERFLOW);
    EXPECT_EQ(vector_push_end(vector, &val), VECTOR_SUCCESS);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(&val, ptr);

    vector_destroy(vector);
}

TEST(EmptyQueue, peekReturnsUnderflow)
{
    vector_t* vector = nullptr;
    void* ptr;

    vector = vector_create(3);

    EXPECT_EQ(vector_peek_begin(vector, &ptr), VECTOR_UNDERFLOW);

    vector_destroy(vector);
}

TEST(EmptyQueue, peekUnderflowDoesNotChangeValueOfArgument)
{
    vector_t* vector = nullptr;
    int val = 101;
    void* ptr = &val;

    vector = vector_create(3);

    ASSERT_EQ(vector_peek_begin(vector, &ptr), VECTOR_UNDERFLOW);
    EXPECT_EQ(ptr, &val);

    vector_destroy(vector);
}

TEST(Circulation, vector_capacity_1)
{
    vector_t* vector = nullptr;
    int val = 101;
    void* ptr;

    vector = vector_create(1);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    ASSERT_EQ(&val, ptr);

    EXPECT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(&val + 1, ptr);

    vector_destroy(vector);
}

TEST(Circulation, vector_capacity_2)
{
    vector_t* vector = nullptr;
    int val = 101;
    void* ptr;

    vector = vector_create(2);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);

    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    ASSERT_EQ(&val, ptr);

    EXPECT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    ASSERT_EQ(ptr, &val + 1);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 2);

    vector_destroy(vector);
}

TEST(Circulation, vector_capacity_3)
{
    vector_t* vector = nullptr;
    int val = 101;
    void* ptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);

    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    ASSERT_EQ(&val + 0, ptr);

    EXPECT_EQ(vector_push_end(vector, &val + 3), VECTOR_SUCCESS);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    ASSERT_EQ(&val + 1, ptr);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    ASSERT_EQ(&val + 2, ptr);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(&val + 3, ptr);

    vector_destroy(vector);
}

TEST(Circulation, vector_capacity_4)
{
    vector_t* vector = nullptr;
    int val_i = 101;
    void* ptr_i;

    double val_d = 101.5;
    void* ptr_d;

    vector = vector_create(4);

    ASSERT_EQ(vector_push_end(vector, &val_i), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val_d), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val_i), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val_d), VECTOR_SUCCESS);

    ASSERT_EQ(vector_pop_begin(vector, &ptr_i), VECTOR_SUCCESS);
    ASSERT_EQ(&val_i, ptr_i);

    EXPECT_EQ(vector_push_end(vector, &val_i), VECTOR_SUCCESS);

    EXPECT_EQ(vector_pop_begin(vector, &ptr_d), VECTOR_SUCCESS);
    ASSERT_EQ(&val_d, ptr_d);
    EXPECT_EQ(vector_pop_begin(vector, &ptr_i), VECTOR_SUCCESS);
    ASSERT_EQ(&val_i, ptr_i);
    EXPECT_EQ(vector_pop_begin(vector, &ptr_d), VECTOR_SUCCESS);
    ASSERT_EQ(&val_d, ptr_d);
    EXPECT_EQ(vector_pop_begin(vector, &ptr_i), VECTOR_SUCCESS);
    EXPECT_EQ(&val_i, ptr_i);

    vector_destroy(vector);
}


TEST(isFull, EmptyQueueCapacity1) {
    vector_t* vector = nullptr;

    vector = vector_create(1);
    EXPECT_EQ(vector_status(vector), VECTOR_EMPTY);

    vector_destroy(vector);
}

TEST(isFull, FullQueueCapacity1) {
    vector_t* vector = nullptr;
    int var = 101;
    void* ptr;

    vector = vector_create(1);

    ASSERT_EQ(vector_push_end(vector, &var), VECTOR_SUCCESS);
    EXPECT_EQ(vector_status(vector), VECTOR_FULL);
    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(vector_status(vector), VECTOR_EMPTY);

    vector_destroy(vector);
}

TEST(isFull, EmptyQueue) {
    vector_t* vector = nullptr;

    vector = vector_create(5);

    EXPECT_EQ(vector_status(vector), VECTOR_EMPTY);

    vector_destroy(vector);
}

TEST(isFull, HalfFullQueue) {
    vector_t* vector = nullptr;
    double val = 5.55;

    vector = vector_create(5);

    ASSERT_EQ(vector_push_end(vector, &val), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);

    EXPECT_EQ(vector_status(vector), VECTOR_NONEMPTY);

    vector_destroy(vector);
}

TEST(isFull, FullQueue) {
    vector_t* vector = nullptr;
    char ch = 'a';

    vector = vector_create(5);

    ASSERT_EQ(vector_push_end(vector, &ch + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &ch + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &ch + 2), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &ch + 3), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &ch + 4), VECTOR_SUCCESS);

    EXPECT_EQ(vector_status(vector), VECTOR_FULL);

    vector_destroy(vector);
}

TEST(isEmpty, EmptyQueueCapacity1) {
    vector_t* vector = nullptr;

    vector = vector_create(1);
    EXPECT_EQ(vector_status(vector), VECTOR_EMPTY);

    vector_destroy(vector);
}

TEST(isEmpty, FullQueueCapacity1) {
    vector_t* vector = nullptr;
    int var = 101;
    void* ptr;

    vector = vector_create(1);

    ASSERT_EQ(vector_push_end(vector, &var), VECTOR_SUCCESS);
    EXPECT_EQ(vector_status(vector), VECTOR_FULL);
    ASSERT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(vector_status(vector), VECTOR_EMPTY);

    vector_destroy(vector);
}

TEST(isEmpty, EmptyQueue) {
    vector_t* vector = nullptr;

    vector = vector_create(5);

    EXPECT_EQ(vector_status(vector), VECTOR_EMPTY);

    vector_destroy(vector);
}

TEST(isEmpty, HalfFullQueue) {
    vector_t* vector = nullptr;
    int val = 101;

    vector = vector_create(5);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);

    EXPECT_EQ(vector_status(vector), VECTOR_NONEMPTY);

    vector_destroy(vector);
}

TEST(isEmpty, FullQueue) {
    vector_t* vector = nullptr;
    int val = 101;

    vector = vector_create(5);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 3), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 4), VECTOR_SUCCESS);

    EXPECT_EQ(vector_status(vector), VECTOR_FULL);

    vector_destroy(vector);
}

TEST(FullQueue, PopEnd) {
    vector_t* vector = nullptr;
    int val = 101;
    void* ptr;

    vector = vector_create(5);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 3), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 4), VECTOR_SUCCESS);

    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 4);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 3);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 2);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 1);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 0);

    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_UNDERFLOW);
    EXPECT_EQ(ptr, &val + 0);

    vector_destroy(vector);
}

TEST(OverflowQueue, PopEnd) {
    vector_t* vector = nullptr;
    int val;
    void* ptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 3), VECTOR_OVERFLOW);

    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 3);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 2);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(ptr, &val + 1);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_UNDERFLOW);
    EXPECT_EQ(ptr, &val + 1);

    vector_destroy(vector);
}

TEST(EmptyQueue, pushBegin) {
    vector_t* vector = nullptr;
    int val;
    void* ptr;

    vector = vector_create(3);

    EXPECT_EQ(vector_push_begin(vector, &val), VECTOR_SUCCESS);
    EXPECT_EQ(vector_peek_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);

    vector_destroy(vector);
}

TEST(FullQueue, pushBegin) {
    vector_t* vector = nullptr;
    int val;
    void* ptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);
    EXPECT_EQ(vector_push_begin(vector, &val + 3), VECTOR_OVERFLOW);
    EXPECT_EQ(vector_pop_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(&val + 3, ptr);

    vector_destroy(vector);
}


TEST(EmptyQueue, popEnd) {
    vector_t* vector = nullptr;
    void* ptr;

    vector = vector_create(5);

    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_UNDERFLOW);

    vector_destroy(vector);
}

TEST(FullQueue, popEnd) {
    vector_t* vector = nullptr;
    int val;
    void* ptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 1), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_end(vector, &val + 2), VECTOR_SUCCESS);

    EXPECT_EQ(vector_push_begin(vector, &val + 3), VECTOR_OVERFLOW);
    EXPECT_EQ(vector_pop_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(&val + 1, (int*)ptr);

    vector_destroy(vector);
}

TEST(PeekBeginPeekEnd, pushBeginPushEnd) {
    vector_t* vector = nullptr;
    int val = 101;
    void* ptr;

    vector = vector_create(3);

    ASSERT_EQ(vector_push_end(vector, &val + 0), VECTOR_SUCCESS);
    ASSERT_EQ(vector_push_begin(vector, &val + 1), VECTOR_SUCCESS);

    EXPECT_EQ(vector_peek_end(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(&val + 0, ptr);

    EXPECT_EQ(vector_peek_begin(vector, &ptr), VECTOR_SUCCESS);
    EXPECT_EQ(&val + 1, ptr);

    vector_destroy(vector);
}