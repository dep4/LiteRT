package(
    # copybara:uncomment default_applicable_licenses = ["@org_tensorflow//tensorflow:license"],
    default_visibility = [
        "//visibility:public",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "generator",
    srcs = ["generator.cc"],
    hdrs = ["generator.h"],
    deps = [
        "//tflite/schema:schema_fbs",
    ],
)

cc_binary(
    name = "generate",
    srcs = ["generate.cc"],
    deps = [
        ":generator",
    ],
)

cc_test(
    name = "generator_test",
    srcs = ["generator_test.cc"],
    deps = [
        ":generator",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "consistency_test",
    srcs = ["consistency_test.cc"],
    data = [
        "//tflite:builtin_ops.h",
    ],
    deps = [
        ":generator",
        "@com_google_googletest//:gtest_main",
    ],
)
