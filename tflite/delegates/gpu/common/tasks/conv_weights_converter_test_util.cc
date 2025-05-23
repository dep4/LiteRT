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

#include "tflite/delegates/gpu/common/tasks/conv_weights_converter_test_util.h"

#include <memory>
#include <vector>

#include "tflite/delegates/gpu/common/status.h"
#include "tflite/delegates/gpu/common/task/testing_util.h"
#include "tflite/delegates/gpu/common/task/weights_conversion.h"
#include "tflite/delegates/gpu/common/tasks/conv_weights_converter.h"

namespace tflite {
namespace gpu {
namespace {
absl::Status ConvolutionWeightsConverterTest(
    const Tensor<OHWI, DataType::FLOAT32>& weights,
    const WeightsDescription& weight_desc, TestExecutionEnvironment* env,
    const OperationDef& op_def) {
  // reinterpreting weights as HWIO-BHWC tensor
  TensorFloat32 src_tensor_as_hwio;
  src_tensor_as_hwio.shape =
      BHWC(weights.shape.h, weights.shape.w, weights.shape.i, weights.shape.o);
  src_tensor_as_hwio.data.resize(src_tensor_as_hwio.shape.DimensionsProduct(),
                                 2.0);
  // reinterpreting weights as OHWI-BHWC tensor
  TensorFloat32 src_tensor_as_ohwi;
  src_tensor_as_ohwi.shape =
      BHWC(weights.shape.o, weights.shape.h, weights.shape.w, weights.shape.i);
  src_tensor_as_ohwi.data.resize(src_tensor_as_ohwi.shape.DimensionsProduct(),
                                 2.0);
  for (int o = 0; o < weights.shape.o; ++o) {
    for (int y = 0; y < weights.shape.h; ++y) {
      for (int x = 0; x < weights.shape.w; ++x) {
        for (int i = 0; i < weights.shape.i; ++i) {
          const int f_index = weights.shape.LinearIndex({o, y, x, i});
          const int s_index_hwio =
              src_tensor_as_hwio.shape.LinearIndex({y, x, i, o});
          src_tensor_as_hwio.data[s_index_hwio] = weights.data[f_index];
          const int s_index_ohwi =
              src_tensor_as_ohwi.shape.LinearIndex({o, y, x, i});
          src_tensor_as_ohwi.data[s_index_ohwi] = weights.data[f_index];
        }
      }
    }
  }

  WeightsDescription weight_desc_copy = weight_desc;
  weight_desc_copy.type = DataType::FLOAT32;
  const int flt_count =
      GetTotalElementsCountForLayout(weight_desc_copy, weights.shape);
  DataType weights_type = DataType::FLOAT32;

  std::vector<uint8_t> weights_data(flt_count * SizeOf(weights_type));
  RearrangeWeights(weights, weight_desc_copy, absl::MakeSpan(weights_data));

  std::vector<TensorFloat32> dst_tensors;
  if (weight_desc_copy.layout ==
          WeightsLayout::k2DX4I4YIsSpatialIAndXIsOOGroupO4 ||
      weight_desc_copy.layout ==
          WeightsLayout::k2DX4O4YIsSpatialIAndXIsOOGroupI4) {
    dst_tensors.resize(4);
    const int dst_depth = AlignByN(DivideRoundUp(weights.shape.o, 4),
                                   weight_desc_copy.output_group_size);
    const int src_depth = DivideRoundUp(weights.shape.i, 4);
    const int kernel_x = weights.shape.w;
    const int kernel_y = weights.shape.h;
    int texture_width = dst_depth;
    int texture_height = src_depth * kernel_x * kernel_y;
    int sub_size = SizeOf(weights_type) * 4 * texture_width * texture_height;
    for (int i = 0; i < 4; ++i) {
      dst_tensors[i].shape = BHWC(1, texture_height, texture_width, 4);
      dst_tensors[i].data.resize(4 * texture_width * texture_height);
      memcpy(dst_tensors[i].data.data(), weights_data.data() + sub_size * i,
             sub_size);
    }
  } else {
    dst_tensors.resize(1);
    dst_tensors[0].shape = BHWC(1, 1, 1, flt_count);
    dst_tensors[0].data.resize(flt_count);
    memcpy(dst_tensors[0].data.data(), weights_data.data(),
           flt_count * SizeOf(weights_type));
  }

  std::vector<TensorFloat32> dst_tensors_gpu(dst_tensors.size());
  std::vector<TensorFloat32*> dst_ptrs;
  std::vector<BHWC> dst_shapes;
  for (int i = 0; i < dst_tensors.size(); ++i) {
    dst_shapes.push_back(dst_tensors[i].shape);
    dst_ptrs.push_back(&dst_tensors_gpu[i]);
  }

  auto converter_from_ohwi = ConverterToConvWeights(
      op_def, weight_desc, /*input layout*/ Layout::OHWI);
  RETURN_IF_ERROR(env->ExecuteGPUOperation(
      {src_tensor_as_ohwi},
      std::make_unique<ConverterToConvWeights>(std::move(converter_from_ohwi)),
      dst_shapes, dst_ptrs));
  for (int i = 0; i < dst_tensors.size(); ++i) {
    RETURN_IF_ERROR(
        PointWiseNear(dst_tensors[i].data, dst_tensors_gpu[i].data, 0.0f));
  }

  auto converter_from_hwio = ConverterToConvWeights(
      op_def, weight_desc, /*input layout*/ Layout::HWIO);
  RETURN_IF_ERROR(env->ExecuteGPUOperation(
      {src_tensor_as_hwio},
      std::make_unique<ConverterToConvWeights>(std::move(converter_from_hwio)),
      dst_shapes, dst_ptrs));
  for (int i = 0; i < dst_tensors.size(); ++i) {
    RETURN_IF_ERROR(
        PointWiseNear(dst_tensors[i].data, dst_tensors_gpu[i].data, 0.0f));
  }
  return absl::OkStatus();
}

}  // namespace

absl::Status ConverterToConvWeights1x1OutX4Test(TestExecutionEnvironment* env) {
  const int kSrcChannels = 8;
  const int kDstChannels = 32;
  auto weights_shape = OHWI(kDstChannels, 1, 1, kSrcChannels);
  WeightsDescription conv_weight_desc;
  conv_weight_desc.output_group_size = 4;

  Tensor<OHWI, DataType::FLOAT32> weights;
  weights.shape = weights_shape;
  weights.data.resize(weights_shape.DimensionsProduct());
  for (int i = 0; i < weights.data.size(); ++i) {
    weights.data[i] = half(static_cast<float>(i));
  }

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      for (auto weights_layout : {WeightsLayout::kOSpatialIOGroupI4O4,
                                  WeightsLayout::kOSpatialIOGroupO4I4}) {
        conv_weight_desc.layout = weights_layout;
        OperationDef op_def;
        op_def.precision = precision;
        op_def.src_tensors.push_back({data_type, storage, Layout::BHWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::BUFFER, Layout::UNKNOWN});
        RETURN_IF_ERROR(ConvolutionWeightsConverterTest(
            weights, conv_weight_desc, env, op_def));
      }
    }
  }
  return absl::OkStatus();
}

