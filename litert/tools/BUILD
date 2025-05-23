# Copyright 2024 Google LLC.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

load("//litert/vendors/qualcomm:qualcomm_build_defs.bzl", "litert_cc_bin_with_qnn")

package(
    # copybara:uncomment default_applicable_licenses = ["@org_tensorflow//tensorflow:license"],
    default_visibility = ["//litert:__subpackages__"],
)

cc_library(
    name = "apply_plugin",
    srcs = ["apply_plugin.cc"],
    hdrs = ["apply_plugin.h"],
    deps = [
        ":dump",
        ":outstream",
        ":tool_display",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
        "//litert/c:litert_common",
        "//litert/c:litert_logging",
        "//litert/c:litert_model",
        "//litert/cc:litert_buffer_ref",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_macros",
        "//litert/cc:litert_model",
        "//litert/compiler/plugin:compiler_flags",
        "//litert/compiler/plugin:compiler_plugin",
        "//litert/core/model:model_serialize",
        "//litert/core/util:flatbuffer_tools",
    ],
)

cc_test(
    name = "apply_plugin_test",
    srcs = ["apply_plugin_test.cc"],
    data = [
        "//litert/test:mlir_test_data",
        "//litert/vendors/examples:example_plugin_so",
    ],
    tags = [
        "noasan",
        "nomsan",
        "nosan",
        "notsan",
    ],
    deps = [
        ":apply_plugin",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/log:absl_check",
        "@com_google_absl//absl/strings:string_view",
        "//litert/c:litert_common",
        "//litert/c:litert_model",
        "//litert/c:litert_op_code",
        "//litert/cc:litert_buffer_ref",
        "//litert/cc:litert_model",
        "//litert/core:build_stamp",
        "//litert/core:dispatch_op_schema",
        "//litert/core/model",
        "//litert/test:common",
        "//litert/test:matchers",
    ],
)

litert_cc_bin_with_qnn(
    name = "apply_plugin_main",
    srcs = ["apply_plugin_main.cc"],
    data = [
        # copybara:uncomment_begin(google-only)
        # "//platforms/darwinn/compiler:compiler_api_wrapper",
        # copybara:uncomment_end
        "//litert/vendors/examples:example_plugin_so",
        "//litert/vendors/google_tensor/compiler:google_tensor_compiler_plugin_so",
        "//litert/vendors/qualcomm/compiler:qnn_compiler_plugin_so",
    ],
    export_litert_only = 1,
    include_system = 1,
    linkstatic = 1,
    # copybara:uncomment malloc = "//base:system_malloc",
    tags = [
        "noasan",
        "nobuilder",
        "nomsan",
        "nosan",
    ],
    ungrte = True,
    deps = [
        ":apply_plugin",
        ":outstream",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//litert/compiler/plugin:compiler_flags",
        "//litert/core:build_stamp",
    ],
)

# Fork of "apply_plugin_main" without the "ungrte" so this tool can be used as part of larger
# integration test pipelines with example_plugin.
cc_binary(
    name = "apply_plugin_main_for_test",
    testonly = 1,
    srcs = ["apply_plugin_main.cc"],
    data = [
        "//litert/vendors/examples:example_plugin_so",
    ],
    linkstatic = 1,
    tags = [
        "noasan",
        "nomsan",
        "nosan",
    ],
    deps = [
        ":apply_plugin",
        ":outstream",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "//third_party/llvm/llvm-project/llvm:Support",
        "//litert/compiler/plugin:compiler_flags",
    ],
)

cc_library(
    name = "tool_display",
    srcs = ["tool_display.cc"],
    hdrs = ["tool_display.h"],
    deps = [
        ":outstream",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_test(
    name = "tool_display_test",
    srcs = ["tool_display_test.cc"],
    data = ["//litert/test:mlir_test_data"],
    deps = [
        ":tool_display",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
    ],
)

cc_library(
    name = "dump",
    srcs = ["dump.cc"],
    hdrs = ["dump.h"],
    deps = [
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/strings:string_view",
        "//litert/c:litert_model",
        "//litert/c:litert_op_code",
        "//litert/compiler/plugin:compiler_plugin",
        "//litert/core/model",
    ],
)

cc_test(
    name = "dump_test",
    srcs = ["dump_test.cc"],
    data = ["//litert/test:mlir_test_data"],
    deps = [
        ":dump",
        "@com_google_googletest//:gtest_main",
        "@com_google_absl//absl/strings:string_view",
        "//litert/core/model",
        "//litert/test:common",
    ],
)

cc_library(
    name = "outstream",
    hdrs = ["outstream.h"],
    deps = [
        "@com_google_absl//absl/strings:string_view",
        "//litert/c:litert_logging",
    ],
)

cc_binary(
    name = "run_model",
    srcs = ["run_model.cc"],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log:absl_log",
        "@com_google_absl//absl/strings:string_view",
        "@com_google_absl//absl/types:span",
        "//litert/c:litert_common",
        "//litert/c:litert_logging",
        "//litert/cc:litert_compiled_model",
        "//litert/cc:litert_environment",
        "//litert/cc:litert_expected",
        "//litert/cc:litert_macros",
        "//litert/cc:litert_model",
    ],
)
