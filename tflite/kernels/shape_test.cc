/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <stdint.h>

#include <initializer_list>
#include <memory>
#include <vector>

#include <gtest/gtest.h>
#include "tflite/core/interpreter.h"
#include "tflite/kernels/test_util.h"
#include "tflite/schema/schema_generated.h"

namespace tflite {
namespace {

using ::testing::ElementsAreArray;

template <typename T>
class ShapeOpModel : public SingleOpModel {
 public:
  ShapeOpModel(std::initializer_list<int> input_shape, TensorType input_type,
               TensorType output_type) {
    input_ = AddInput(input_type);
    output_ = AddOutput(output_type);
    SetBuiltinOp(BuiltinOperator_SHAPE, BuiltinOptions_ShapeOptions,
                 CreateShapeOptions(builder_, output_type).Union());
    BuildInterpreter({input_shape});
  }

  TfLiteStatus InvokeWithResult() { return interpreter_->Invoke(); }

  int input() { return input_; }

  int32_t GetOutputSize() { return GetTensorSize(output_); }
  std::vector<T> GetOutput() { return ExtractVector<T>(output_); }
  std::vector<int> GetOutputShape() { return GetTensorShape(output_); }
  TfLiteAllocationType GetOutputAllocationType() const {
    return interpreter_->tensor(interpreter_->outputs()[0])->allocation_type;
  }

 private:
  int input_;
  int output_;
};

TEST(ShapeOpTest, OutTypeInt) {
  ShapeOpModel<int32_t> model({1, 3, 1, 3, 5}, TensorType_FLOAT32,
                              TensorType_INT32);
  ASSERT_EQ(model.GetOutputAllocationType(), kTfLitePersistentRo);

  // Unlike most ops, Rank populates outputs in Prepare().
  EXPECT_THAT(model.GetOutput(), ElementsAreArray({1, 3, 1, 3, 5}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({5}));

  // Invoke is superfluous and shouldn't change the output.
  ASSERT_EQ(model.Invoke(), kTfLiteOk);

  EXPECT_THAT(model.GetOutput(), ElementsAreArray({1, 3, 1, 3, 5}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({5}));
}

TEST(ShapeOpTest, OutTypeInt64) {
  ShapeOpModel<int64_t> model({1, 3, 1, 3, 5}, TensorType_FLOAT32,
                              TensorType_INT64);

  EXPECT_THAT(model.GetOutput(), ElementsAreArray({1, 3, 1, 3, 5}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({5}));
}

TEST(ShapeOpTest, ScalarTensor) {
  ShapeOpModel<int32_t> model({}, TensorType_FLOAT32, TensorType_INT32);

  EXPECT_EQ(model.GetOutputSize(), 0);
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({0}));
}

TEST(ShapeOpTest, EmptyTensor) {
  ShapeOpModel<int32_t> model({1, 0}, TensorType_FLOAT32, TensorType_INT32);

  EXPECT_THAT(model.GetOutput(), ElementsAreArray({1, 0}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({2}));
}

}  // namespace
}  // namespace tflite