absl::Status ConverterToConvWeights1x1OutX4UnalignedTest(
    TestExecutionEnvironment* env) {
  const int kSrcChannels = 8;
  const int kDstChannels = 17;
  auto weights_shape = OHWI(kDstChannels, 1, 1, kSrcChannels);
  WeightsDescription conv_weight_desc;
  conv_weight_desc.output_group_size = 4;

  Tensor<OHWI, DataType::FLOAT32> weights;
  weights.shape = weights_shape;
  weights.data.resize(weights_shape.DimensionsProduct());
  for (int i = 0; i < weights.data.size(); ++i) {
    weights.data[i] = half(static_cast<float>(i));
  }

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      for (auto weights_layout : {WeightsLayout::kOSpatialIOGroupI4O4,
                                  WeightsLayout::kOSpatialIOGroupO4I4}) {
        conv_weight_desc.layout = weights_layout;
        OperationDef op_def;
        op_def.precision = precision;
        op_def.src_tensors.push_back({data_type, storage, Layout::BHWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::BUFFER, Layout::UNKNOWN});
        RETURN_IF_ERROR(ConvolutionWeightsConverterTest(
            weights, conv_weight_desc, env, op_def));
      }
    }
  }
  return absl::OkStatus();
}

absl::Status ConverterToConvWeights1x1OutX2Test(TestExecutionEnvironment* env) {
  const int kSrcChannels = 7;
  const int kDstChannels = 37;
  auto weights_shape = OHWI(kDstChannels, 1, 1, kSrcChannels);
  WeightsDescription conv_weight_desc;
  conv_weight_desc.output_group_size = 2;

  Tensor<OHWI, DataType::FLOAT32> weights;
  weights.shape = weights_shape;
  weights.data.resize(weights_shape.DimensionsProduct());
  for (int i = 0; i < weights.data.size(); ++i) {
    weights.data[i] = half(static_cast<float>(i));
  }

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      for (auto weights_layout : {WeightsLayout::kOSpatialIOGroupI4O4,
                                  WeightsLayout::kOSpatialIOGroupO4I4}) {
        conv_weight_desc.layout = weights_layout;
        OperationDef op_def;
        op_def.precision = precision;
        op_def.src_tensors.push_back({data_type, storage, Layout::BHWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::BUFFER, Layout::UNKNOWN});
        RETURN_IF_ERROR(ConvolutionWeightsConverterTest(
            weights, conv_weight_desc, env, op_def));
      }
    }
  }
  return absl::OkStatus();
}

