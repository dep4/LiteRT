package(
    # copybara:uncomment default_applicable_licenses = ["@org_tensorflow//tensorflow:license"],
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],
)

cc_library(
    name = "tfl_tensor_ref",
    srcs = ["tfl_tensor_ref.cc"],
    hdrs = ["tfl_tensor_ref.h"],
    deps = [
        "//tflite:array",
        "//tflite/core/c:c_api_types",
        "//tflite/core/c:common",
        "//tflite/experimental/ml_adjacent:lib",
        "//tflite/kernels:kernel_util",
        "//tflite/kernels/internal:compatibility",
    ],
)

cc_test(
    name = "tfl_tensor_ref_test",
    srcs = ["tfl_tensor_ref_test.cc"],
    deps = [
        ":tfl_tensor_ref",
        "//tflite:util",
        "//tflite/core/c:c_api_types",
        "//tflite/core/c:common",
        "//tflite/experimental/ml_adjacent:lib",
        "//tflite/kernels:kernel_util",
        "//tflite/kernels:test_util",
        "//tflite/kernels/internal:compatibility",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "extern_call",
    srcs = ["extern_call.cc"],
    hdrs = ["extern_call.h"],
    deps = [
        ":tfl_tensor_ref",
        "//tflite/core/c:c_api_types",
        "//tflite/core/c:common",
        "//tflite/experimental/ml_adjacent:lib",
        "//tflite/experimental/ml_adjacent/algo:crop",
        "//tflite/experimental/ml_adjacent/algo:resize",
        "//tflite/kernels:kernel_util",
    ],
)

cc_test(
    name = "extern_call_test",
    srcs = ["extern_call_test.cc"],
    deps = [
        ":extern_call",
        "//tflite/core/c:c_api_types",
        "//tflite/core/c:common",
        "//tflite/kernels:test_util",
        "//tflite/schema:schema_fbs",
        "@com_google_googletest//:gtest_main",
    ],
)
