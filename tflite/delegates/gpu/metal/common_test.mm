/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "tflite/delegates/gpu/metal/common.h"

#import <XCTest/XCTest.h>

#include <string>
#include <tuple>
#include <vector>

using ::tflite::gpu::metal::GetBestSupportedMetalDevice;
using ::tflite::gpu::metal::CreateComputeProgram;

@interface CommonTest : XCTestCase

@end

@implementation CommonTest

- (void)testComputeShaderCompilation {
  const std::string code = R"(\
#include <metal_stdlib>
using namespace metal;
kernel void FunctionName(device TYPE* const src_buffer[[buffer(0)]],
                         device TYPE* const dst_buffer[[buffer(1)]],
                         constant int2& size[[buffer(2)]],
                         uint3 gid[[thread_position_in_grid]]) {
  if (int(gid.x) >= size.x || int(gid.y) >= size.y) {
    return;
  }
  const int linear_index = (gid.z * size.y + gid.y) * size.x + gid.x;
  dst_buffer[linear_index] = src_buffer[linear_index];
}
)";

  id<MTLDevice> device = GetBestSupportedMetalDevice();
  XCTAssertNotNil(device, @"The Metal device must exists on real device");
  id<MTLComputePipelineState> program;
  absl::Status status;

  status = CreateComputeProgram(device, code, "FunctionName", {{"TYPE", "float4"}}, &program);
  XCTAssertTrue(status.ok(), @"%s", std::string(status.message()).c_str());
  XCTAssertNotNil(program);

  status = CreateComputeProgram(device, code, "FunctionName", {{"TYPE", "half4"}}, &program);
  XCTAssertTrue(status.ok(), @"%s", std::string(status.message()).c_str());
  XCTAssertNotNil(program);

  // This compilation is intended to be incorrect
  program = nil;
  status = CreateComputeProgram(device, code, "FunctionName", {{"TYPE", "some_undefined_value"}},
                                &program);
  XCTAssertFalse(status.ok(), @"Shader contains an error that has not been detected");
  XCTAssertNil(program);
}

@end