absl::Status ConverterToConvWeightsOutX2Test(TestExecutionEnvironment* env) {
  const int kSrcChannels = 8;
  const int kDstChannels = 38;
  auto weights_shape = OHWI(kDstChannels, 3, 4, kSrcChannels);
  WeightsDescription conv_weight_desc;
  conv_weight_desc.output_group_size = 2;

  Tensor<OHWI, DataType::FLOAT32> weights;
  weights.shape = weights_shape;
  weights.data.resize(weights_shape.DimensionsProduct());
  for (int i = 0; i < weights.data.size(); ++i) {
    weights.data[i] = half(static_cast<float>(i));
  }

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      for (auto weights_layout : {WeightsLayout::kOSpatialIOGroupI4O4,
                                  WeightsLayout::kOSpatialIOGroupO4I4}) {
        conv_weight_desc.layout = weights_layout;
        OperationDef op_def;
        op_def.precision = precision;
        op_def.src_tensors.push_back({data_type, storage, Layout::BHWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::BUFFER, Layout::UNKNOWN});
        RETURN_IF_ERROR(ConvolutionWeightsConverterTest(
            weights, conv_weight_desc, env, op_def));
      }
    }
  }
  return absl::OkStatus();
}

absl::Status ConverterToConvTransposedWeights4x4Test(
    TestExecutionEnvironment* env) {
  const int kSrcChannels = 7;
  const int kDstChannels = 11;
  auto weights_shape = OHWI(kDstChannels, 4, 4, kSrcChannels);
  WeightsDescription weight_desc;
  weight_desc.spatial_remap = {10, 11, 14, 15, 8, 9, 12, 13,
                               2,  3,  6,  7,  0, 1, 4,  5};

  Tensor<OHWI, DataType::FLOAT32> weights;
  weights.shape = weights_shape;
  weights.data.resize(weights_shape.DimensionsProduct());
  for (int i = 0; i < weights.data.size(); ++i) {
    weights.data[i] = half(static_cast<float>(i));
  }

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      for (auto weights_layout : {WeightsLayout::kOICustomSpatialI4O4,
                                  WeightsLayout::kOICustomSpatialO4I4}) {
        weight_desc.layout = weights_layout;
        OperationDef op_def;
        op_def.precision = precision;
        op_def.src_tensors.push_back({data_type, storage, Layout::BHWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::BUFFER, Layout::UNKNOWN});
        RETURN_IF_ERROR(
            ConvolutionWeightsConverterTest(weights, weight_desc, env, op_def));
      }
    }
  }
  return absl::OkStatus();
}

absl::Status ConverterToConvWeights4xTexturesTest(
    TestExecutionEnvironment* env) {
  const int src_channels = 9;
  const int dst_channels = 17;
  auto weights_shape = OHWI(dst_channels, 1, 1, src_channels);
  WeightsDescription conv_weight_desc;
  conv_weight_desc.output_group_size = 4;

  Tensor<OHWI, DataType::FLOAT32> weights;
  weights.shape = weights_shape;
  weights.data.resize(weights_shape.DimensionsProduct());
  for (int i = 0; i < weights.data.size(); ++i) {
    weights.data[i] = half(static_cast<float>(i));
  }

  for (auto precision : env->GetSupportedPrecisions()) {
    auto data_type = DeduceDataTypeFromPrecision(precision);
    for (auto storage : env->GetSupportedStorages(data_type)) {
      for (auto weights_layout :
           {WeightsLayout::k2DX4I4YIsSpatialIAndXIsOOGroupO4,
            WeightsLayout::k2DX4O4YIsSpatialIAndXIsOOGroupI4}) {
        conv_weight_desc.layout = weights_layout;
        OperationDef op_def;
        op_def.precision = precision;
        op_def.src_tensors.push_back({data_type, storage, Layout::BHWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::TEXTURE_2D, Layout::HWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::TEXTURE_2D, Layout::HWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::TEXTURE_2D, Layout::HWC});
        op_def.dst_tensors.push_back(
            {data_type, TensorStorageType::TEXTURE_2D, Layout::HWC});
        RETURN_IF_ERROR(ConvolutionWeightsConverterTest(
            weights, conv_weight_desc, env, op_def));
      }
    }
  }
  return absl::OkStatus();
}

}  // namespace gpu
}  // namespace tflite
