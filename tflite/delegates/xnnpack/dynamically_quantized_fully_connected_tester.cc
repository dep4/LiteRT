/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

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

#include "tflite/delegates/xnnpack/dynamically_quantized_fully_connected_tester.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

#include <gtest/gtest.h>
#include "flatbuffers/buffer.h"  // from @flatbuffers
#include "flatbuffers/flatbuffer_builder.h"  // from @flatbuffers
#include "flatbuffers/string.h"  // from @flatbuffers
#include "tensorflow/compiler/mlir/lite/schema/schema_conversion_utils.h"
#include "tflite/c/c_api_types.h"
#include "tflite/core/interpreter_builder.h"
#include "tflite/core/kernels/register.h"
#include "tflite/delegates/xnnpack/xnnpack_delegate.h"
#include "tflite/interpreter.h"
#include "tflite/schema/schema_generated.h"
#include "tflite/version.h"

namespace tflite {
namespace xnnpack {

std::vector<int32_t> DynamicallyQuantizedFullyConnectedTester::OutputShape()
    const {
  EXPECT_NE(input_shape_.size(), 0);
  if (KeepDims()) {
    std::vector<int32_t> output_shape(input_shape_.cbegin(),
                                      input_shape_.cend() - 1);
    output_shape.push_back(OutputChannels());
    return output_shape;
  } else {
    EXPECT_EQ(InputSize() % InputChannels(), 0);
    return std::vector<int32_t>(
        {InputSize() / InputChannels(), OutputChannels()});
  }
}

void DynamicallyQuantizedFullyConnectedTester::Test(
    Interpreter* delegate_interpreter, Interpreter* default_interpreter) const {
  std::random_device random_device;
  auto rng = std::mt19937(random_device());
  auto input_rng =
      std::bind(std::uniform_real_distribution<float>(-10, 10), std::ref(rng));

  float* default_input_data = default_interpreter->typed_input_tensor<float>(0);
  std::generate_n(default_input_data, InputSize(), std::ref(input_rng));

  float* delegate_input_data =
      delegate_interpreter->typed_input_tensor<float>(0);
  std::copy_n(default_input_data, InputSize(), delegate_input_data);

  ASSERT_EQ(default_interpreter->Invoke(), kTfLiteOk);
  ASSERT_EQ(delegate_interpreter->Invoke(), kTfLiteOk);

  float* default_output_data =
      default_interpreter->typed_output_tensor<float>(0);
  float* delegate_output_data =
      delegate_interpreter->typed_output_tensor<float>(0);

  const int num_output_values = ComputeSize(OutputShape());
  int different_output_values = 0;
  // TFLite rounds to nearest with ties to Away. XNNPACK rounds to nearest with
  // ties to even. IEEE 754 states: "Round to nearest, ties to even" is the
  // default for binary floating point and the recommended default for decimal.
  // For this reason, many output values may differ slightly.
  for (size_t i = 0; i < num_output_values; i++) {
    if (std::abs(default_output_data[i] - delegate_output_data[i]) >
        0.005 * std::abs(default_output_data[i])) {
      ++different_output_values;
    }
  }
  if (different_output_values > 0.05 * num_output_values) {
    GTEST_FAIL() << (float)different_output_values / num_output_values * 100.f
                 << "% of output values differ";
  }
}

void DynamicallyQuantizedFullyConnectedTester::Test(
    TfLiteDelegate* delegate) const {
  std::vector<char> buffer = CreateTfLiteModel();
  const Model* model = GetModel(buffer.data());

  std::unique_ptr<Interpreter> delegate_interpreter;
  ASSERT_EQ(
      InterpreterBuilder(
          model,
          ::tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates())(
          &delegate_interpreter),
      kTfLiteOk);
  std::unique_ptr<Interpreter> default_interpreter;
  ASSERT_EQ(
      InterpreterBuilder(
          model,
          ::tflite::ops::builtin::BuiltinOpResolverWithoutDefaultDelegates())(
          &default_interpreter),
      kTfLiteOk);

  ASSERT_TRUE(delegate_interpreter);
  ASSERT_TRUE(default_interpreter);

  ASSERT_EQ(delegate_interpreter->inputs().size(), 1);
  ASSERT_EQ(default_interpreter->inputs().size(), 1);

  ASSERT_EQ(delegate_interpreter->outputs().size(), 1);
  ASSERT_EQ(default_interpreter->outputs().size(), 1);

  ASSERT_EQ(delegate_interpreter->AllocateTensors(), kTfLiteOk);
  ASSERT_EQ(default_interpreter->AllocateTensors(), kTfLiteOk);

  ASSERT_EQ(delegate_interpreter->ModifyGraphWithDelegate(delegate), kTfLiteOk);

  if (weights_cache_ != nullptr) {
    TfLiteXNNPackDelegateWeightsCacheFinalizeHard(weights_cache_);
  }

  Test(delegate_interpreter.get(), default_interpreter.get());
}

std::vector<char> DynamicallyQuantizedFullyConnectedTester::CreateTfLiteModel()
    const {
  std::random_device random_device;
  auto rng = std::mt19937(random_device());
  auto filter_rng = std::bind(std::uniform_int_distribution<int32_t>(
                                  -std::numeric_limits<int8_t>::max(),
                                  std::numeric_limits<int8_t>::max()),
                              std::ref(rng));
  auto bias_rng =
      std::bind(std::uniform_real_distribution<float>(-10, 10), std::ref(rng));

  flatbuffers::FlatBufferBuilder builder;

  /*************************** Define operator codes **************************/
  const std::array<flatbuffers::Offset<OperatorCode>, 1> operator_codes{
      {CreateOperatorCode(builder, BuiltinOperator_FULLY_CONNECTED)}};
  std::vector<flatbuffers::Offset<Operator>> operators;

  /*********************** Generate filter and bias data **********************/
  int filter_size_bytes = -1;
  switch (WeightsType()) {
    case WeightsType::kChannelWiseQuantizedInt4: {
      filter_size_bytes = (InputChannels() * OutputChannels() + 1) / 2;
      break;
    }
    case WeightsType::kChannelWiseQuantizedInt8:
    case WeightsType::kTensorWiseQuantizedInt8: {
      filter_size_bytes = InputChannels() * OutputChannels();
      break;
    }
  }
  std::vector<int8_t> filter_data(filter_size_bytes);
  std::generate(filter_data.begin(), filter_data.end(), std::ref(filter_rng));
  std::vector<float> bias_data(OutputChannels());
  std::generate(bias_data.begin(), bias_data.end(), std::ref(bias_rng));

  /****************************** Define buffers ******************************/
  const std::array<flatbuffers::Offset<Buffer>, 3> buffersq{{
      CreateBuffer(builder, builder.CreateVector({})),
      CreateBuffer(builder,
                   builder.CreateVector(
                       reinterpret_cast<const uint8_t*>(filter_data.data()),
                       sizeof(int8_t) * filter_data.size())),
      CreateBuffer(builder,
                   builder.CreateVector(
                       reinterpret_cast<const uint8_t*>(bias_data.data()),
                       sizeof(float) * bias_data.size())),
  }};

  /****************************** Define tensors ******************************/
  const std::array<int32_t, 2> filter_shape{
      {OutputChannels(), InputChannels()}};
  const std::array<int32_t, 1> bias_shape{{OutputChannels()}};
  const std::vector<int32_t> output_shape = OutputShape();
  std::vector<flatbuffers::Offset<Tensor>> tensors;
  tensors.emplace_back(CreateTensor(
      builder,
      builder.CreateVector<int32_t>(InputShape().data(), InputShape().size()),
      TensorType_FLOAT32, /*buffer=*/0));
  tflite::TensorType filter_tensor_type;
  std::vector<float> filter_scale;
  std::vector<int64_t> filter_zero_point;
  switch (WeightsType()) {
    case WeightsType::kChannelWiseQuantizedInt4:
      filter_tensor_type = tflite::TensorType_INT4;
      filter_scale.assign(OutputChannels(), FilterScale());
      filter_zero_point.assign(OutputChannels(), 0);
      break;
    case WeightsType::kChannelWiseQuantizedInt8:
      filter_tensor_type = tflite::TensorType_INT8;
      filter_scale.assign(OutputChannels(), FilterScale());
      filter_zero_point.assign(OutputChannels(), 0);
      break;
    case WeightsType::kTensorWiseQuantizedInt8: {
      filter_tensor_type = tflite::TensorType_INT8;
      filter_scale = {FilterScale()};
      filter_zero_point = {0};
      break;
    }
  }
  tensors.emplace_back(CreateTensor(
      builder,
      builder.CreateVector<int32_t>(filter_shape.data(), filter_shape.size()),
      filter_tensor_type, /*buffer=*/1, /*name=*/0,
      CreateQuantizationParameters(
          builder, /*min=*/0, /*max=*/0,
          builder.CreateVector<float>(filter_scale),
          builder.CreateVector<int64_t>(filter_zero_point))));
  if (HasBias()) {
    tensors.emplace_back(CreateTensor(
        builder,
        builder.CreateVector<int32_t>(bias_shape.data(), bias_shape.size()),
        TensorType_FLOAT32, /*buffer=*/2));
  }
  tensors.emplace_back(CreateTensor(
      builder,
      builder.CreateVector<int32_t>(output_shape.data(), output_shape.size()),
      TensorType_FLOAT32));

  /***************************** Define operators *****************************/
  flatbuffers::Offset<FullyConnectedOptions> fully_connected_options =
      CreateFullyConnectedOptions(
          builder, Activation(), FullyConnectedOptionsWeightsFormat_DEFAULT,
          KeepDims(), /*asymmetric_quantize_inputs=*/true);

  std::vector<int32_t> op_inputs{{static_cast<int32_t>(tensors.size()) - 3,
                                  static_cast<int32_t>(tensors.size()) - 2}};
  if (HasBias()) {
    op_inputs.insert(op_inputs.begin(),
                     static_cast<int32_t>(tensors.size()) - 4);
  }
  const std::array<int32_t, 1> op_outputs{
      {static_cast<int32_t>(tensors.size()) - 1}};
  operators.emplace_back(CreateOperator(
      builder, /*opcode_index=*/0,
      builder.CreateVector<int32_t>(op_inputs.data(), op_inputs.size()),
      builder.CreateVector<int32_t>(op_outputs.data(), op_outputs.size()),
      BuiltinOptions_FullyConnectedOptions, fully_connected_options.Union()));

  /****************************** Define subgraph *****************************/
  const std::array<int32_t, 1> subgraph_inputs{
      {static_cast<int>(tensors.size()) - 3 - static_cast<int>(HasBias())}};
  const std::array<int32_t, 1> subgraph_outputs{
      {static_cast<int>(tensors.size()) - 1}};
  flatbuffers::Offset<SubGraph> subgraph = CreateSubGraph(
      builder, builder.CreateVector(tensors.data(), tensors.size()),
      builder.CreateVector<int32_t>(subgraph_inputs.data(),
                                    subgraph_inputs.size()),
      builder.CreateVector<int32_t>(subgraph_outputs.data(),
                                    subgraph_outputs.size()),
      builder.CreateVector(operators.data(), operators.size()));

  flatbuffers::Offset<flatbuffers::String> description =
      builder.CreateString("Fully Connected model");

  flatbuffers::Offset<Model> model_buffer = CreateModel(
      builder, TFLITE_SCHEMA_VERSION,
      builder.CreateVector(operator_codes.data(), operator_codes.size()),
      builder.CreateVector(&subgraph, 1), description,
      builder.CreateVector(buffersq.data(), buffersq.size()));

  builder.Finish(model_buffer);

  return std::vector<char>(builder.GetBufferPointer(),
                           builder.GetBufferPointer() + builder.GetSize());
}

int32_t DynamicallyQuantizedFullyConnectedTester::ComputeSize(
    const std::vector<int32_t>& shape) {
  return std::accumulate(shape.cbegin(), shape.cend(), 1,
                         std::multiplies<int32_t>());
}

}  // namespace xnnpack
}  // namespace tflite
